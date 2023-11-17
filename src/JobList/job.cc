#include "job.h"
#include <sstream>
#include <QDir>
#include <algorithm>
#include "domelementwalker.h"
#include "xml_parse_helper.h"
#include "default_values.h"
#include "datamodel.h"
#include "xml_dom_helper.h"
#include "geocoordinates.h"
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
//
// Job representation


Job::Job() :
    mDataModel( 0 ),
    mMeasurementMode( ControlData::NormalMeasurementMode )
{
    mTXMPolarisations = mTXMPolarisations.fill(0.0, DefaultValues::TXMMaxNumDipoles);

    this->bLocked = false;
}

Job::Spt Job::create( QDomElement jobElement ) 
{
    Job::Spt result = Job::Spt( new Job() );
    QString idAsString = jobElement.attribute( "id" );
    bool conversionOk = false;
    result -> mId = idAsString.toUInt( &conversionOk ); 

   if ( not conversionOk ) {
        std::stringstream ss;
        ss << "unable to parse jobtag , invaild id attribute : [" << idAsString.toStdString() << "]" << std::endl;
        throw std::runtime_error( ss.str() );
    }

   QDomElement targetDirectory = jobElement.firstChildElement("TargetDirectory");
   if ( targetDirectory.isNull() ) {
       std::stringstream ss;
       ss << "unable to parse jobtag , invaild [TargetDirectory] element (Job id is " << result -> mId << " ) " << std::endl;
       throw std::runtime_error( ss.str() );
   }

   result -> mJobFileName = jobElement.firstChildElement("TargetDirectory").text(); // contains the job file name.

   // read "Locked" attribute
   QString qstrLocked = jobElement.attribute("locked", "0");
   if (qstrLocked.toUInt() == 0)
   {
       result->bLocked = false;
   }
   else
   {
       result->bLocked = true;
   }

   return result;
}


Job::Spt Job::createFromTemplate( )
{
    // FIXME: hardcode the template here?! dunno
    Job::Spt result = Job::Spt( new Job() );
    result -> prepareJobChannelConfigVector();
    return result;
} 

Job::Spt Job::deepCopy( Job::Spt jobToCopy )
{
    // careful: using a compiler generated copy ctor!
    // update document
    jobToCopy -> updateDocument();
    Job::Spt result = Job::Spt( new Job( (*jobToCopy) ) ); // compiler generated ctor?

    result -> cloneDocument();
    result -> parseXML();

    return result;
}



void Job::setJobDirectory( const QString& directory )
{
    mJobRootDirectory = directory;
}

void Job::loadAndParseFile() 
{
    QDir      jobRoot( mJobRootDirectory );
    QFileInfo fileInfo( jobRoot.canonicalPath() + mJobFileName );
    // QFile     file( fileInfo.canonicalFilePath() );
    QFile     file( fileInfo.filePath() );

    if ( not file.open(QIODevice::ReadOnly) ) {        
        std::stringstream ss;
        ss << "unable to open file : [" << file.fileName().toStdString() << "] error message is : " << file.errorString().toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }

    if ( not mJobDocument.setContent(&file) ) {
        file.close();
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ << " unable to parse file : " << file.fileName().toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }

    parseXML();

}

void Job::parseXML()
{
    parseGeneric();    
    prepareJobChannelConfigVector();
    parseChannelData();
    parseComments();
    // we dont read calibration data till now!
}



struct JobChannelConfigFromDefault
{
    JobChannelConfigFromDefault( const DataModel* dataModel ):
        mCounter( 0 ),
        mData( dataModel )
        {}

    unsigned int mCounter;
    const DataModel* mData;

    JobChannelConfig::Spt operator()() {
        JobChannelConfig::Spt result = JobChannelConfig::createFromDefaults( mData );
        result -> mId = mCounter++; // set channel number.
        return result;
    }
};

void Job::prepareJobChannelConfigVector()
{
    mJobChannelConfigVector.clear(); // a bit redundant..
    mJobChannelConfigVector = JobChannelConfigVector( DefaultValues::MaxChannels );
    JobChannelConfigFromDefault fob( mDataModel );
    std::generate( mJobChannelConfigVector.begin() , mJobChannelConfigVector.end() , fob );
    
}

