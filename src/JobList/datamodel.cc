#include "datamodel.h"

#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <limits>

#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QFile>

#include "domelementwalker.h"
#include "joblistmodel.h"
#include "xml_dom_helper.h"
#include "default_values.h"

////////////////////////////////////////////////////////////////////////////////
//
// Joblist function objects

struct UpdateJobData
{
    QDateTime mNewStart;
    bool      mAdjustToMinute;
    ControlData::AdjustMode mAdjustMode;
    unsigned  mNewId;
    QString   qstrTmp;
    double    dTmp;

    UpdateJobData( QDateTime newStart , bool adjustToMinute , ControlData::AdjustMode adjustMode /* , ControlData::MeasurementMode measurementMode */ ) :
        mNewStart( newStart ),
        mAdjustToMinute( adjustToMinute ),
        mAdjustMode( adjustMode ),
        mNewId( 0 )
        {}

    int  durationOfPauseInSeconds (Job::Spt job)
    {
        int pauseDuration = 0;

        qDebug () << "[" << __PRETTY_FUNCTION__ << "] adjust mode:" << mAdjustMode;

        if (mAdjustMode == ControlData::AdjustAuto)
        {
            switch (job->getMeasurementMode())
            {
                case ControlData::NormalMeasurementMode:
                    pauseDuration = 25;
                    break;

                case ControlData::CSAMTMeasurementMode:
                    pauseDuration = 4 * 60; // at least 4 minutes!
                    break;

                case ControlData::UndefindedMeasurementMode:
                    qDebug() << __PRETTY_FUNCTION__  << " WTF ControlData::UndefindedMeasurementMode ";
                    break;
            }
        }
        else
        {
            pauseDuration = mAdjustMode * 60; // abuse enumvalue --> intvalue
        }

        if ((job->getFrequency (qstrTmp, dTmp) > DefaultValues::LongerPauseFrequency) and
            (pauseDuration < 3 * 60))
        {
            pauseDuration = 3 * 60; // at least 3 minutes!
        }

        return pauseDuration;
    }


    void operator()( Job::Spt job )
    {
        Types::TimeInterval duration = job -> getDuration();
        if (job -> isInCSAMTMode())
        {
            int duration = job->getTXMNumStacks().toUInt() * job->getTXMDipTXTime().toUInt() * job->getTXMNumDipoles().toUInt();
            if (duration > 0)
            {
                job->setDuration(duration);
            }
        }

        job->setId( mNewId++ );
        if (mAdjustMode == ControlData::AdjustIsOff)
        {
            return;
        }

        if (mAdjustToMinute)
        {
            // do not align to next full minute, if we already are on a full minute
            if (mNewStart.time().second() > 0)
            {
                mNewStart = mNewStart.addSecs(60 - mNewStart.time().second());
            }
        }

        job->setStartDateTime(mNewStart);
        duration.addSeconds(durationOfPauseInSeconds(job));
        mNewStart = mNewStart.addSecs(duration.asSeconds()); ; // QDateTime does not have a addMinutes for unkown reasons.

        #ifdef DEBUG_OUTPUT
            std::cout << __PRETTY_FUNCTION__ << " : mNewStart : " << mNewStart.toString().toStdString()
              << " "
              << job -> getStartTime().toString().toStdString()
              << " duration....................: " << duration.asSeconds()
              << " adjust to minute............: " << std::boolalpha << mAdjustToMinute
              << " duration of pause in seconds: " << durationOfPauseInSeconds( job )
              << std::endl;
        #endif
    }
};

struct SaveJobFile
{
    void operator()( Job::Spt job ) {
        job -> saveFile();
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// datamodel

DataModel::DataModel() :
    mJobListModel( 0 ),
    mSelectionModel( 0 ),
    mJobListDocument( "JobList" ),
    mIsReady( false ),
    mJobListStartTimeChanged( false ),
    eJoblistMode (C_JOBLIST_MODE_INV),
    mAdjustToMinute( false ),
    mAdjustMode( ControlData::AdjustIsOff ),
    mJobSelecteionUsesGuiSelectionModel( true ) // we'd change this value only in debug mode. in an ideal world, we'd supply a "selection factory" here...
{
    mJobList.clear();
}

DataModel::~DataModel()
{
    delete mJobListModel;
}

void DataModel::loadAndParseJobListFile(const QString & fileName )
{
    setIsReady( false );
    mJobListFileInfo = fileName;
    QFile jobXmlFile( mJobListFileInfo.canonicalFilePath() );

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Filename : " << fileName;

    mJobListStartTimeChanged = false;
    
    if (not jobXmlFile.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] unable to open file : " << jobXmlFile.fileName();
        throw std::runtime_error( ss.str() );
    }

    if (not mJobListDocument.setContent(&jobXmlFile))
    {
        jobXmlFile.close();
        std::stringstream ss;
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] unable to parse file : " << jobXmlFile.fileName();
        throw std::runtime_error( ss.str() );
    }

    parseJobList();
    loadJobFiles();
    setupSensorData(); // load from job 0 or defaults...
    setIsReady( true );
    emit jobDataLoaded();

}

void DataModel::reset()
{
    mJobList.clear();
    mConfiguredSensorCfg.clear();

    emit jobDataLoaded();

}


void DataModel::loadAndParseHwData( const QDir& dir )
{
    mHardwareConfig.clear();
    mHwDb.clear();


    qDebug () << "[" << __PRETTY_FUNCTION__ << "] HW database dir:" << dir.canonicalPath();
    fflush (stdout);
    fflush (stderr);


    QFileInfo hwDbFileName ( dir , DefaultValues::HwDatabaseFile );
    QFile hwDbFile ( hwDbFileName.absoluteFilePath() );
    mHwDb = HardwareDB::createFromFile( hwDbFile );
    
    QFileInfo hwConfigFileName( dir , DefaultValues::HwConfigFile );
    QFile hwConfigFile( hwConfigFileName.absoluteFilePath() );
    
    mHardwareConfig  = HardwareConfig::createFromFile( hwConfigFile , mHwDb );

    emit sensorConfigLoaded();
    
}


void DataModel::parseJobList()
{
    // declaration of variables
    QString qstrTmp;
    bool    bTmp;

    try
    {
        {
            Tools::DomElementWalker walker( mJobListDocument );
            walker.assertElementName( "ADUConf" );
            walker.goToFirstChild( "TimeFormat" );
            mTimeFormatElement = walker.get();
        }

        {
            Tools::DomElementWalker walker( mJobListDocument );
            walker.assertElementName( "ADUConf" );
            walker.goToFirstChild( "start_time" );
            mStartTimeElement = walker.get();
        }

        {
            Tools::DomElementWalker walker( mJobListDocument );
            walker.assertElementName( "ADUConf" );
            walker.goToFirstChild( "start_date" );
            mStartDateElement = walker.get();
        }

        {
            try
            {
                Tools::DomElementWalker walker (mJobListDocument);
                walker.assertElementName       ("ADUConf");
                walker.goToFirstChild          ("JoblistMode");
                qstrTmp = walker.get           ().firstChild().nodeValue();
                if (qstrTmp.indexOf(C_JOBLIST_MODE_MT_STRING) != -1)
                {
                    this->eJoblistMode = C_JOBLIST_MODE_MT;
                }
                else if (qstrTmp.indexOf (C_JOBLIST_MODE_CSMT_STRING) != -1)
                {
                    this->eJoblistMode = C_JOBLIST_MODE_CSMT;
                }
                else
                {
                    this->eJoblistMode = C_JOBLIST_MODE_INV;
                }
            }
            catch ( const std::runtime_error& e )
            {
                qDebug() << __PRETTY_FUNCTION__  << " Error parsing joblist: " << e.what();
                this->eJoblistMode = C_JOBLIST_MODE_INV;
            }
        }

        {
            try
            {
                Tools::DomElementWalker walker (mJobListDocument);
                walker.assertElementName       ("ADUConf");
                walker.goToFirstChild          ("UTCLocalTimeOffset");
                qstrTmp = walker.get           ().firstChild().nodeValue();

                this->dUTCLocalTime = qstrTmp.toDouble(&bTmp);
                if (bTmp == false)
                {
                    this->dUTCLocalTime = 0.0;
                }

            }
            catch ( const std::runtime_error& e )
            {
                qDebug() << __PRETTY_FUNCTION__  << " Error parsing joblist: " << e.what();
                this->dUTCLocalTime = 0.0;
            }
        }

        // read storage mode parameters (target directory and storage mode)
        {
            Tools::DomElementWalker walker(mJobListDocument);
            walker.assertElementName ("ADUConf");
            walker.goToFirstChild    ("TargetDirectory");
            this->mTargetDirectory = walker.get().firstChild().nodeValue();

            walker.goToParent     ();
            walker.goToFirstChild ("StorageMode");
            this->mStorageMode = walker.get().firstChild().nodeValue();

            if (mStorageMode.compare(Types::qstrlStorageModeStrings [Types::StorageModes::WriteToInternal]) == 0)
            {
                mTargetDirectory = Types::qstrlTargetDirStrings [Types::StorageModes::WriteToInternal];
            }
            else
            {
                mStorageMode     = Types::qstrlStorageModeStrings [Types::StorageModes::WriteToUSB];
                mTargetDirectory = Types::qstrlTargetDirStrings   [Types::StorageModes::WriteToUSB];
            }
        }

        // read CleanJobTable flag
        {
            Tools::DomElementWalker walker(mJobListDocument);
            walker.assertElementName ("ADUConf");
            walker.goToFirstChild    ("CleanJobTable");
            this->mCleanJobTables = walker.get().firstChild().nodeValue();
        }

        // read AdaptChannelConfig flag
        {
            Tools::DomElementWalker walker(mJobListDocument);
            walker.assertElementName ("ADUConf");
            walker.goToFirstChild    ("AdaptChannelConfig");
            this->mAdaptChannelConfig = walker.get().firstChild().nodeValue();
        }

        // read AdaptSensorType flag
        {
            Tools::DomElementWalker walker(mJobListDocument);
            walker.assertElementName ("ADUConf");
            walker.goToFirstChild    ("AdaptSensorType");
            this->mAdaptSensorType = walker.get().firstChild().nodeValue();
        }

        // read AdaptSensorSpacing flag
        {
            Tools::DomElementWalker walker(mJobListDocument);
            walker.assertElementName ("ADUConf");
            walker.goToFirstChild    ("AdaptSensorSpacing");
            this->mAdaptSensorSpacing = walker.get().firstChild().nodeValue();
        }

        parseJobListStartTime();
    }
    catch ( const std::runtime_error& e ) 
    {
        qDebug() << __PRETTY_FUNCTION__  << " Error parsing joblist: " << e.what();
        throw;
    }

}

void DataModel::parseJobListStartTime()
{
    // declaration of variables
    QDateTime startDate;
    QTime     startTime = QTime::fromString( mStartTimeElement.text(), DefaultValues::TimeFormat );

    // map date 0000-00-00 that is used for grid and adjusted / daily modes in
    // JLE to 1970-01-01 as all Qt Date components do not allow dates of values
    // 0 for year, month or day.
    // same is done when writing back file to disk.
    if (mStartDateElement.text().compare("0000-00-00") == 0)
    {
        startDate = QDateTime::fromString("1970-01-01", DefaultValues::DateFormat);
    }
    else
    {
        startDate = QDateTime::fromString( mStartDateElement.text(), DefaultValues::DateFormat );
    }
    
    if ( not startDate.isValid() ) 
        startDate = QDateTime::currentDateTimeUtc();

    if ( startTime < startDate.time() )
        startTime = startDate.time();

    mJobListStartDateTime = QDateTime( startDate.date(), startTime , Qt::UTC );

    // update start-time with UTC to localtime offset
    mJobListStartDateTime = mJobListStartDateTime.addSecs ((qint64) (this->dUTCLocalTime * 3600));
}

void DataModel::loadJobFiles()
{
    mJobList.clear();
    // create some sort of factory... or put this into the JobListModel ctor...
    if ( mJobListModel == 0 ) {
        mJobListModel = new JobListModel( *this );
        // FIXME: move this into the main window where all the signal/slot plumbing happens...!!
        QObject::connect( this , SIGNAL( jobDataChanged() ), mJobListModel, SLOT( dataModelChanged() ) , Qt::UniqueConnection );

        QObject::connect( this , SIGNAL( startInsertJobAtIndex( const Types::Index& ) ), mJobListModel, SLOT( startInsertJobAtIndex( const Types::Index& ) ) , Qt::UniqueConnection );
        QObject::connect( this , SIGNAL( endInsertJob() )                              , mJobListModel, SLOT( endInsertJob() )                               , Qt::UniqueConnection );

        QObject::connect( this , SIGNAL( deleteInsertJobAtIndex( const Types::Index& ) ), mJobListModel, SLOT( deleteInsertJobAtIndex( const Types::Index& ) ), Qt::UniqueConnection );
        QObject::connect( this , SIGNAL( endDeleteJob() )                               , mJobListModel, SLOT( endDeleteJob() )                               , Qt::UniqueConnection );

    }

    // QObject::dumpObjectInfo();

    Tools::DomElementWalker walker( mJobListDocument );
    
    walker.assertElementName( "ADUConf" );

    #ifdef DEBUG_OUTPUT
    std::cout << __PRETTY_FUNCTION__ << " walker : " << walker.getCurrentElementNameStd() << std::endl;
    #endif

    walker
        .goToFirstChild( "JobList" )
        .goToFirstChild( "Job" );

    #ifdef DEBUG_OUTPUT
    std::cout << __PRETTY_FUNCTION__ << " walker : " << walker.getCurrentElementNameStd() << std::endl;
    #endif

    Tools::DomElementSiblingIterator jobIterator( walker );

    while ( jobIterator.isValid() )
    {
        Job::Spt job = Job::create( jobIterator.get() ); // process all Job tags ins equence
        job -> setDataModel( this );
        job -> setJobDirectory( getJobDirectory() );
        job -> loadAndParseFile();        
        mJobList.push_back( job );
        jobIterator.next();
    }

    shiftStartChanged( mJobListStartDateTime ); // this does a "emit dataModelChanged()", so we can omit it here.
}

void DataModel::setupSensorData()
{

    if ( mJobList.empty() ) {
        mConfiguredSensorCfg = SensorChannelConfig::createFromHwDb( getHwConfig() ); 
        return;
    }

    SensorChannelConfig::JobPointerVector jobPtrVector;
    JobVector::const_iterator it = mJobList.begin();
    while ( not ( it == mJobList.end() ) ) {
        jobPtrVector.push_back( (*it).data() );
        ++it;
    }

    mConfiguredSensorCfg = SensorChannelConfig::createFromJobList( jobPtrVector );

}


QAbstractTableModel* DataModel::getJobListModel()
{
    if ( mJobListModel != 0 )
        return mJobListModel;
    
    std::stringstream ss;
    ss << __PRETTY_FUNCTION__ << " JobListModel not yet created!";
    throw std::runtime_error( ss.str() );
    
}

QItemSelectionModel* DataModel::getSelectionModel()
{
    if ( mSelectionModel == 0 ) {
        mSelectionModel = new QItemSelectionModel( getJobListModel() );
    }
    return mSelectionModel;
}


unsigned int DataModel::jobCount() const
{
    // declaration of variables
    int iSize = this->mJobList.size();

    return (iSize);
}

Types::Frequency DataModel::frequnecyOfJob (const Types::Index& index, QString& qstrDigFilType, double& dDigFilDecrate) const
{
    Job::Spt job = mJobList.at( index.get() );
    return job->getFrequency (qstrDigFilType, dDigFilDecrate);
}