void Job::parseGeneric()
{
    // some day, i'll split up this method...
    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "start_time" );
        
        mStartTimeElement = walker.get();
        #ifdef DEBUG_OUTPUT
        std::cout << __PRETTY_FUNCTION__ << "mStartTimeElement.text() " << mStartTimeElement.text().toStdString() << std::endl;
        #endif

        walker.goToParent();
        Tools::parseOptionalChildInto(walker.get(), "CyclicEvent", this->mCyclicEvent);
        Tools::parseOptionalChildInto(walker.get(), "Cycle",       this->mCyclicDuration);
        Tools::parseOptionalChildInto(walker.get(), "Granularity", this->mCyclicUnit);
    }
    
    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "start_date" );
        
        mStartDateElement = walker.get();
        #ifdef DEBUG_OUTPUT
        std::cout << __PRETTY_FUNCTION__ << "mStartTimeElement.text() " << mStartTimeElement.text().toStdString() << std::endl;
        #endif
    }

    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "stop_time" );
        
        mStopTimeElement = walker.get();

        #ifdef DEBUG_OUTPUT
        std::cout << __PRETTY_FUNCTION__ << "mStopTimeElement.text() " << mStopTimeElement.text().toStdString() << std::endl;
        #endif
    }

    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "stop_date" );
        
        mStopDateElement = walker.get();

        #ifdef DEBUG_OUTPUT
        std::cout << __PRETTY_FUNCTION__ << "mStopTimeElement.text() " << mStopTimeElement.text().toStdString() << std::endl;
        #endif
    }

    {
        QString valueTextStart = mStartTimeElement.text();
        QString valueTextStop  = mStopTimeElement.text();

        QString valueTextStartDate = mStartDateElement.text();
        QString valueTextStopDate  = mStopDateElement.text();

        mStartDate = QDate::fromString( valueTextStartDate , DefaultValues::DateFormat );
        mStopDate = QDate::fromString( valueTextStopDate , DefaultValues::DateFormat );

        mStartTime = QTime::fromString( valueTextStart, DefaultValues::TimeFormat );
        mStopTime  = QTime::fromString( valueTextStop,  DefaultValues::TimeFormat );

        int delta = mStartDate.daysTo( mStopDate ) * 24 * 60 * 60; // seconds of a day, zero if datetime not there or not parseable...

        mDurationSec.setSeconds( mStartTime.secsTo( mStopTime ) ); 

        mDurationSec.addSeconds( delta );

    }

    {
        Tools::DomElementWalker global_config_walker( mJobDocument );
        global_config_walker.assertElementName( "measurement" );

        global_config_walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "input" )
            .goToFirstChild( "Hardware" )
            .goToFirstChild( "global_config" );

        Tools::DomElementWalker sample_freq_walker( global_config_walker );

        sample_freq_walker.goToFirstChild( "sample_freq" );
        mSampleFreqElement = sample_freq_walker.get();
        mSampleFreq = (unsigned int ) mSampleFreqElement.text().toDouble();
    }

    {
        QSharedPointer<Tools::DomElementWalker> pclWalker;

        qDebug () << "[" << __PRETTY_FUNCTION__ << "] searching for meas_type - 1";

        this->goToATSWriterNode(this->mJobDocument, pclWalker, this->mDigFilType, this->mDigFilDecrate);
        pclWalker->goToFirstChild ("configuration");
        pclWalker->goToFirstChild ("meas_type");

        qDebug () << "[" << __PRETTY_FUNCTION__ << "] searching for meas_type - 2";

        QString measType = pclWalker->get().text().trimmed();

        mMeasurementMode = measType.compare( DefaultValues::CSAMT ) == 0 ?
            ControlData::CSAMTMeasurementMode :
            ControlData::NormalMeasurementMode ;

        pclWalker.clear();
    }

    // read the TXM22 Settings from the Jobfile
    {
        {
            // read waveform name
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Waveform");
            walker.goToFirstChild   ("Name");

            QString qstrTmp = walker.get().text().trimmed();
            this->mTXMWaveformName = qstrTmp;
        }

        {
            // read max amplitude
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Waveform");
            walker.goToFirstChild   ("MaxAmplitude");

            QString qstrTmp = walker.get().text().trimmed();
            bool    bOK;
            (void) qstrTmp.toDouble(&bOK);
            if (bOK == false)
            {
                this->mTXMMaxAmpl = DefaultValues::Undefined;
            }
            else
            {
                this->mTXMMaxAmpl = qstrTmp;
            }
        }


        {
            // read dipole rotation frequency (equals dipole TX time value)
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Sequence");
            walker.goToFirstChild   ("DipoleRotFreq");

            QString qstrTmp = walker.get().text().trimmed();
            bool    bOK;
            (void) qstrTmp.toDouble(&bOK);
            if (bOK == false)
            {
                this->mTXMDipTXTime = DefaultValues::Undefined;
            }
            else
            {
                this->mTXMDipTXTime = QString::number (1.0/qstrTmp.toDouble());
            }
        }

        {
            // read dipole TX frequency
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Sequence");
            walker.goToFirstChild   ("TransmissionList");
            walker.goToFirstChild   ("Cycle");
            walker.goToFirstChild   ("BaseFrequency");

            QString qstrTmp = walker.get().text().trimmed();
            bool    bOK;
            (void) qstrTmp.toDouble(&bOK);
            if (bOK == false)
            {
                this->mTXMBaseFreq = DefaultValues::Undefined;
            }
            else
            {
                this->mTXMBaseFreq = qstrTmp;
            }
        }

        {
            // read num iterations
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Sequence");
            walker.goToFirstChild   ("TransmissionList");
            walker.goToFirstChild   ("Cycle");
            walker.goToFirstChild   ("Iterations");

            QString qstrTmp = walker.get().text().trimmed();
            bool    bOK;
            (void) qstrTmp.toUInt (&bOK);
            if (bOK == false)
            {
                this->mTXMIterations = DefaultValues::Undefined;
            }
            else
            {
                this->mTXMIterations = qstrTmp;
            }
        }

        {
            // read number of dipoles and dipole polarisations
            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("Sequence");
            walker.goToFirstChild   ("TransmissionList");
            walker.goToFirstChild   ("Cycle");

            this->mTXMNumDipoles = QString::number (walker.getNumChilds("Polarisation"));


            // now compute the number of stacks from dipole tx time, number of dipoles and job length
            if ((this->mTXMIterations != DefaultValues::Undefined) &&
                (this->mTXMBaseFreq   != DefaultValues::Undefined) &&
                (this->mTXMNumDipoles != DefaultValues::Undefined) &&
                (this->mTXMDipTXTime  != DefaultValues::Undefined))
            {
                double dNumIterations = this->mTXMIterations.toDouble();
                double dBaseFreq      = this->mTXMBaseFreq.toDouble  ();
                double dNumDipoles    = this->mTXMNumDipoles.toDouble();
                double dDipTXTime     = this->mTXMDipTXTime.toDouble ();
                double dTmp;

                dTmp = dNumIterations / dBaseFreq;

                qDebug () << "[" << __PRETTY_FUNCTION__ << "] TXM job length:" << dTmp << "s";

                dTmp = dTmp / (dNumDipoles * dDipTXTime);
                dTmp = ceil (dTmp);

                if (dTmp <= 0.0)
                {
                    dTmp = 1.0;
                }

                qDebug () << "[" << __PRETTY_FUNCTION__ << "] TXM num stacks:" << dTmp;

                this->mTXMNumStacks = QString::number (dTmp);
            }
        }

        {
            // read number of dipoles and dipole polarisations
            bool bOK;

            this->mTXMPolarisations.resize(DefaultValues::TXMMaxNumDipoles);
            for (unsigned int uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
            {
                this->mTXMPolarisations [uiCounter] = 0.0;
            }

            for (unsigned int uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
            {
                Tools::DomElementWalker walker( mJobDocument );
                walker.assertElementName("measurement");
                walker.goToFirstChild   ("recording");
                walker.goToFirstChild   ("input");
                walker.goToFirstChild   ("TXM22Hardware");
                walker.goToFirstChild   ("Sequence");
                walker.goToFirstChild   ("TransmissionList");
                walker.goToFirstChild   ("Cycle");

                if (walker.goToChildNode ("Polarisation", "dipole", QString::number(uiCounter)) == true)
                {
                    this->mTXMPolarisations [uiCounter] = walker.get().text().trimmed().toDouble(&bOK);
                    if (bOK == false)
                    {
                        this->mTXMPolarisations [uiCounter] = 0.0;
                    }
                }
            }

            qDebug () << "[" << __PRETTY_FUNCTION__ << "] found polarisation angles:" << this->mTXMPolarisations;
        }


        {
            // read TXM electrode positions from XML file
            bool bOK;

            this->qvecTXMElecPosLat.resize (DefaultValues::TXMNumElectrodePos);
            this->qvecTXMElecPosLon.resize (DefaultValues::TXMNumElectrodePos);
            this->qvecTXMElecPosElev.resize(DefaultValues::TXMNumElectrodePos);

            for (unsigned int uiCounter = 0; uiCounter < DefaultValues::TXMNumElectrodePos; uiCounter++)
            {
                this->qvecTXMElecPosLat  [uiCounter] = 0.0;
                this->qvecTXMElecPosLon  [uiCounter] = 0.0;
                this->qvecTXMElecPosElev [uiCounter] = 0.0;
            }

            Tools::DomElementWalker walker( mJobDocument );
            walker.assertElementName("measurement");
            walker.goToFirstChild   ("recording");
            walker.goToFirstChild   ("input");
            walker.goToFirstChild   ("TXM22Hardware");
            walker.goToFirstChild   ("ElectrodePositions");
            for (unsigned int uiCounter = 0; uiCounter < 3; uiCounter++)
            {
                QString qstrTmp = "E" + QString::number (uiCounter + 1);
                walker.goToFirstChild (qstrTmp);

                // read center electrode position only for first electrode E1
                if (uiCounter == 0)
                {
                    if (walker.goToChildNode ("Position", "id", "0") == true)
                    {
                        walker.goToFirstChild ("Latitude");

                        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: center electrode latitude: " << walker.get().text();

                        this->qvecTXMElecPosLat [C_TXM_ELECTRODE_CENTER] = walker.get().text().trimmed().toInt(&bOK);
                        if (bOK == false)
                        {
                            this->qvecTXMElecPosLat [C_TXM_ELECTRODE_CENTER] = 0.0;
                        }
                        walker.goToParent();

                        walker.goToFirstChild ("Longitude");

                        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: center electrode longitude: " << walker.get().text();

                        this->qvecTXMElecPosLon [C_TXM_ELECTRODE_CENTER] = walker.get().text().trimmed().toInt(&bOK);
                        if (bOK == false)
                        {
                            this->qvecTXMElecPosLon [C_TXM_ELECTRODE_CENTER] = 0.0;
                        }
                        walker.goToParent();

                        walker.goToFirstChild ("Elevation");

                        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: center electrode elevation: " << walker.get().text();

                        this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER] = walker.get().text().trimmed().toInt(&bOK);
                        if (bOK == false)
                        {
                            this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER] = 0.0;
                        }
                        walker.goToParent();
                    }
                    walker.goToParent();
                }

                if (walker.goToChildNode ("Position", "id", "1") == true)
                {
                    walker.goToFirstChild ("Latitude");
                    this->qvecTXMElecPosLat [uiCounter + 1] = walker.get().text().trimmed().toInt(&bOK);
                    if (bOK == false)
                    {
                        this->qvecTXMElecPosLat [uiCounter + 1] = 0.0;
                    }
                    walker.goToParent();

                    walker.goToFirstChild ("Longitude");
                    this->qvecTXMElecPosLon [uiCounter + 1] = walker.get().text().trimmed().toInt(&bOK);
                    if (bOK == false)
                    {
                        this->qvecTXMElecPosLon [uiCounter + 1] = 0.0;
                    }
                    walker.goToParent();

                    walker.goToFirstChild ("Elevation");
                    this->qvecTXMElecPosElev [uiCounter + 1] = walker.get().text().trimmed().toInt(&bOK);
                    if (bOK == false)
                    {
                        this->qvecTXMElecPosElev [uiCounter + 1] = 0.0;
                    }
                    walker.goToParent();
                }
                walker.goToParent();
                walker.goToParent();
            }
        }
    }
}