Types::Time DataModel::startTimeOfJob ( const Types::Index& index ) const
{
    Job::Spt job = mJobList.at( index.get() );
    return job -> getStartTime();    
}

QDateTime DataModel::startDateTimeOfJob(const Types::Index& index) const
{
    // declaration of variables
    QDateTime clDateTime;

    Job::Spt job = mJobList.at (index.get());

    clDateTime.setDate(job->getStartDate());
    clDateTime.setTime(job->getStartTime());

    return (clDateTime);
}


Types::Time DataModel::getStartTimeOfJobLocalTime (const Types::Index& clIndex) const
{
    // declaration of variables
    Types::Time clJobStartTime;

    // get Starttime from job in UTC
    Job::Spt job = mJobList.at(clIndex.get());

    // add UTC to Localtime offset
    clJobStartTime = job->getStartTime();
    clJobStartTime = clJobStartTime.addSecs((qint64) (this->dUTCLocalTime * 3600));

    return (clJobStartTime);
}


QDateTime DataModel::getStartDateTimeOfJobLocalTime(const Types::Index& clIndex) const
{
    // declaration of variables
    QDateTime clDateTime;

    // get Startdate/time from job in UTC
    Job::Spt job = mJobList.at (clIndex.get());
    clDateTime.setDate(job->getStartDate());
    clDateTime.setTime(job->getStartTime());

    // add UTC to Localtime offset
    clDateTime = clDateTime.addSecs((qint64) (this->dUTCLocalTime * 3600));

    return (clDateTime);
}


bool DataModel::setStartTimeOfJobLocalTime(const Types::Index& clIndex, const Types::Time& clNewStartTime)
{
    // declaration of variables
    QDateTime clDateTime;
    int iDay = 0;

    // check, if we went into a new day
    clDateTime = QDateTime::fromTime_t(86400);
    clDateTime.setTime(clNewStartTime);

    iDay = clDateTime.date().day();
    clDateTime = clDateTime.addSecs((qint64) (this->dUTCLocalTime * -3600));

    if (this->dUTCLocalTime >= 0)
    {
        // we subtracted time from original time value - check, if we went into the day before:
        // e.g. new start time local = 03:15:00
        // UTC to LocalTime offset   = 05:00:00
        // new time will be          = 22:45:00 - 1 day
        if (iDay > clDateTime.date().day())
        {
            iDay = -1;
        }
    }
    else
    {
        // we added time to original time value - check, if we went into the next day:
        // e.g. new start time local = 23:15:00
        // UTC to LocalTime offset   = 05:00:00
        // new time will be          = 04:45:00 + 1 day
        if (iDay < clDateTime.date().day())
        {
            iDay = 1;
        }
    }

    Job::Spt job = mJobList.at(clIndex.get()); // this throws std::out_of_range
    clDateTime.setDate(job->getStartDate().addDays(iDay));
    clDateTime.setTime(clNewStartTime.addSecs((qint64) (this->dUTCLocalTime * -3600)));

    job->setStartDateTime(clDateTime);

    updateJobList();

    return (true);
}


Types::Time DataModel::stopTimeOfJob  ( const Types::Index& index ) const
{
    Job::Spt job = mJobList.at( index.get() );
    return job -> getStopTime();    
}

Types::TimeInterval DataModel::durationOfJob  ( const Types::Index& index ) const
{
    Job::Spt job = mJobList.at( index.get() );
    return job -> getDuration();    
}

Types::ObjectId DataModel::getObjectIdOfJob ( const Types::Index& index ) const
{
    Job::Spt job = mJobList.at( index.get() );
    return job -> getJobObjectId();        
}



void DataModel::setDurationOfJobTo( const Types::Index& index, const Types::TimeInterval& duration ) // throws std::out_of_range
{
    Job::Spt job = mJobList.at( index.get() ); // this throws std::out_of_range
    job -> setDuration( duration );
    updateJobList(); 
} 


void DataModel::setStartTimeOfJobTo(const Types::Index& index, const Types::Time& cNewStartTime)
{
    // declaration of variables
    QDateTime clDateTime;

    Job::Spt job = mJobList.at( index.get() ); // this throws std::out_of_range

    clDateTime.setDate(job->getStartDate());
    clDateTime.setTime(cNewStartTime);

    job->setStartDateTime(clDateTime);

    updateJobList();
}


void DataModel::addJobBeforeJobAtIndex( const Types::Index& selectedIndex, Job::Spt newJob )
{
    
    void endInsertJobAtIndex();
    JobVector::iterator it;
    Types::Index index;
    for ( it = mJobList.begin() , index = 0;
          ( not( it == mJobList.end() ) and ( index < selectedIndex ) );
          ++it )
        ++index ;

    emit startInsertJobAtIndex( index );
    mJobList.insert( it , newJob );
    newJob -> setDataModel( this );
    updateJobList();
    emit endInsertJob();

}

void DataModel::addJobAfterJobAtIndex  ( const Types::Index& selectedIndex, Job::Spt newJob )
{
    JobVector::iterator it;
    Types::Index index;

    for (it = mJobList.begin(), index = 0;
        (not(it == mJobList.end()) and (index < selectedIndex));
         ++it, ++index);

    // since in stl insertions take place before the iterator, we try to get one step further, after the jobindex...
    if (not (it == mJobList.end()))
    {
        ++it;
    }

    emit startInsertJobAtIndex( index ); // FIXME: is this index correct ? verify!

    mJobList.insert(it, newJob );

    newJob->setDataModel(this);

    updateJobList();

    emit endInsertJob();
    

}

void DataModel::deleteJobAtIndex ( const Types::Index& selectedIndex )
{
    JobVector::iterator it;
    Types::Index index;

    for ( it = mJobList.begin() , index = 0;
          ( not( it == mJobList.end() ) and ( index < selectedIndex ) );
          ++it, ++index );

    if ( it == mJobList.end() )
        // must have been a broken selectedIndex
        return;

    emit deleteInsertJobAtIndex( index );
    mJobList.erase( it );
    updateJobList();
    emit endDeleteJob();

}

bool isGreaterThanBorderFreq( Job::Spt job ) 
{
    QString qstrTmp;
    double  dTmp;
    return job->getFrequency (qstrTmp, dTmp) > DefaultValues::BorderFrequency;
}

bool isSmallerThanBorderFreq( Job::Spt job ) 
{
    QString qstrTmp;
    double  dTmp;
    return job->getFrequency (qstrTmp, dTmp) <= DefaultValues::BorderFrequency;
}

Job::Spt DataModel::cloneJobWithFreqNear (unsigned int frequency)
{
    // declaration with variables
    QString  qstrTmp;
    double   dTmp;
    size_t   sCount;
    Job::Spt clResult;
    double   dSampleMax       = 0.0;
    double   dSampleMin       = 1.0E9;
    size_t   sIndex           = std::numeric_limits<std::size_t>::max ();
    size_t   sIndexSampleMax  = std::numeric_limits<std::size_t>::max ();
    size_t   sIndexSampleMin  = std::numeric_limits<std::size_t>::max ();
    QMap<double, unsigned int> qmapTemplateFreqs;

    // collect all sample frequencies of template joblist into QMap
    qmapTemplateFreqs.clear();
    for (sCount = 0; sCount < mJobList.size(); sCount++)
    {
        qmapTemplateFreqs.insert(mJobList [sCount]->getFrequency (qstrTmp, dTmp), sCount);
    }
    QMapIterator<double, unsigned int>qIter (qmapTemplateFreqs);

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] template sample freqs:" << qmapTemplateFreqs;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] searching for freq...:" << frequency;

    // search for job with next smaller sampling frequency
    qIter.toFront();
    while (qIter.hasNext())
    {
        qIter.next();
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] checking:" << frequency << "/" << qIter.key();

        if (qIter.key() <= ((double) frequency))
        {
            dSampleMin      = qIter.key  ();
            sIndexSampleMin = qIter.value();
        }
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] template sample freqs:" << qmapTemplateFreqs;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] searching for freq...:" << frequency;

    // search for job with next bigger sampling frequency
    qIter.toBack();
    while (qIter.hasPrevious())
    {
        qIter.previous();
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] checking:" << frequency << "/" << qIter.key();

        if (qIter.key() >= ((double) frequency))
        {
            dSampleMax      = qIter.key  ();
            sIndexSampleMax = qIter.value();
        }
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] next smaller:" << dSampleMin;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] next smaller:" << sIndexSampleMin;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] next bigger.:" << dSampleMax;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] next bigger.:" << sIndexSampleMax;

    // check, if one of the found jobs fits 100% to sample frequency, if not,
    // choose next bigger one
    if (dSampleMin == frequency)
    {
        sIndex = sIndexSampleMin;
    }
    else
    {
        sIndex = sIndexSampleMax;
    }

    if (sIndex != std::numeric_limits<std::size_t>::max ())
    {
        clResult = Job::deepCopy(mJobList [sIndex]);
    }
    else
    {
        clResult = Job::deepCopy(mJobList [0]);
    }
        
    return clResult;
}


Job::Spt DataModel::getJobAt( const Types::Index& index ) const
{
    // gnack.. :-/
    return mJobList.at( index.get() );
}


void DataModel::updateJobList()
{
    // declaration of variables
    QDateTime clDateTime = jobListStartDateTime ();
    clDateTime = clDateTime.addSecs((qint64) (this->dUTCLocalTime * -3600));

    updateJobList (clDateTime);
}


void DataModel::updateJobList( const QDateTime & newStart )
{
    if (mJobList.empty() == false)
    {
        // update start time in ADUConf XML file
        mJobListStartDateTime = newStart;

        // update all jobs of the joblist
        UpdateJobData sAllJob = UpdateJobData (newStart, mAdjustToMinute, mAdjustMode);
        std::vector<Job::Spt>::iterator iterJob;

        for (iterJob  = mJobList.begin();
             iterJob != mJobList.end  ();
             iterJob++)
        {
            sAllJob (*iterJob);
        }

        // signal to all connected classes that job data has changed
        emit jobDataChanged();
    }
}


bool DataModel::swapJobPosition(const size_t sIndex1, const size_t sIndex2)
{
    // declaration of variables
    bool bRetValue = true;
    Job::Spt pJob1;

    // first check, if both entries are within the vector
    if ((sIndex1 >= this->mJobList.size()) ||
        (sIndex2 >= this->mJobList.size()))
    {
        bRetValue = false;
    }
    else
    {
        pJob1                    = this->mJobList [sIndex1];
        this->mJobList [sIndex1] = this->mJobList [sIndex2];
        this->mJobList [sIndex2] = pJob1;
        updateJobList();
    }

    return (bRetValue);
}


// slot
void DataModel::shiftStartChanged (const QDateTime & newStart)
{
    // declaration of variables
    QDateTime clNewDateTime(newStart);

    mJobListStartTimeChanged = true;

    // add UTC to Localtime Offset
    clNewDateTime = clNewDateTime.addSecs((qint64) (this->dUTCLocalTime * -3600));

    updateJobList (clNewDateTime);
}

// slot
void DataModel::setAdjustToMinute( bool adjustIt  )
{
    mAdjustToMinute = adjustIt;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] was called before shiftStartChanged (...)";
    shiftStartChanged (this->getJoblistStartTimeLocalTime());
}

// slot
void DataModel::setAdjustPause( ControlData::AdjustMode mode )
{
    mAdjustMode = mode;    
    shiftStartChanged (this->getJoblistStartTimeLocalTime());
}

// slot
void DataModel::enableBoardChannelForCurrentJob( unsigned channel , bool enable )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return;
    job -> setEnableBoardChannel( channel , enable );
    emit jobDataChanged();
}

void DataModel::saveFiles()
{
    qint64 writeRestult = 0;

    updateDocument();
    #ifdef HARDCODE_OUTPUT_DIR_JOBLIST
    QDir saveDirectory( "/tmp" );
    QString outFileName = saveDirectory.filePath( mJobListFileInfo.fileName() );
    qDebug() << __PRETTY_FUNCTION__  << " *** hardcoded file path ***";
    #else
    QString outFileName = mJobListFileInfo.absoluteFilePath();
    #endif

    // delete existing header and insert a new one
    QDomNode xmlNode = mJobListDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"ISO-8859-1\"");
    mJobListDocument.replaceChild(xmlNode, mJobListDocument.firstChild());

    // write ADUConf XML file
    QByteArray result   = mJobListDocument.toByteArray( 4 );

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] saving XML file:\n" << mJobListDocument.toString();
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] saving XML file:\n" << outFileName;

    QFile clOutFile;
    clOutFile.setFileName(outFileName);
    clOutFile.remove();
    clOutFile.open (QIODevice::WriteOnly | QIODevice::Truncate);
    writeRestult = clOutFile.write(result.data());
    clOutFile.close();

    if ( writeRestult == -1 ) {
        std::stringstream ss;
        ss << "unable to write file : " << outFileName.toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }

    std::for_each( mJobList.begin(), mJobList.end(), SaveJobFile() );

}

void DataModel::updateDocument()
{
    // declaration of variables
    QString qstrTmp;

    if (mJobListStartTimeChanged)
    {
        Tools::updateDomTimeElement( mStartTimeElement, mJobListStartDateTime.time() );

        if (mJobListStartDateTime.date() == QDate (1970, 1, 1))
        {
            Tools::updateDomTextElement (mStartDateElement, "0000-00-00");
        }
        else
        {
            Tools::updateDomDateElement( mStartDateElement, mJobListStartDateTime.date());
        }
    }

    // update storage mode and target directory
    Tools::DomElementWalker walker(mJobListDocument);
    walker.assertElementName    ("ADUConf");
    walker.goToFirstChild       ("TargetDirectory");
    Tools::updateDomTextElement (walker.get(), this->mTargetDirectory);

    walker.goToParent           ();
    walker.goToFirstChild       ("StorageMode");
    Tools::updateDomTextElement (walker.get(), this->mStorageMode);

    // update CleanJobTable flag
    walker.goToParent           ();
    walker.goToFirstChild       ("CleanJobTable");
    Tools::updateDomTextElement (walker.get(), this->mCleanJobTables);

    // update AdaptChannelConfig flag
    walker.goToParent           ();
    walker.goToFirstChild       ("AdaptChannelConfig");
    Tools::updateDomTextElement (walker.get(), this->mAdaptChannelConfig);

    // update AdaptChannelConfig flag
    walker.goToParent           ();
    walker.goToFirstChild       ("AdaptSensorType");
    Tools::updateDomTextElement (walker.get(), this->mAdaptSensorType);

    // update AdaptSensorSpacing flag
    walker.goToParent           ();
    walker.goToFirstChild       ("AdaptSensorSpacing");
    Tools::updateDomTextElement (walker.get(), this->mAdaptSensorSpacing);

    // update JoblistMode tag
    walker.goToParent           ();
    if (this->eJoblistMode == C_JOBLIST_MODE_MT)
    {
        qstrTmp = C_JOBLIST_MODE_MT_STRING;
    }
    else if (this->eJoblistMode == C_JOBLIST_MODE_CSMT)
    {
        qstrTmp = C_JOBLIST_MODE_CSMT_STRING;
    }
    else
    {
        qstrTmp = C_JOBLIST_MODE_INV_STRING;
    }

    try
    {
        if (walker.hasChild("JoblistMode") == false)
        {
            // node does not seem to exist - create it
            QDomElement qdnJoblistModeNode = mJobListDocument.createElement  ("JoblistMode");
            QDomText    contentNode        = mJobListDocument.createTextNode (qstrTmp);
            qdnJoblistModeNode.appendChild(contentNode);
            walker.get().appendChild(qdnJoblistModeNode);
        }
        else
        {
            walker.goToFirstChild       ("JoblistMode");
            Tools::updateDomTextElement (walker.get(), qstrTmp);
        }
    }
    catch (std::runtime_error* e)
    {
    }

    // update UTCLocalTimeOffset tag
    walker.goToParent ();
    try
    {
        qstrTmp = QString::number(this->dUTCLocalTime, 'g', 2);
        if (walker.hasChild("UTCLocalTimeOffset") == false)
        {
            // node does not seem to exist - create it
            QDomElement qdnJoblistModeNode = mJobListDocument.createElement  ("UTCLocalTimeOffset");
            QDomText    contentNode        = mJobListDocument.createTextNode (qstrTmp);
            qdnJoblistModeNode.appendChild(contentNode);
            walker.get().appendChild(qdnJoblistModeNode);
        }
        else
        {
            walker.goToFirstChild       ("UTCLocalTimeOffset");
            Tools::updateDomTextElement (walker.get(), qstrTmp);
        }
    }
    catch (std::runtime_error* e)
    {
    }

    // update rest of all joblist nodes (links to XML job files)
    updateJobListXmlTag();
}