bool Job::goToATSWriterNode (QDomDocument& clDOMDoc, QSharedPointer<Tools::DomElementWalker>& pclWalker, QString& qstrDigFilType, double& dDecRate)
{
    // declaration of variables
    bool bRetValue      = true;
    bool bIsDigFilJob   = false;
    bool bIsAutoCorrJob = false;

    qstrDigFilType.clear();
    dDecRate = 1.0;

    // go to ATSWriter part - look, if we possibly have a DigitalFilter job. in this
    // case we have a different path to the ATSWriter
    try
    {
        // check, if we have normal job without digital filter
        try
        {
            pclWalker = QSharedPointer<Tools::DomElementWalker> (new Tools::DomElementWalker (clDOMDoc));
            pclWalker->assertElementName ("measurement");
            pclWalker->goToFirstChild    ("recording");
            pclWalker->goToFirstChild    ("output");
            pclWalker->goToFirstChild    ("ProcessingTree");
            pclWalker->goToFirstChild    ("output");
            pclWalker->goToFirstChild    ("ATSWriter");
        }
        catch (...)
        {
            bIsDigFilJob = true;
        }

        // check, if we have a digital filter job
        if (bIsDigFilJob == true)
        {
            try
            {
                pclWalker.clear ();
                pclWalker = QSharedPointer<Tools::DomElementWalker> (new Tools::DomElementWalker (clDOMDoc));
                pclWalker->assertElementName ("measurement");
                pclWalker->goToFirstChild    ("recording");
                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("ProcessingTree");
                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("DigitalFilter");

                // if we got here, read the DigitalFilter type
                pclWalker->goToFirstChild     ("configuration");
                QDomElement clNode = pclWalker->get();
                Tools::parseOptionalChildInto (clNode, "filter_type", qstrDigFilType);

                if (qstrDigFilType.indexOf("mtx32x") != -1)
                {
                    dDecRate = 32.0;
                }
                if (qstrDigFilType.indexOf("mtx8x") != -1)
                {
                    dDecRate = 8.0;
                }
                if (qstrDigFilType.indexOf("mtx4x") != -1)
                {
                    dDecRate = 4.0;
                }
                if (qstrDigFilType.indexOf("mtx2x") != -1)
                {
                    dDecRate = 2.0;
                }

                pclWalker->goToParent ();

                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("ATSWriter");
            }
            catch (...)
            {
                qDebug () << "[" << __PRETTY_FUNCTION__ << "]: no DigitalFilter/ATSWriter node found";
                bIsDigFilJob   = false;
                bIsAutoCorrJob = true;
            }
        }


        // check, if we have a AutoCorr job
        if (bIsAutoCorrJob == true)
        {
            try
            {
                pclWalker.clear ();
                pclWalker = QSharedPointer<Tools::DomElementWalker> (new Tools::DomElementWalker (clDOMDoc));
                pclWalker->assertElementName ("measurement");
                pclWalker->goToFirstChild    ("recording");
                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("ProcessingTree");
                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("AutoCorr");
                pclWalker->goToFirstChild    ("output");
                pclWalker->goToFirstChild    ("ATSWriter");
            }
            catch (...)
            {
                qDebug () << "[" << __PRETTY_FUNCTION__ << "]: no AutoCorr/ATSWriter node found";

                bIsAutoCorrJob = false;
                bRetValue      = false;
            }
        }
    }
    catch (...)
    {
        qstrDigFilType.clear();
        dDecRate  = 1.0;
        bRetValue = false;
    }

    return (bRetValue);
}


void Job::parseChannelData()
{
    // parse channel configuration
    try
    {
        Tools::DomElementWalker channelTagWalker( mJobDocument );
        channelTagWalker.assertElementName( "measurement" );
        channelTagWalker
            .goToFirstChild( "recording")
            .goToFirstChild( "input" )
            .goToFirstChild( "Hardware" )
            .goToFirstChild( "channel_config" );

        if ( channelTagWalker.hasChild( "channel" ) ) {

            channelTagWalker.goToFirstChild( "channel" );
            Tools::DomElementSiblingIterator it0( channelTagWalker );
            while ( it0.isValid() ) {
                it0.assertElementName( "channel" ); // remove this later!
                JobChannelConfig::Spt cfg = JobChannelConfig::createFromXmlChannelTag( mDataModel, it0.get() );
                mJobChannelConfigVector.at( cfg -> getChannelId() ) = cfg;
                it0.next();
            }
        }
    }
    catch ( const std::runtime_error& e )
    {
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ << " unable to parse input channel specific data. reason : " << e.what();
        throw std::runtime_error( ss.str() );
    }

    // parse ATSWriter part - look, if we possibly have a DigitalFilter job. in this
    // case we have a different path to the ATSWriter
    try
    {
        QSharedPointer<Tools::DomElementWalker> pclOutputChannelWalker;

        this->goToATSWriterNode(this->mJobDocument, pclOutputChannelWalker, this->mDigFilType, this->mDigFilDecrate);
        pclOutputChannelWalker->goToFirstChild    ("configuration");
        pclOutputChannelWalker->goToFirstChild    ("channel");
        
        Tools::DomElementSiblingIterator it1(*pclOutputChannelWalker);
        while (it1.isValid())
        {

            unsigned int channelId = JobChannelConfig::parseOutputChannelId( it1.get() );

            if ( channelId > ( DefaultValues::MaxChannels - 1 ) ) {
                qDebug()  << "Waring: unable to parse output channel info for channel number : " << channelId << " , ignoring this entry!";
                continue;
            }

            JobChannelConfig::Spt cfg = mJobChannelConfigVector.at( channelId );
            if ( cfg.isNull() ) {
                qDebug()  << "Waring: no input channel defined for output channel number : " << channelId << " , ignoring this entry!";
                continue;
            }

            cfg -> parseOutputChannelElement( it1.get() );

            it1.next();
        }
    }
    catch ( const std::runtime_error& e )
    {
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ << " unable to parse output channel specific data . reason : " << e.what();
        throw std::runtime_error( ss.str() );
    }
}