void DataModel::updateJobListXmlTag()
{
    Tools::DomElementWalker walker( mJobListDocument );
    walker.assertElementName( "ADUConf" );
    walker.goToFirstChild( "JobList" );

    QDomElement jobListTag = walker.get();

    QDomElement jobtag = jobListTag.firstChildElement( "Job" );
    while ( not jobtag.isNull() ) {
        jobListTag.removeChild( jobtag );
        jobtag = jobListTag.firstChildElement( "Job" );
    }

    for ( JobVector::const_iterator it = mJobList.begin() ; it != mJobList.end() ; ++it ) {
        QDomElement newJobTag = mJobListDocument.createElement( "Job" );
        newJobTag.setAttribute( "id" , (*it) -> getId() );

        if ((*it)->getLocked () == true)
        {
            newJobTag.setAttribute("locked", "1");
        }
        else
        {
            newJobTag.setAttribute("locked", "0");
        }

        QDomElement newTargetDirectoryTag = mJobListDocument.createElement( "TargetDirectory" );
        (*it) -> updateJobFileName();
        QDomText contentNode = mJobListDocument.createTextNode( (*it) -> getJobFileName() );
        newTargetDirectoryTag.appendChild( contentNode );
        newJobTag.appendChild( newTargetDirectoryTag );
        jobListTag.appendChild( newJobTag );
    }

}


////////////////////////////////////////////////////////////////////////////////
//
// get sensors stuff

SensorInfo::Spt DataModel::getSensorInfoForChannel( const Types::Index& channelNumber ) 
{
    return getDetectedSensorHwForChannel( channelNumber );
}

SensorInfo::Spt DataModel::getDetectedSensorHwForChannel( const Types::Index& channelNumber )
{
    return mHardwareConfig.isNull() ? SensorInfo::Spt() : mHardwareConfig -> getSensorInfoForChannel( channelNumber );
}

bool    DataModel::getSensorHWEnabledInputAtChannel     ( const Types::Index& /* index */ ) const
{
    return true;

    // return 
    //     (
    //         ( not mConfiguredSensorCfg.isNull() ) 
    //         and 
    //         ( ( mConfiguredSensorCfg -> getSelectedInputAtChannel( index ).compare( DefaultValues::Zero ) == 0 ) 
    //           or 
    //           ( mConfiguredSensorCfg -> getSelectedInputAtChannel( index ).compare( DefaultValues::One ) == 0  ) 
    //             )
    //         );
}


QString DataModel::getSensorHWSelectedInputAtChannel    ( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getSelectedInputAtChannel( index );
}

QString DataModel::getSensorHWChannelTypeInputAtChannel ( const Types::Index& index) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getChannelTypeInputAtChannel( index );
}

QString DataModel::getSensorHWConfigInputAtChannel ( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getSensorConfigInputAtChannel( index );
}


QString DataModel::getSensorHWSensorTypeInputAtChannel  ( const Types::Index& index ) const
{

    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getSensorTypeInputAtChannel( index );
    
}

QString DataModel::getSensorHWSerialInputAtChannel ( const Types::Index& index ) const
{

    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getSensorSerialInputAtChannel( index );
    
}

////////////////////////////////////////////////////////////////////////////////
//
// modify sensor config

void sensorConfigGuard( SensorChannelConfig::Spt sensorChannelConfig ) {
 
    if ( not sensorChannelConfig.isNull() ) 
        return;

    std::stringstream ss; 
    ss << "No sensor config exists till now, must not happen!";
    throw std::runtime_error( ss.str() );
    
}

void DataModel::setSensorHWSelectedInputAtChannel    ( const Types::Index& index, const QString& value )
{

    // sensorConfigGuard( mConfiguredSensorCfg );
    if (mConfiguredSensorCfg.isNull())
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to set sensor input!";
        return;
    }

    mConfiguredSensorCfg->setHWSelectedInputAtChannel(index, value);
}

void DataModel::setSensorHWChannelTypeInputAtChannel ( const Types::Index& index, const QString& value )
{
    // sensorConfigGuard( mConfiguredSensorCfg );
    if (mConfiguredSensorCfg.isNull())
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to set channel type!";
        return;
    }
    mConfiguredSensorCfg->setHWChannelTypeInputAtChannel (index, value);
}

void DataModel::setSensorHWSensorTypeInputAtChannel  ( const Types::Index& index, const QString& value )
{
    // sensorConfigGuard( mConfiguredSensorCfg );
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setHWSensorTypeInputAtChannel( index , value );

}

void DataModel::setSensorHWConfigInputAtChannel      ( const Types::Index& index , const QString& value ) 
{
    // sensorConfigGuard( mConfiguredSensorCfg );
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setHWSensorConfigInputAtChannel( index , value );

}

void DataModel::setSensorHWSerialInputAtChannel ( const Types::Index& index , const QString& value )
{
    // sensorConfigGuard( mConfiguredSensorCfg );
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setHWSensorSerialInputAtChannel( index , value );

}

QString DataModel::getSensorHWPosX1( const Types::Index& index ) const 
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosX1AtChannel( index );
}
QString DataModel::getSensorHWPosY1( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosY1AtChannel( index );
}
QString DataModel::getSensorHWPosZ1( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosZ1AtChannel( index );
}
QString DataModel::getSensorHWPosX2( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosX2AtChannel( index );
}
QString DataModel::getSensorHWPosY2( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosY2AtChannel( index );
}
QString DataModel::getSensorHWPosZ2( const Types::Index& index ) const
{
    return mConfiguredSensorCfg.isNull() ? DefaultValues::Undefined : mConfiguredSensorCfg -> getPosZ2AtChannel( index );
}

void DataModel::setSensorHWPosX1( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosX1AtChannel( index , value );
}

void DataModel::setSensorHWPosY1( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosY1AtChannel( index , value );

}
void DataModel::setSensorHWPosZ1( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosZ1AtChannel( index , value );

}
void DataModel::setSensorHWPosX2( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosX2AtChannel( index , value );

}
void DataModel::setSensorHWPosY2( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosY2AtChannel( index , value );

}
void DataModel::setSensorHWPosZ2( const Types::Index& index, const QString& value )
{
    if ( mConfiguredSensorCfg.isNull() ) 
        return;
    mConfiguredSensorCfg -> setPosZ2AtChannel( index , value );

}



////////////////////////////////////////////////////////////////////////////////
//
// get board stuff


QStringList DataModel::allowedSampleFreqs (void)
{
    // declaration of variables
    QVector<double> qvecSampleFreqs;
    QStringList     qstrlTmp;
    size_t          sBoardCount;
    size_t          sEntryCount;

    if (mHardwareConfig.isNull() == false)
    {
        for (sBoardCount = 0; sBoardCount < 10; sBoardCount++)
        {
            AdbBoardInfo::Spt pclBoardInfo = mHardwareConfig->getAdbBoardInfoForChannel (sBoardCount);
            if (pclBoardInfo.isNull() == false)
            {
                qstrlTmp = pclBoardInfo->getSampleFreqs ();
                for (sEntryCount = 0; sEntryCount < (size_t) qstrlTmp.size(); sEntryCount++)
                {
                    if (qvecSampleFreqs.indexOf(qstrlTmp.at(sEntryCount).toDouble()) == -1)
                    {
                        qvecSampleFreqs.append(qstrlTmp.at(sEntryCount).toDouble());
                    }
                }
            }
        }
    }

    // add "digital filter" frequencies - only for ADU-07e LF/HF joblists (not for MF, TXM or ADU-08e)
    if ((this->isMF()        == false) &&
        (this->isTXM()       == false) &&
        (this->isADU09u10e() == false) &&
        (this->isADU08eBB()  == false))
    {
        qvecSampleFreqs.append  (4.0);
        qvecSampleFreqs.append (16.0);
        qvecSampleFreqs.append (32.0);
        qvecSampleFreqs.append (64.0);
    }

    // sort vector
    qSort (qvecSampleFreqs);

    // create new sorted string list from vector
    qstrlTmp.clear();
    for (sEntryCount = 0; sEntryCount < (size_t) qvecSampleFreqs.size(); sEntryCount++)
    {
        qstrlTmp.append(QString::number (qvecSampleFreqs.at (sEntryCount)));
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] found sample freqs:" <<  qvecSampleFreqs;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] found sample freqs:" <<  qstrlTmp;


    return (qstrlTmp);
}


QStringList DataModel::allowedSampleFreqs (const Types::Index& iIndex)
{
    // declaration of variables
    QVector<double> qvecSampleFreqs;
    QVector<unsigned int> qvecChannels;
    QStringList     qstrlTmp;
    ssize_t         sBoardCount;
    ssize_t         sEntryCount;

    // get channel ids of the channels that are used inside this job
    for (sBoardCount = 0; sBoardCount < 10; sBoardCount++)
    {
        Types::Index iBoardIndex (sBoardCount, true);
        try
        {
            if (this->getJobAt(iIndex)->getIsChannelEnabled(iBoardIndex) == true)
            {
                qvecChannels.append(sBoardCount);
            }
        }
        catch (...)
        {
        }
    }

    if (mHardwareConfig.isNull() == false)
    {
        for (sBoardCount = 0; sBoardCount < qvecChannels.size(); sBoardCount++)
        {
            AdbBoardInfo::Spt pclBoardInfo = mHardwareConfig->getAdbBoardInfoForChannel (qvecChannels [sBoardCount]);
            if (pclBoardInfo.isNull() == false)
            {
                qstrlTmp = pclBoardInfo->getSampleFreqs ();
                for (sEntryCount = 0; sEntryCount < (ssize_t) qstrlTmp.size(); sEntryCount++)
                {
                    if (qvecSampleFreqs.indexOf(qstrlTmp.at(sEntryCount).toDouble()) == -1)
                    {
                        qvecSampleFreqs.append(qstrlTmp.at(sEntryCount).toDouble());
                    }
                }
            }
        }
    }

    // sort vector
    //qSort (qvecSampleFreqs);
    std::sort (qvecSampleFreqs.begin(), qvecSampleFreqs.end());

    // create new sorted string list from vector
    qstrlTmp.clear();
    for (sEntryCount = 0; sEntryCount < (ssize_t) qvecSampleFreqs.size(); sEntryCount++)
    {
        qstrlTmp.append(QString::number (qvecSampleFreqs.at (sEntryCount)));
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] found sample freqs:" <<  qvecSampleFreqs;
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] found sample freqs:" <<  qstrlTmp;


    return (qstrlTmp);
}


QStringList DataModel::allowedDiscreteValuesForGain1AtChannel( const Types::Index& channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QStringList();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );
    
    return boardInfo.isNull() ? QStringList() : boardInfo -> getGainStage1();

}

QStringList DataModel::allowedDiscreteValuesForGain2AtChannel( const Types::Index& channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QStringList();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );
    
    return boardInfo.isNull() ? QStringList() : boardInfo -> getGainStage2();
}

QStringList DataModel::allowedLpHpFilterValuesAtChannel       ( const Types::Index&  channelNumber ) 
{
    if ( mHardwareConfig.isNull() )
        return QStringList();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QStringList() : boardInfo -> getLpHpFilter();
    
}

QStringList DataModel::allowedRfFilterValuesAtChannel(Types::Index const& channelNumber )
{
    // declaration of variables
    QStringList qstrlTmp;

    if ( mHardwareConfig.isNull() )
        return QStringList();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    qstrlTmp = boardInfo.isNull() ? QStringList() : boardInfo -> getRfFilter();

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] allowed RF filter:" << qstrlTmp;

    return (qstrlTmp);
    
}

QStringList DataModel::allowedChopperValuesAtChannel          ( const Types::Index&  channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QStringList();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QStringList() : boardInfo -> getChopper();
    
}

bool DataModel::getHasDac ( const Types::Index& channelNumber ) const 
{

    if ( mHardwareConfig.isNull() )
        return false;

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? false : boardInfo -> getHasDac();
    
}



QString DataModel::minExtGainAtChannel ( const Types::Index& channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QString();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QString() : boardInfo -> getExtGainMin();    
}

QString DataModel::maxExtGainAtChannel ( const Types::Index& channelNumber)
{
    if ( mHardwareConfig.isNull() )
        return QString();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QString() : boardInfo -> getExtGainMax();        
}

QString     DataModel::getMinDacAtChannel ( const Types::Index& channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QString();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QString() : QString::number( boardInfo -> getDacRangeLow() );            
}

QString     DataModel::getMaxDacAtChannel ( const Types::Index& channelNumber )
{
    if ( mHardwareConfig.isNull() )
        return QString();

    AdbBoardInfo::Spt boardInfo = mHardwareConfig -> getAdbBoardInfoForChannel( channelNumber );

    return boardInfo.isNull() ? QString() : QString::number( boardInfo -> getDacRangeHigh() );            

}


////////////////////////////////////////////////////////////////////////////////
//
// set board stuff

bool DataModel::setExtGainForChannel    ( const Types::Index& channel , QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setExtGainForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}
    
bool DataModel::setChopperForChannel    ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setChopperForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}
    
bool DataModel::setRfFilterForChannel    ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setRfFilterForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}

bool DataModel::setGain1ForChannel      ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setGain1ForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}
  
bool DataModel::setDacForChannel        ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setDacForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}
    
bool DataModel::setLpHpFilterForChannel ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setLpHpFilterForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}
    
bool DataModel::setGain2ForChannel      ( const Types::Index& channel, QString value )
{
    Job::Spt job = getSelectedJob();
    if ( job.isNull() )
        return false;

    job -> setGain2ForChannel( channel.get() , value );
    emit jobDataChanged();
    return true;
}

void DataModel::setMeasurementMode (ControlData::MeasurementMode measurementMode, const bool bUpdateAllJobs)
{
    // declaration of variables
    unsigned int uiCounter;

    if (bUpdateAllJobs == false)
    {
        if (getSelectedJob().isNull())
        {
            return;
        }

        // QObject::dumpObjectInfo();
        getSelectedJob() -> setMeasurementMode( measurementMode );
    }
    else
    {
        for (uiCounter = 0; uiCounter < this->jobCount(); uiCounter++)
        {
            this->getJobAt(uiCounter)->setMeasurementMode(measurementMode);
        }
    }

    updateJobList();
}

QString DataModel::getTXMNumStacks()
{
    return getSelectedJob().isNull() ? DefaultValues::Undefined : getSelectedJob() -> getTXMNumStacks();
}

QString DataModel::getTXMDipTXTime()
{
    return getSelectedJob().isNull() ? DefaultValues::Undefined : getSelectedJob() -> getTXMDipTXTime();
}

QString DataModel::getTXMNumDipoles()
{
    return getSelectedJob().isNull() ? DefaultValues::Undefined : getSelectedJob() -> getTXMNumDipoles();
}