void Job::parseComments()
{    
    // read processing instructions
    // MWI: CAUTION: ONLY USING PROCESSING TREE 1

    QSharedPointer<Tools::DomElementWalker> pclWalker = QSharedPointer<Tools::DomElementWalker> (new Tools::DomElementWalker (this->mJobDocument));
    pclWalker->assertElementName  ("measurement");
    pclWalker->goToFirstChild     ("recording");
    pclWalker->goToFirstChild     ("output");
    pclWalker->goToFirstChild     ("ProcessingTree");
    pclWalker->goToFirstChild     ("configuration");
    Tools::parseOptionalChildInto (pclWalker->get(), "processings", mProcessings);
    pclWalker.clear();

    if (this->goToATSWriterNode(this->mJobDocument, pclWalker, this->mDigFilType, this->mDigFilDecrate) == false)
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] no ATSWriter node found";
    }
    else
    {

        pclWalker->goToFirstChild     ("comments");
        QDomElement commentElement = pclWalker->get();
        Tools::parseOptionalChildInto (commentElement, "operator"          , mOperator);
        Tools::parseOptionalChildInto (commentElement, "client"            , mClient);
        Tools::parseOptionalChildInto (commentElement, "contractor"        , mContractor);
        Tools::parseOptionalChildInto (commentElement, "area"              , mArea);
        Tools::parseOptionalChildInto (commentElement, "survey_id"         , mSurveyId);
        Tools::parseOptionalChildInto (commentElement, "weather"           , mWeather);
        Tools::parseOptionalChildInto (commentElement, "general_comments"  , mComment);
        Tools::parseOptionalChildInto (commentElement, "site_name"         , mSiteName);
        Tools::parseOptionalChildInto (commentElement, "site_name_rr"      , mSiteNameRR);
        Tools::parseOptionalChildInto (commentElement, "site_name_emap"      , mSiteNameEMAP);
        pclWalker->goToParent         ();

        {
            Tools::DomElementWalker w( mJobDocument );
            w.assertElementName( "measurement" );
            w.goToFirstChild( "recording" )
             .goToFirstChild( "input" )
             .goToFirstChild( "TXM22Hardware" )
             .goToFirstChild( "Sequence" )
             .goToFirstChild( "TransmissionList" )
             .goToFirstChild( "Cycle" );

            Tools::parseOptionalChildInto(w.get(), "BaseFrequency", mBaseFrequency);

            qDebug() << "[" << __PRETTY_FUNCTION__ << "] reading TXM base frequency:" << mBaseFrequency;
        }

        {
            Tools::DomElementWalker w( mJobDocument );
            w.assertElementName( "measurement" );
            w.goToFirstChild( "recording" )
                .goToFirstChild( "input" )
                .goToFirstChild( "TXM22Hardware" )
                .goToFirstChild( "Waveform" );
            Tools::parseOptionalChildInto( w.get(), "Name" , mTXMWaveformName );
        }

        {
            pclWalker->goToFirstChild     ("output_file") ;
            Tools::parseOptionalChildInto (pclWalker->get(), "line_num", mLineNumber);
            Tools::parseOptionalChildInto (pclWalker->get(), "run_num",  mRunNumber);
            Tools::parseOptionalChildInto (pclWalker->get(), "site_num", mSiteId);
            pclWalker->goToParent         ();
        }
        pclWalker.clear();
    }
}


void Job::saveFile()
{
    // delcaration of variables
    QString qstrTmp;
    double  dTmp;

    updateDocument(); // update inferior dom xml

    // please keep in mind: if the filename of this job does not exist, it is created at the point in time when the save-command runs.
    // see void DataModel::updateJobListXmlTag() ...        (*it) -> updateJobFileName();
    // the filename will not be altered after this, even if the job changes and begins at another point in time.

    QFileInfo fileInfo ( mJobFileName );
    QString jobFileDir = mDataModel -> getJobDirectory() + mDataModel -> getJobSubdir();
    QDir saveDirectory( jobFileDir );
    if ( mJobFileName.isEmpty() ) {
        qDebug()  << "Job does not have a filename; no file written. Frequency: " << getFrequency(qstrTmp, dTmp) << " starttime: " << getStartTime().toString();
        return;
    }
    QString outFileName = saveDirectory.filePath( fileInfo.fileName() );

    // delete existing header and insert a new one
    QDomNode xmlNode = mJobDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"ISO-8859-1\"");
    mJobDocument.replaceChild(xmlNode, mJobDocument.firstChild());

    QByteArray result = mJobDocument.toByteArray( 4 );
    QFile outfile( outFileName );

    outfile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    qint64 writeRestult = outfile.write( result );
    outfile.close();

    if ( writeRestult == -1 ) {
        std::stringstream ss;
        ss << "unable to write file : " << outFileName.toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }    
}

QString Job::getJobFileName() const { 

    if ( ( not mUseGeneratedJobFileName ) or
         ( mDataModel == 0 ) )
        return mJobFileName;

    // Hmm... this implementation of QFileInfo does not contact the underlying filesystem and manipulates pathes only
    // There is no real guarantee the this will be the same in the future. 
    // but dont be afraid, boost::filesystem will come to rescue!
    QFileInfo jobFileInfo( mJobFileName );
    QFileInfo newFileInfo = QFileInfo( mDataModel -> getJobSubdir() + "/" + jobFileInfo.fileName() );
    return newFileInfo.filePath();
}


Types::Frequency Job::getFrequency (QString& qstrDigFilType, double& dDigFilDecRate) const
{     
    qstrDigFilType = this->mDigFilType;
    dDigFilDecRate = this->mDigFilDecrate;
    return (mSampleFreq / this->mDigFilDecrate);
}

Types::Time Job::getStartTime() const 
{ 
    return mStartTime;
}

Types::Date Job::getStartDate() const
{
    return mStartDate;
}

time_t Job::getStartDateTimeUnix () const
{
    // declaration of variables
    time_t      sRetValue;
    QDateTime   clStartDateTime (this->mStartDate, this->mStartTime);

    sRetValue = clStartDateTime.toTime_t();

    return (sRetValue);
}

Types::Time Job::getStopTime() const
{
    return mStopTime;
}

Types::Date Job::getStopDate()  const
{
    return mStartDate;
}


Types::TimeInterval Job::getDuration() const 
{ 
    return mDurationSec;
}

ControlData::MeasurementMode Job::getMeasurementMode() const
{
    return mMeasurementMode;
}

void Job::setMeasurementMode( ControlData::MeasurementMode measurementMode )
{
    mMeasurementMode = measurementMode;
}

QString Job::getTXMNumStacks()
{
    return mTXMNumStacks.isEmpty() ? DefaultValues::Undefined : mTXMNumStacks;
}

QString Job::getTXMDipTXTime()
{
    return mTXMDipTXTime.isEmpty() ? DefaultValues::Undefined : mTXMDipTXTime;
}

QString Job::getTXMNumDipoles()
{
    return mTXMNumDipoles.isEmpty() ? DefaultValues::Undefined : mTXMNumDipoles;
}

bool Job::setTXMNumStacks ( const QString& value )
{
    mTXMNumStacks = value;
    return (true);
}

bool Job::setTXMDipTXTime ( const QString& value )
{
    mTXMDipTXTime = value;
    return (true);
}

bool Job::setTXMNumDipoles ( const QString& value )
{
    mTXMNumDipoles = value;
    return (true);
}

Job& Job::setStartDateTime( const QDateTime & newStart )
{
    mStartTime = newStart.time();
    mStartDate = newStart.date();
    calculateData();
    return *this;
}

Job& Job::setDuration( const Types::TimeInterval& duration )
{
    mDurationSec = duration;
    calculateData();
    return *this;
}

Job& Job::setFrequency (const Types::Frequency& frequency)
{
    mSampleFreq = (frequency * this->mDigFilDecrate);
    return *this;
    // doConstraintCheck(); // do somthing like this...? 
}


void Job::setEnableBoardChannel( unsigned channel , bool enable )
{
    getJobChannelConfigForChannel( channel ) -> setIsEnabled( enable );
}

void Job::setExtGainForChannel(Types::Index const& channel, QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setExtGain( value );
}

void Job::setChopperForChannel( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setChopper( value );    
}

void Job::setRfFilterForChannel( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setRfFilter( value );
}

void Job::setGain1ForChannel( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setGain1( value );
}

void Job::setDacForChannel( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setDac( value );
}

void Job::setLpHpFilterForChannel ( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setLpHpFilter( value );
}

void Job::setGain2ForChannel( const Types::Index& channel , QString value )
{
    getJobChannelConfigForChannel( channel.get() ) -> setGain2( value );
}

bool Job::getIsChannelEnabled ( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> isEnabled();
}


QString Job::getExtGainForChannel    ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getExtGain();
}

QString Job::getChopperForChannel    ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getHChopper();
}

QString Job::getRfFilterForChannel   ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getRfFilter();
}

QString Job::getGain1ForChannel      ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getGain1();
}

QString Job::getDacForChannel        ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getDacVal();
}

QString Job::getLpHpFilterForChannel ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getLpHpFilter();
}

QString Job::getGain2ForChannel      ( const Types::Index& channel )
{
    return getJobChannelConfigForChannel( channel.get() ) -> getGain2();
}

////////////////////////////////////////////////////////////////////////////////
//
// postitions stuff.. 

QString Job::getPosX1ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosX1();
}

QString Job::getPosY1ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosY1();
}

QString Job::getPosZ1ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosZ1();
}

QString Job::getPosX2ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosX2();
}

QString Job::getPosY2ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosY2();
}

QString Job::getPosZ2ForChannel( const Types::Index& channel ) const
{
    return getJobChannelConfigForChannel( channel.get() ) -> getPosZ2();
}


void Job::setPosX1ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosX1( value );
}
void Job::setPosY1ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosY1( value );
}
void Job::setPosZ1ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosZ1( value );
}
void Job::setPosX2ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosX2( value );
}
void Job::setPosY2ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosY2( value );
}
void Job::setPosZ2ForChannel( const Types::Index& channel , QString value ) // throws  std::out_of_range
{
    getJobChannelConfigForChannel( channel.get() ) -> setPosZ2( value );
}



JobChannelConfig* Job::getJobChannelConfigForChannel( unsigned channelNumber ) const // throws std::out_of_range
{
    // do we have a reason to trust QSharedPointer ? No...
    JobChannelConfig::Spt jcc = mJobChannelConfigVector.at( channelNumber );

    return jcc.isNull() ? 0 : jcc.data();
}

void Job::calculateData()
{
    // FIXME: we might need to recalc duration here...
    QDateTime newStopDateTime( mStartDate, mStartTime );
    newStopDateTime = newStopDateTime.addSecs( mDurationSec.asSeconds() );
    mStopTime = newStopDateTime.time();
    mStopDate = newStopDateTime.date();

}

void Job::updateDocument()
{
    calculateData();

    Tools::updateDomTimeElement( mStartTimeElement, mStartTime );
    Tools::updateDomDateElement( mStartDateElement, mStartDate );

    Tools::updateDomTimeElement( mStopTimeElement, mStopTime );
    Tools::updateDomDateElement( mStopDateElement, mStopDate );

    Tools::updateDomUIntElement( mSampleFreqElement, mSampleFreq );

    (void) updateCyclicJobParameters ();

    updateChannelConfigTag();
    // updateDetectedHardwareTag();
    updateMeasTypeTag();
    updateCommentsTag();
    updateCalibrationTags();
    updateTXMTags();

}


void Job::updateTXMTags()
{
    // declaration of variables
    double       dTmp;
    unsigned int uiCounter;

    // update waveform name
    Tools::DomElementWalker clWalker (mJobDocument);
    clWalker.assertElementName ("measurement");
    clWalker.goToFirstChild    ("recording");
    clWalker.goToFirstChild    ("input");
    clWalker.goToFirstChild    ("TXM22Hardware");
    clWalker.goToFirstChild    ("Waveform");
    clWalker.goToFirstChild    ("Name");
    clWalker.get().firstChild().setNodeValue(this->mTXMWaveformName);

    // update waveform amplitude
    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("MaxAmplitude");
    clWalker.get().firstChild().setNodeValue(this->mTXMMaxAmpl);

    // update dipole rotation freq (dipole TX time)
    clWalker.goToParent        ();
    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("Sequence");
    clWalker.goToFirstChild    ("DipoleRotFreq");
    clWalker.get().firstChild().setNodeValue(QString::number(1.0/this->mTXMDipTXTime.toDouble()));

    // update base frequency
    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("TransmissionList");
    clWalker.goToFirstChild    ("Cycle");
    clWalker.goToFirstChild    ("BaseFrequency");
    clWalker.get().firstChild().setNodeValue(this->mTXMBaseFreq);

    // update iterations
    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("Iterations");
    dTmp  = this->mTXMDipTXTime.toDouble() * this->mTXMNumDipoles.toDouble() * this->mTXMNumStacks.toDouble();
    dTmp *= this->mTXMBaseFreq.toDouble();
    unsigned int uiTmp = (unsigned int) dTmp;

    qDebug () << "Number of Iteration:" << dTmp << " / " << uiTmp;

    clWalker.get().firstChild().setNodeValue(QString::number(uiTmp));

    // update polarisations
    clWalker.goToParent        ();
    for (uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
    {
        clWalker.removeChildNode("Polarisation", "dipole", QString::number (uiCounter));
    }

    for (uiCounter = 0; uiCounter < this->mTXMNumDipoles.toUInt(); uiCounter++)
    {
        clWalker.addChild("Polarisation", QString::number (this->mTXMPolarisations [uiCounter]), "dipole", QString::number (uiCounter));
    }

    // update Electrode Positions
    clWalker.goToParent        ();
    clWalker.goToParent        ();
    clWalker.goToParent        ();
    for (uiCounter = 0; uiCounter < 3; uiCounter++)
    {
        clWalker.goToFirstChild ("ElectrodePositions");
        QString qstrTmp = "E" + QString::number (uiCounter + 1);
        clWalker.goToFirstChild (qstrTmp);

        // update center electrode
        clWalker.goToChildNode  ("Position", "id", "0");
        clWalker.goToFirstChild ("Longitude");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosLon [C_TXM_ELECTRODE_CENTER]));
        clWalker.goToParent     ();
        clWalker.goToFirstChild ("Latitude");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosLat [C_TXM_ELECTRODE_CENTER]));
        clWalker.goToParent     ();
        clWalker.goToFirstChild ("Elevation");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER]));
        clWalker.goToParent     ();
        clWalker.goToParent     ();

        // update electrode values for electrode
        clWalker.goToChildNode  ("Position", "id", "1");
        clWalker.goToFirstChild ("Longitude");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosLon [uiCounter + 1]));
        clWalker.goToParent     ();
        clWalker.goToFirstChild ("Latitude");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosLat [uiCounter + 1]));
        clWalker.goToParent     ();
        clWalker.goToFirstChild ("Elevation");
        clWalker.get().firstChild().setNodeValue(QString::number(this->qvecTXMElecPosElev [uiCounter + 1]));
        clWalker.goToParent     ();
        clWalker.goToParent     ();

        clWalker.goToParent     ();
        clWalker.goToParent     ();
    }
}