bool DataModel::setTXMNumStacks      ( const QString& value )
{
    if (getSelectedJob().isNull())
    {
        return (false);
    }
    getSelectedJob() -> setTXMNumStacks( value );

    // MWI: Note - updateJobList (...) must be called twice here, as in CSAMT mode
    //      the duration of the job is updated. as a result if only called once,
    //      the start-times of the following jobs will not be updated.
    updateJobList(); 
    updateJobList();

    return (true);
}

bool DataModel::setTXMDipTXTime      ( const QString& value )
{
    if (getSelectedJob().isNull())
    {
        return (false);
    }
    getSelectedJob() -> setTXMDipTXTime( value );

    // MWI: Note - updateJobList (...) must be called twice here, as in CSAMT mode
    //      the duration of the job is updated. as a result if only called once,
    //      the start-times of the following jobs will not be updated.
    updateJobList();
    updateJobList();

    return (true);
}

bool DataModel::setTXMNumDipoles ( const QString& value )
{
    if (getSelectedJob().isNull())
    {
        return (false);
    }
    getSelectedJob() -> setTXMNumDipoles( value );

    // MWI: Note - updateJobList (...) must be called twice here, as in CSAMT mode
    //      the duration of the job is updated. as a result if only called once,
    //      the start-times of the following jobs will not be updated.
    updateJobList();
    updateJobList();

    return (true);
}


////////////////////////////////////////////////////////////////////////////////
//
// get stuff from current job.

QString DataModel::getExtGainForChannel    ( const Types::Index& index )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getExtGainForChannel( index );
}

QString DataModel::getChopperForChannel    ( const Types::Index& index  )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getChopperForChannel( index );
}

QString DataModel::getRfFilterForChannel   ( const Types::Index& index )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getRfFilterForChannel( index );
}

QString DataModel::getGain1ForChannel      ( const Types::Index& index )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getGain1ForChannel( index );
}

QString DataModel::getDacForChannel        ( const Types::Index& index )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getDacForChannel( index );
}

QString DataModel::getLpHpFilterForChannel ( const Types::Index& index )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getLpHpFilterForChannel( index );
}

QString DataModel::getGain2ForChannel      ( const Types::Index& index  )
{
    return getSelectedJobAndThrowIfThereIsNoSelectedJob() -> getGain2ForChannel( index );
}

ControlData::MeasurementMode DataModel::getMeasurementMode()
{
    return getSelectedJob().isNull() ? ControlData::UndefindedMeasurementMode : getSelectedJob() -> getMeasurementMode (); 
}


QString DataModel::getCommentsLineNumber()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getLineNumber (); 
}

QString DataModel::getCommentsRunNumber()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getRunNumber (); 
}

QString DataModel::getCommentsClient()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getClient (); 
}

QString DataModel::getCommentsOperator()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getOperator(); 
}

QString DataModel::getCommentsContractor()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getContractor (); 
}

QString DataModel::getCommentsArea()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getArea (); 
}

QString DataModel::getCommentsSurveyId()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getSurveyId (); 
}

QString DataModel::getCommentsSiteId()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getSiteId (); 
}

QString DataModel::getCommentsWeather()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getWeather (); 
}

QString DataModel::getCommentsComment()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getComment (); 
}

QString DataModel::getCommentsSiteName()
{
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getSiteName ();
}

QString DataModel::getCommentsSiteNameRR()
{
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getSiteNameRR ();
}

QString DataModel::getCommentsSiteNameEMAP()
{
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getSiteNameEMAP ();
}

QString DataModel::getCommentsBaseFrequency()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getBaseFrequency (); 
}

// QString DataModel::getCommentsDipoleTxTime()
// { 
//     return getSelectedJob().isNull() ? "" : getSelectedJob() -> getDipoleTxTime (); 
// }

// QString DataModel::getCommentsNumberOfDipoles()
// { 
//     return getSelectedJob().isNull() ? "" : getSelectedJob() -> getNumberOfDipoles (); 
// }

QString DataModel::getCommentsWaveFormName()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getWaveFormName (); 
}

QString DataModel::getCommentsProcessings()
{ 
    return getSelectedJob().isNull() ? "" : getSelectedJob() -> getProcessings (); 
}


void DataModel::setCommentsProcessings( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setProcessings( value );
}

void DataModel::setCommentsWeather( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setWeather( value );
}

void DataModel::setCommentsWaveFormName( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setWaveFormName( value );
}

void DataModel::setCommentsSurveyId( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setSurveyId( value );
}

void DataModel::setCommentsSiteId( const QString& value )
{ 
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setSiteId( value );
}


// void DataModel::setCommentsDipoleTxTime( const QString& value )
// {
//     if ( getSelectedJob().isNull() )
//         return;
//     getSelectedJob() -> setDipoleTxTime( value );
// }

// void DataModel::setCommentsNumberOfDipoles( const QString& value )
// {
//     if ( getSelectedJob().isNull() )
//         return;
//     getSelectedJob() -> setNumberOfDipoles( value );
// }

void DataModel::setCommentsArea( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setArea( value );
}

void DataModel::setCommentsComment( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setComment( value );
}


void DataModel::setCommentsSiteName( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setSiteName( value );
}


void DataModel::setCommentsSiteNameRR( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setSiteNameRR( value );
}


void DataModel::setCommentsSiteNameEMAP( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setSiteNameEMAP( value );
}


void DataModel::setCommentsContractor( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setContractor( value );
}

void DataModel::setCommentsLineNumber( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setLineNumber( value );
}

void DataModel::setCommentsBaseFrequency( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setBaseFrequency( value );
}

void DataModel::setCommentsClient( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setClient( value );
}

void DataModel::setCommentsOperator( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setOperator( value );
}

void DataModel::setCommentsRunNumber( const QString& value )
{
    if ( getSelectedJob().isNull() )
        return;
    getSelectedJob() -> setRunNumber( value );
}

////////////////////////////////////////////////////////////////////////////////
//
// job selecton stuff

Job::Spt DataModel::getSelectedJob()
{
    return mJobSelecteionUsesGuiSelectionModel ? 
        getGuiSelectedJob() :
        getManuallySelectedJob() ;
}

Job::Spt DataModel::getGuiSelectedJob()
{
    Job::Spt result;
    QItemSelectionModel* selectionModel = getSelectionModel();
    if ( selectionModel == 0 ) {
        throw std::runtime_error( std::string( __PRETTY_FUNCTION__ ).append( " FATAL selectionModel is null, must not happen!" ) );
    }

    QModelIndexList selectionList =  selectionModel -> selectedRows();
    if ( selectionList.size() > 1 )
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    if ( selectionList.size() == 0 )
        return result; // nothing selected

    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__ << " selectionList[ 0 ].row(): " << selectionList[ 0 ].row();
    #endif

    result = mJobList.at( selectionList[ 0 ].row() );

    return result;
}

Job::Spt DataModel::getManuallySelectedJob()
{
    return mManuallySelectedJobIndex.isValid() ?
        mJobList.at( mManuallySelectedJobIndex.get() ) :
        Job::Spt() ;
}

Job::Spt DataModel::getSelectedJobAndThrowIfThereIsNoSelectedJob()
{
    Job::Spt result = getSelectedJob();
    if ( result.isNull() ) {
        std::stringstream ss; 
        ss <<  __PRETTY_FUNCTION__ << " no job selected, so i throw as promised!";
        throw std::runtime_error( ss.str() );
    }
    return result;
}

void DataModel::setJobSelectionUsesGuiSelectionModel( bool jobSelecteionUsesGuiSelectionModel ) 
{ 
    mJobSelecteionUsesGuiSelectionModel =  jobSelecteionUsesGuiSelectionModel ; 
}

void DataModel::setSelectedJobIndex( const Types::Index& jobNumber ) 
{ 
    mManuallySelectedJobIndex = jobNumber; 
}

////////////////////////////////////////////////////////////////////////////////
//
// debug stuff below


std::string DataModel::sensorConfigToStdString() const
{
    return mConfiguredSensorCfg.isNull() ?
        std::string( " No configured hardware data available!" ) :
        mConfiguredSensorCfg -> toStdString();
}


const QString DataModel::getTimeFormat (void)
{
    return (this->mTimeFormatElement.firstChild().nodeValue());
}


bool DataModel::setTimeFormat (const QString& qstrTimeFormat)
{
    // declaration of variables
    bool bRetValue = true;

    Tools::updateDomTextElement(this->mTimeFormatElement, qstrTimeFormat);

    return (bRetValue);
}


QString DataModel::getTargetDirectory (void)
{
    return (this->mTargetDirectory);
}


QString DataModel::getStorageMode (void)
{
    return (this->mStorageMode);
}


bool DataModel::setTargetDirectory(const QString& qstrTargetDir)
{
    this->mTargetDirectory = qstrTargetDir;
    return (true);
}


bool DataModel::setStorageMode(const QString& qstrStorageMode)
{
    this->mStorageMode = qstrStorageMode;
    return (true);
}


bool DataModel::setCleanJobTables(const bool bFlag)
{
    if (bFlag == true)
    {
        mCleanJobTables = "TRUE";
    }
    else
    {
        mCleanJobTables = "FALSE";
    }

    return (true);
}


bool DataModel::getCleanJobTables(void)
{
    // declaration of variables
    bool bRetValue;

    if (mCleanJobTables.compare("TRUE") == 0)
    {
        bRetValue = true;
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}


bool DataModel::setAdaptChannelConfig(const bool bFlag)
{
    if (bFlag == true)
    {
        mAdaptChannelConfig = "TRUE";
    }
    else
    {
        mAdaptChannelConfig = "FALSE";
    }

    return (true);
}


bool DataModel::getAdaptChannelConfig(void)
{
    // declaration of variables
    bool bRetValue;

    if (mAdaptChannelConfig.compare("TRUE") == 0)
    {
        bRetValue = true;
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}


bool DataModel::setAdaptSensorType(const QString& qstrMode)
{
    this->mAdaptSensorType = qstrMode;
    return (true);
}


QString DataModel::getAdaptSensorType(void)
{
    return (this->mAdaptSensorType);
}


bool DataModel::setAdaptSensorSpacing(const bool bFlag)
{
    if (bFlag == true)
    {
        mAdaptSensorSpacing = "TRUE";
    }
    else
    {
        mAdaptSensorSpacing = "FALSE";
    }

    return (true);
}


bool DataModel::getAdaptSensorSpacing(void)
{
    // declaration of variables
    bool bRetValue;

    if (mAdaptSensorSpacing.compare("TRUE") == 0)
    {
        bRetValue = true;
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}


bool DataModel::getTXMElectrodePosStr(const unsigned int uiElectrodeName, QString& qstrLat, QString& qstrLon, QString& qstrElevation)
{
    return (this->getJobAt(0)->getTXMElectrodePosStr (uiElectrodeName, qstrLat, qstrLon, qstrElevation));
}


bool DataModel::getTXMElectrodePos(const unsigned int uiElectrodeName, long& lLat, long& lLon, long& lElevation)
{
    return (this->getJobAt(0)->getTXMElectrodePos (uiElectrodeName, lLat, lLon, lElevation));
}


bool DataModel::setTXMElectrodePos(const unsigned int uiElectrodeName, long& lLat, long& lLon, long& lElevation)
{
    // declaration of variables
    bool         bRetValue = true;
    unsigned int uiCounter;

    // always update the values in ALL jobs as the electrode positions will not change
    // from job to job
    for (uiCounter = 0; uiCounter < this->jobCount(); uiCounter++)
    {
        if (this->getJobAt(uiCounter)->setTXMElectrodePos (uiElectrodeName, lLat, lLon, lElevation) == false)
        {
            bRetValue = false;
        }
    }

    return (bRetValue);
}


bool DataModel::setJoblistMode(const eJoblistModes_t eNewJoblistMode)
{
    this->eJoblistMode = eNewJoblistMode;
    emit jobDataChanged();

    return (true);
}


eJoblistModes_t DataModel::getJoblistMode(void)
{
    return (this->eJoblistMode);
}


bool DataModel::isTXM(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (this->mHardwareConfig.isNull() == false)
    {
        bRetValue = this->mHardwareConfig->isTXM();
    }

    return (bRetValue);
}


bool DataModel::isMF(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (this->mHardwareConfig.isNull() == false)
    {
        bRetValue = this->mHardwareConfig->isMF();
    }

    return (bRetValue);
}


bool DataModel::isADU08eBB(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (this->mHardwareConfig.isNull() == false)
    {
        bRetValue = this->mHardwareConfig->isADU08eBB();
    }

    return (bRetValue);
}


bool DataModel::isADU09u10e(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (this->mHardwareConfig.isNull() == false)
    {
        bRetValue = this->mHardwareConfig->isADU09u10e();
    }

    return (bRetValue);
}


double DataModel::getUTCLocalTimeOffset(void)
{
    return (this->dUTCLocalTime);
}


bool DataModel::setUTCLocalTimeOffset(const double dNewUTCLocalTimeOffset)
{
    // declaration of variables
    QDateTime clActualStartDateTime(this->jobListStartDateTime ());

    this->dUTCLocalTime = dNewUTCLocalTimeOffset;

    // update start-times in joblist according to new UTC to LocalTime offset
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] was called before shiftStartChanged (...)";
    this->shiftStartChanged(clActualStartDateTime);

    emit jobDataChanged();

    return (true);
}


QDateTime DataModel::jobListStartDateTime() const
{
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Joblist starttime:" << mJobListStartDateTime.toString();

    // declaration of variables
    QDateTime clStartDateTimeUTC;
    clStartDateTimeUTC.setTimeSpec(Qt::UTC);

    clStartDateTimeUTC = mJobListStartDateTime.addSecs( (qint64) (this->dUTCLocalTime * 3600));

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Joblist starttime:" << mJobListStartDateTime.toString();

    return (clStartDateTimeUTC);
}


const QDateTime DataModel::getJoblistStartTimeLocalTime (void)
{
    return (this->jobListStartDateTime ());
}


const QStringList DataModel::getAllowedSampleFreqsFromTemplate (void)
{
    // declaration of variables
    QStringList     qstrlRetValue;
    QVector<double> qlTmp;
    QString         qstrTmp;
    double          dTmp;
    QVariant        qvarTmp;
    unsigned int    uiCounter;

    // as first steps, read hardware supported sampling frequencies
    qstrlRetValue = this->allowedSampleFreqs();

    qlTmp.clear();
    for (uiCounter = 0; uiCounter < (unsigned int) qstrlRetValue.size(); uiCounter++)
    {
        qlTmp.append(qstrlRetValue[uiCounter].toDouble());
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] native sample freqs:" << qlTmp;

    // now check for all sampling frequencies of all jobs of this joblist
    // and add them to the list.
    for (uiCounter = 0; uiCounter < this->jobCount(); uiCounter++)
    {
        qvarTmp.setValue (this->getJobAt(uiCounter)->getFrequency(qstrTmp, dTmp));
        dTmp = qvarTmp.toDouble();
        if (qlTmp.contains(dTmp) == false)
        {
            qlTmp.append(dTmp);
            qDebug () << "[" << __PRETTY_FUNCTION__ << "] adding non native..:" << dTmp;
        }
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] all valid freqs....:" << qlTmp;

    qstrlRetValue.clear();
    qSort (qlTmp.begin(), qlTmp.end(), DefaultValues::aBiggerB);
    for (uiCounter = 0; uiCounter < (unsigned int) qlTmp.size(); uiCounter++)
    {
        qstrlRetValue.append(QString::number (qlTmp[uiCounter], 'g', 9));
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] all valid freqs....:" << qstrlRetValue;

    return (qstrlRetValue);
}