bool Job::updateCyclicJobParameters(void)
{
    // declaration of variables

    Tools::DomElementWalker clWalker (mJobDocument);
    clWalker.assertElementName ("measurement");
    clWalker.goToFirstChild    ("recording");
    clWalker.goToFirstChild    ("CyclicEvent");
    clWalker.get().firstChild().setNodeValue(this->mCyclicEvent);

    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("Cycle");
    clWalker.get().firstChild().setNodeValue(this->mCyclicDuration);

    clWalker.goToParent        ();
    clWalker.goToFirstChild    ("Granularity");
    clWalker.get().firstChild().setNodeValue(this->mCyclicUnit);

    return (true);
}


void Job::updateChannelConfigTag()
{
    // declaration of variables
    QString qstrTmp;
    double  dTmp;

    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "recording" )
            .goToFirstChild( "input" )
            .goToFirstChild( "Hardware" )
            .goToFirstChild( "channel_config" );

        QDomElement channelConfigTag = walker.get();
    
        QDomElement channeltag = channelConfigTag.firstChildElement( "channel" );

        while ( not channeltag.isNull() ) {
            channelConfigTag.removeChild( channeltag );
            channeltag = channelConfigTag.firstChildElement( "channel" );
        }

        for ( JobChannelConfigVector::const_iterator it = mJobChannelConfigVector.begin(); 
              it != mJobChannelConfigVector.end();
              ++it ) 
        {

            if ( not mDataModel -> getSensorHWEnabledInputAtChannel( (*it) -> getChannelId() ) )
                continue;

            // skip disabled thing...
            if ( not (*it) -> isEnabled() )
                continue;

            QDomElement newChannelTag = mJobDocument.createElement( "channel" );
            newChannelTag.setAttribute( "id" , (*it) -> getChannelId() ); //<channel id="0">

            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "gain_stage1" , (*it)  -> getGain1() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "gain_stage2" , (*it)  -> getGain2() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "ext_gain"    , (*it)  -> getExtGain() ) );
            QStringList filterList;
            QString rfFilter = (*it) -> getRfFilter();
            if ( not ( rfFilter.isEmpty() or ( rfFilter.compare( DefaultValues::Off ) == 0 ) ) )
                filterList.append( rfFilter );
            QString lpHpFilter = (*it) -> getLpHpFilter();
            if ( not ( lpHpFilter.isEmpty() or ( lpHpFilter.compare( DefaultValues::Off ) == 0 ) ) )
                filterList.append( lpHpFilter );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "filter_type" , filterList.join(",") ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "echopper"    , "0" ) );
            QString chopperValue = (*it)  -> getHChopper();
            if (chopperValue.compare( DefaultValues::Auto ) == 0 )
                chopperValue = getFrequency(qstrTmp, dTmp) <= DefaultValues::ChopperAutoFrequency ? DefaultValues::One : DefaultValues::Zero;
            
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "hchopper"    , chopperValue ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "dac_val"     , (*it)  -> getDacVal() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "dac_on"      , (*it)  -> getDacOn() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "input"       , (*it)  -> getInputSelection() ) );

            QString sensorType    = mDataModel -> getSensorHWSensorTypeInputAtChannel( (*it) -> getChannelId() );
            QString input_divider = this->getInputDivForSensor (sensorType);
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "input_divider" , input_divider ) );

            channelConfigTag.appendChild( newChannelTag );
        }
    }

    // new scope... 
    {
        QSharedPointer<Tools::DomElementWalker> pclWalker;

        this->goToATSWriterNode(this->mJobDocument, pclWalker, this->mDigFilType, this->mDigFilDecrate);
        pclWalker->goToFirstChild ("configuration");
        
        QDomElement configurationTag = pclWalker->get();
        QDomElement channeltag       = configurationTag.firstChildElement ("channel");

        while (not channeltag.isNull())
        {
            configurationTag.removeChild (channeltag);
            channeltag = configurationTag.firstChildElement ("channel");
        }

        for ( JobChannelConfigVector::const_iterator it = mJobChannelConfigVector.begin(); 
              it != mJobChannelConfigVector.end();
              ++it ) 
        {

            if ( not mDataModel -> getSensorHWEnabledInputAtChannel( (*it) -> getChannelId() ) )
                continue;

            // if the jobchannelconfig has SensorChannelConfig information, these will override those from jobchannelconfig ( the latter contains only per job information parsesd from xml! )
            // FIXME: this has to be done earlier..
            (*it) -> setSensorChannelConfig( mDataModel -> getSensorChannelConfig() );

            // skip disabled thing...
            if ( not (*it) -> isEnabled() )
                continue;

            QDomElement newChannelTag = mJobDocument.createElement( "channel" );
            newChannelTag.setAttribute( "id" , (*it) -> getChannelId() );

            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "start_time"       , (*it)  -> getStartTime() ) );    
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "start_date"       , (*it)  -> getStartDate() ) );      
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "sample_freq"      , (*it)  -> getSampleFreq() ) );    
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "num_samples"      , (*it)  -> getNumSamples() ) );    
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "ats_data_file"    , (*it)  -> getAtsDataFile() ) );   
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "channel_type"     , (*it)  -> getChannelType() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "sensor_sernum"    , (*it)  -> getSensorSerialNumber() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "sensor_type"      , (*it)  -> getSensorType() ) );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "ts_lsb"           , (*it)  -> getTsLsb() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "sensor_cal_file"  , (*it)  -> getSensorCalFile() ) ); 
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_x1"           , (*it)  -> getPosX1() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_y1"           , (*it)  -> getPosY1() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_z1"           , (*it)  -> getPosZ1() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_x2"           , (*it)  -> getPosX2() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_y2"           , (*it)  -> getPosY2() ) );         
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "pos_z2"           , (*it)  -> getPosZ2() ) );         

            configurationTag.appendChild( newChannelTag );

        }
    }

    // edit InputBufferSize for OpenMT_DataInterface, if found as this is needed for TXM-22
    // joblists
    {
        Tools::DomElementWalker clWalker( mJobDocument );
        bool bFoundNode = true;
        clWalker.assertElementName("measurement");
        clWalker.goToFirstChild   ("recording");
        clWalker.goToFirstChild   ("output");

        try
        {
            clWalker.goToFirstChild   ("ProcessingTree2");
            clWalker.goToFirstChild   ("output");
            clWalker.goToFirstChild   ("OpenMT_DataInterface");
            clWalker.goToFirstChild   ("ProcessDoc");
            clWalker.goToFirstChild   ("InputBufferSize");
        }
        catch (...)
        {
            bFoundNode = false;
        }

        if (bFoundNode == true)
        {
            // maximum window length is 65536 samples
            if ((((double)this->mSampleFreq) * 4.0) < 65536.0)
            {
                clWalker.get().firstChild().setNodeValue(QString::number((unsigned int)(((double)this->mSampleFreq) * 4.0)));
            }
            else
            {
                clWalker.get().firstChild().setNodeValue("65536");
            }
        }
    }
}


const QString Job::getInputDivForSensor (const QString qstrSensorType)
{
    // declaration of variables
    QString qstrRetValue = "0"; ///< only check for those sensors, where input divider must be switched on

    if (qstrSensorType.indexOf(DefaultValues::MFS06) != -1)
    {
        // works for MFS06 and MFS06e
        qstrRetValue = "1";
    }
    else if (qstrSensorType.indexOf(DefaultValues::MFS07) != -1)
    {
        // works for MFS07 and MFS07e
        qstrRetValue = "1";
    }
    else if (qstrSensorType.indexOf(DefaultValues::FGS02) != -1)
    {
        qstrRetValue = "1";
    }
    else if (qstrSensorType.indexOf(DefaultValues::FGS03) != -1)
    {
        // works for FGS03 and FGS03e
        qstrRetValue = "1";
    }

    return (qstrRetValue);
}


// void Job::updateDetectedHardwareTag()
// {
//     if ( mDataModel == 0 )
//         return;

//     HardwareConfig::Spt hwCfg = mDataModel -> getHwConfig();

//     if ( hwCfg.isNull() )
//         return;

//     QDomElement HwConfigElement = hwCfg -> getXMLHwConfigNode().cloneNode().toElement() ;

//     if ( not HwConfigElement.isElement() ) {
//         qDebug()  << "no detected hardware data found, not writing to xml.";
//         return;
//     }

//     Tools::DomElementWalker walker( mJobDocument );
//     walker.assertElementName( "measurement" );
//     QDomElement measurementElement = walker.get();
//     QDomElement oldHwConfigElement =  measurementElement.firstChildElement( "HwConfig" );

//     if ( oldHwConfigElement.isNull() )
//         measurementElement.appendChild( HwConfigElement );
//     else
//         measurementElement.replaceChild( HwConfigElement , oldHwConfigElement );
// }

void Job::updateMeasTypeTag()
{
    QSharedPointer<Tools::DomElementWalker> pclWalker;
    this->goToATSWriterNode(this->mJobDocument, pclWalker, this->mDigFilType, this->mDigFilDecrate);
    pclWalker->goToFirstChild   ("configuration");
    pclWalker->goToFirstChild   ("meas_type");
    Tools::updateDomTextElement (pclWalker->get(), mMeasurementMode == ControlData::CSAMTMeasurementMode ? DefaultValues::CSAMT : DefaultValues::MT);
    pclWalker.clear();
}


void Job::updateCommentsTag()
{
    if (mDataModel == 0)
    {
        return;
    }

    QSharedPointer<Tools::DomElementWalker> pclWalker = QSharedPointer<Tools::DomElementWalker> (new Tools::DomElementWalker (this->mJobDocument));
    pclWalker->assertElementName("measurement");
    pclWalker->goToFirstChild   ("recording");
    pclWalker->goToFirstChild   ("output");
    pclWalker->goToFirstChild   ("ProcessingTree");
    pclWalker->goToFirstChild   ("configuration");
    QDomElement clTmpNode = pclWalker->get();
    Tools::createOrUpdateChildElement(mJobDocument, clTmpNode, "processings", mProcessings);
    pclWalker.clear ();

    this->goToATSWriterNode(this->mJobDocument, pclWalker, this->mDigFilType, this->mDigFilDecrate);

    QDomElement commentElement;
    if (not pclWalker->hasChild ("comments"))
    {
        commentElement = mJobDocument.createElement ("comments");
        pclWalker->get().appendChild (commentElement);
    }
    else
    {
        pclWalker->goToFirstChild ("comments");
        commentElement = pclWalker->get();
    }

    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "operator"          , mOperator );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "client"            , mClient );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "contractor"        , mContractor );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "area"              , mArea );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "survey_id"         , mSurveyId );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "weather"           , mWeather );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "general_comments"  , mComment );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "site_name"         , mSiteName );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "site_name_rr"      , mSiteNameRR );
    Tools::createOrUpdateChildElement (mJobDocument,  commentElement, "site_name_emap"      , mSiteNameEMAP );
    pclWalker->goToParent();

    {
        Tools::DomElementWalker w( mJobDocument );
        w.assertElementName( "measurement" );
        w.goToFirstChild( "recording" )
            .goToFirstChild( "input" )
            .goToFirstChild( "TXM22Hardware" )
            .goToFirstChild( "Sequence" )
            .goToFirstChild( "TransmissionList" )
            .goToFirstChild( "Cycle" );
        QDomElement elem = w.get();
        Tools::createOrUpdateChildElement( mJobDocument, elem , "BaseFrequency" , mBaseFrequency ); 
    }

    {
        Tools::DomElementWalker w( mJobDocument );
        w.assertElementName( "measurement" );    
        w.goToFirstChild( "recording" )
            .goToFirstChild( "input" )
            .goToFirstChild( "TXM22Hardware" )
            .goToFirstChild( "Waveform" );
        QDomElement elem = w.get();
        Tools::createOrUpdateChildElement( mJobDocument,  elem, "Name" , mTXMWaveformName );
    }

    {
        pclWalker->goToFirstChild ("output_file") ;
        QDomElement elem = pclWalker->get();
        Tools::createOrUpdateChildElement (mJobDocument, elem, "run_num",  mRunNumber);
        Tools::createOrUpdateChildElement (mJobDocument, elem, "site_num", mSiteId);
        Tools::createOrUpdateChildElement (mJobDocument, elem, "line_num", mLineNumber);
    }
    pclWalker.clear();
}


void Job::updateCalibrationTags()
{
    if ( mDataModel == 0 )
        return;

    {
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "calibration_channels" );

        QDomElement calibrationChannelTag = walker.get();
        QDomElement channeltag = calibrationChannelTag.firstChildElement( "channel" );
        while ( not channeltag.isNull() ) 
        {
            calibrationChannelTag.removeChild( channeltag );
            channeltag = calibrationChannelTag.firstChildElement( "channel" );
        }

        for ( JobChannelConfigVector::const_iterator it = mJobChannelConfigVector.begin(); 
              it != mJobChannelConfigVector.end();
              ++it ) 
        {
            if ( not mDataModel -> getSensorHWEnabledInputAtChannel( (*it) -> getChannelId() ) )
                continue;
            if ( not (*it) -> isEnabled() )
                continue;
            QDomElement newChannelTag = mJobDocument.createElement( "channel" );
            newChannelTag.setAttribute( "id" , (*it) -> getChannelId() );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "calibration"       , "" ) );    
            calibrationChannelTag.appendChild( newChannelTag );
        }
    }

    {
        // note : yes, this is a copy and paste from above. but i guess you will be happy i did this the way i did it when you start inserting real values here.
        Tools::DomElementWalker walker( mJobDocument );
        walker.assertElementName( "measurement" );
        walker
            .goToFirstChild( "calibration_sensors" );

        QDomElement calibrationSensorTag = walker.get();
        QDomElement channeltag = calibrationSensorTag.firstChildElement( "channel" );
        while ( not channeltag.isNull() ) 
        {
            calibrationSensorTag.removeChild( channeltag );
            channeltag = calibrationSensorTag.firstChildElement( "channel" );
        }

        for ( JobChannelConfigVector::const_iterator it = mJobChannelConfigVector.begin(); 
              it != mJobChannelConfigVector.end();
              ++it ) 
        {
            if ( not mDataModel -> getSensorHWEnabledInputAtChannel( (*it) -> getChannelId() ) )
                continue;
            if ( not (*it) -> isEnabled() )
                continue;
            QDomElement newChannelTag = mJobDocument.createElement( "channel" );
            newChannelTag.setAttribute( "id" , (*it) -> getChannelId() );
            newChannelTag.appendChild( Tools::createTextElement( mJobDocument, "calibration"       , "" ) );    
            calibrationSensorTag.appendChild( newChannelTag );
        }
    }
}


void Job::cloneDocument()
{
    QDomDocument tmp = mJobDocument;
    mJobDocument = tmp.cloneNode( true ).toDocument();

}

void Job::updateJobFileName()
{
    if ( not mJobFileName.isEmpty() ) 
        return;
    
    mJobFileName = generateFileNameForJobSubDir( mDataModel -> getJobSubdir() );
}


QString Job::generateFileNameForJobSubDir( const QString& jobSubDir ) const
{
    // delcaration of variables
    QString qstrTmp;
    double  dTmp;

    QString StartDate = getStartDate().toString( DefaultValues::DateFormat );
    QString StartTime = getStartTime().toString( DefaultValues::TimeFormatForFiles );
    QStringList tmp;
    for ( unsigned i = 0; i < DefaultValues::MaxChannels ; ++i ) tmp.append( getIsChannelEnabled( i ) ? QString::number( i ) : "x" );
    QString frequency = QString::number (getFrequency (qstrTmp, dTmp));
    QString channelMatrix = tmp.join("_");
    QString result = jobSubDir + "/" + StartDate + "_" + StartTime + "_" + channelMatrix + "_" + frequency + ".xml";

    return result;
}


QString Job::getTXMBaseFreq ()
{
    return (this->mTXMBaseFreq);
}


QString Job::getTXMIterations ()
{
    return (this->mTXMIterations);
}


QString Job::getTXMMaxAmpl ()
{
    return (this->mTXMMaxAmpl);
}


QString Job::getTXMWaveformName ()
{
    return (this->mTXMWaveformName);
}


QVector<double> Job::getTXMPolarisations ()
{
    return (this->mTXMPolarisations);
}


bool Job::setTXMBaseFreq (const QString& qstrValue)
{
    this->mTXMBaseFreq = qstrValue;
    return (true);
}


bool Job::setTXMIterations (const QString& qstrValue)
{
    this->mTXMIterations = qstrValue;
    return (true);
}


bool Job::setTXMMaxAmpl (const QString& qstrValue)
{
    this->mTXMMaxAmpl = qstrValue;
    return (true);
}


bool Job::setTXMWaveformName (const QString& qstrValue)
{
    this->mTXMWaveformName = qstrValue;
    return (true);
}


bool Job::setTXMPolarisations (const QVector<double> qvecValue)
{
    this->mTXMPolarisations = qvecValue;
    return (true);
}


QString Job::getCyclicEvent(void)
{
    return (this->mCyclicEvent);
}


QString Job::getCyclicDuration(void)
{
    return (this->mCyclicDuration);
}


QString Job::getCyclicUnit(void)
{
    return (this->mCyclicUnit);
}


bool Job::setCyclicEvent (const QString& qstrEvent)
{
    this->mCyclicEvent = qstrEvent;
    return (true);
}


bool Job::setCyclicDuration (const QString& qstrDuration)
{
    this->mCyclicDuration = qstrDuration;
    return (true);
}


bool Job::setCyclicUnit (const QString& qstrUnit)
{
    this->mCyclicUnit = qstrUnit;
    return (true);
}


bool Job::getTXMElectrodePosStr(const unsigned int uiElectrodeName, QString& qstrLat, QString& qstrLon, QString& qstrElevation)
{
    // declaration of variables
    bool    bRetValue = true;
    QString qstrTmp;

    switch (uiElectrodeName)
    {
        case C_TXM_ELECTRODE_CENTER:
            {
                geocoordinates* pclGeoCoordinates = new geocoordinates ();
                pclGeoCoordinates->set_lon_lat_msec (this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_CENTER],
                                                     this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_CENTER],
                                                     this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER]);

                qstrLat       = pclGeoCoordinates->get_lat_str_google_maps();
                qstrLon       = pclGeoCoordinates->get_lon_str_google_maps();
                qstrElevation = pclGeoCoordinates->get_elevation_str      (qstrTmp);
                delete (pclGeoCoordinates);
            }
            break;

        case C_TXM_ELECTRODE_E1:
            {
                geocoordinates* pclGeoCoordinates = new geocoordinates ();
                pclGeoCoordinates->set_lon_lat_msec (this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E1],
                                                     this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E1],
                                                     this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E1]);

                qstrLat       = pclGeoCoordinates->get_lat_str_google_maps();
                qstrLon       = pclGeoCoordinates->get_lon_str_google_maps();
                qstrElevation = pclGeoCoordinates->get_elevation_str      (qstrTmp);
                delete (pclGeoCoordinates);
            }
            break;

        case C_TXM_ELECTRODE_E2:
            {
                geocoordinates* pclGeoCoordinates = new geocoordinates ();
                pclGeoCoordinates->set_lon_lat_msec (this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E2],
                                                     this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E2],
                                                     this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E2]);

                qstrLat       = pclGeoCoordinates->get_lat_str_google_maps();
                qstrLon       = pclGeoCoordinates->get_lon_str_google_maps();
                qstrElevation = pclGeoCoordinates->get_elevation_str      (qstrTmp);
                delete (pclGeoCoordinates);
            }
            break;

        case C_TXM_ELECTRODE_E3:
            {
                geocoordinates* pclGeoCoordinates = new geocoordinates ();
                pclGeoCoordinates->set_lon_lat_msec (this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E3],
                                                     this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E3],
                                                     this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E3]);

                qstrLat       = pclGeoCoordinates->get_lat_str_google_maps();
                qstrLon       = pclGeoCoordinates->get_lon_str_google_maps();
                qstrElevation = pclGeoCoordinates->get_elevation_str      (qstrTmp);
                delete (pclGeoCoordinates);
            }
            break;

        default:
            {
                qstrLat       = "N/A";
                qstrLon       = "N/A";
                qstrElevation = "N/A";
                bRetValue     = false;
            }
            break;
    }

    return (bRetValue);
}


bool Job::getTXMElectrodePos (const unsigned int uiElectrodeName, long& lLat, long& lLon, long& lElevation)
{
    // declaration of variables
    bool bRetValue = true;

    switch (uiElectrodeName)
    {
        case C_TXM_ELECTRODE_CENTER:
            {
                lLat       = this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_CENTER];
                lLon       = this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_CENTER];
                lElevation = this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER];
            }
            break;

        case C_TXM_ELECTRODE_E1:
            {
                lLat       = this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E1];
                lLon       = this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E1];
                lElevation = this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E1];
            }
            break;

        case C_TXM_ELECTRODE_E2:
            {
                lLat       = this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E2];
                lLon       = this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E2];
                lElevation = this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E2];
            }
            break;

        case C_TXM_ELECTRODE_E3:
            {
                lLat       = this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E3];
                lLon       = this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E3];
                lElevation = this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E3];
            }
            break;

        default:
            {
                lLat       = 0;
                lLon       = 0;
                lElevation = 0;
                bRetValue  = false;
            }
            break;
    }

    return (bRetValue);
}


bool Job::setTXMElectrodePos(const unsigned int uiElectrodeName, long& lLat, long& lLon, long& lElevation)
{
    // declaration of variables
    bool bRetValue = true;

    switch (uiElectrodeName)
    {
        case C_TXM_ELECTRODE_CENTER:
            {
                this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_CENTER] = lLat;
                this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_CENTER] = lLon;
                this->qvecTXMElecPosElev [C_TXM_ELECTRODE_CENTER] = lElevation;
            }
            break;

        case C_TXM_ELECTRODE_E1:
            {
                this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E1] = lLat;
                this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E1] = lLon;
                this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E1] = lElevation;
            }
            break;

        case C_TXM_ELECTRODE_E2:
            {
                this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E2] = lLat;
                this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E2] = lLon;
                this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E2] = lElevation;
            }
            break;

        case C_TXM_ELECTRODE_E3:
            {
                this->qvecTXMElecPosLat  [C_TXM_ELECTRODE_E3] = lLat;
                this->qvecTXMElecPosLon  [C_TXM_ELECTRODE_E3] = lLon;
                this->qvecTXMElecPosElev [C_TXM_ELECTRODE_E3] = lElevation;
            }
            break;

        default:
            {
                bRetValue  = false;
            }
            break;
    }

    return (bRetValue);
}


bool Job::getLocked(void)
{
    return (this->bLocked);
}

bool Job::setLocked (const bool bIsLocked)
{
    this->bLocked = bIsLocked;
    return (true);
}

unsigned int Job::getNumChannels(void)
{
    return (this->mJobChannelConfigVector.size());
}




