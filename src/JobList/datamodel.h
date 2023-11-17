#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QString>
#include <QDomDocument>
#include <QSharedPointer>
#include <QAbstractTableModel>
#include <vector>
#include <QTime>
#include <QDateTime>
#include <QFileInfo>
#include <QItemSelectionModel>

class JobListModel;

#include "default_values.h"
#include "tools_types.h"
#include "hwdatabase.h"
#include "sensorchannelconfig.h"
#include "job.h"
#include "controldata.h"

struct UpdateChannelStatusFOB;
struct UpdateTxmParametersStatusFOB;

/**
 * Electrode names for TXM.
 */
#define C_TXM_ELECTRODE_CENTER 0
#define C_TXM_ELECTRODE_E1     1
#define C_TXM_ELECTRODE_E2     2
#define C_TXM_ELECTRODE_E3     3

/**
 * These constants define the single modes of the Joblist
 * (either CSMT or MT).
 * The flag is stored inside the ADUConf XML file.
 */
#define C_JOBLIST_MODE_MT_STRING   "MT"     ///< standard (A)MT recording mode - stand-alone ADU-07e
#define C_JOBLIST_MODE_CSMT_STRING "CS"     ///< CS(A)MT recording mode - ADU-07e + TXM-22
#define C_JOBLIST_MODE_INV_STRING  "INV"    ///< invalid mode


#define C_JOBLIST_MODE_MT_GUI_STRING   "(A)MT"      ///< standard (A)MT recording mode - stand-alone ADU-07e
#define C_JOBLIST_MODE_CSMT_GUI_STRING "CS(A)MT"    ///< CS(A)MT recording mode - ADU-07e + TXM-22

/**
 * enum contains the different recording modes of the joblist.
 */
typedef enum
{
    C_JOBLIST_MODE_MT   = 0,    ///< standard (A)MT recording mode - stand-alone ADU-07e
    C_JOBLIST_MODE_CSMT = 1,    ///< CS(A)MT recording mode - ADU-07e + TXM-22
    C_JOBLIST_MODE_INV  = 255   ///< invalid / unkown mode
} eJoblistModes_t;


class DataModel : public QObject
{

    Q_OBJECT

public:

    ////////////////////////////////////////////////////////////////////////////////
    //
    // general stuff.

    DataModel();
    ~DataModel();
    void loadAndParseJobListFile( const QString& ); // throws std::runtime_error

    void reset();

    void loadAndParseHwData( const QDir& );

    inline bool getIsReady() const { return mIsReady; }
    inline void setJobDirectory( const QString&  directory ) { mJobRootDirectory = directory; } 
    QString getJobDirectory() const { return mJobRootDirectory; } // this is where the jobfiles are stored.

    const QFileInfo getJobListFileInfo() const { return mJobListFileInfo; }
    void setJobListFileInfo( const QFileInfo& fileInfo ) { mJobListFileInfo = fileInfo; }

    // this defines the directory expression that will be written into the joblist (this is not necessarily as mJobRootDirectory).
    void    setJobSubdir( const QString& jobSubDir ) { mJobSubDir = jobSubDir; }
    QString getJobSubdir() const      { return mJobSubDir;            }

    void    setJobListTeamplateName( const QString& jobListTeamplateName ) { mJobListTeamplateName = jobListTeamplateName; }
    QString getJobListTeamplateName() const { return mJobListTeamplateName; }

    void saveFiles(); // FIXME: output directory hardcoded till now

    QAbstractTableModel* getJobListModel(); // throws std::runtime_error
    QItemSelectionModel* getSelectionModel();

    inline bool haveSelectedJob() { return not getSelectedJob().isNull(); }

    unsigned int jobCount() const;

    // BEGIN DEBUG CHEATS SECTION
    // FIXME: we need a selection handling object ( a factory or so... ) here. The following is crap.
    void setJobSelectionUsesGuiSelectionModel( bool jobSelecteionUsesGuiSelectionModel );
    void setSelectedJobIndex( const Types::Index& jobNumber );
    // BEGIN DEBUG CHEATS SECTION

    // these are needed by Qt Model Stuff...
    Types::Frequency     frequnecyOfJob   (const Types::Index&, QString& qstrDigFilType, double& dDigFilDecrate) const; // Hz , throws std::out_of_range
    // FIXME: if think, we need to make Types::Time capable of periods of time that cross the border ofthe day...
    Types::Time          startTimeOfJob   ( const Types::Index& ) const; // throws std::out_of_range
    QDateTime            startDateTimeOfJob   (const Types::Index&) const; // throws std::out_of_range
    Types::Time          stopTimeOfJob    ( const Types::Index& ) const; // throws std::out_of_range
    Types::TimeInterval  durationOfJob    ( const Types::Index& ) const; // throws std::out_of_range
    Types::ObjectId      getObjectIdOfJob ( const Types::Index& ) const; // throws std::out_of_range

    Types::Time getStartTimeOfJobLocalTime (const Types::Index& clIndex) const;
    QDateTime getStartDateTimeOfJobLocalTime (const Types::Index& clIndex) const;
    bool  setStartTimeOfJobLocalTime (const Types::Index& clIndex, const Types::Time& clNewStartTime);

    void setDurationOfJobTo( const Types::Index& index, const Types::TimeInterval&); // throws std::out_of_range

    /**
     * This function is used to set a new start time for the job.
     */
    void setStartTimeOfJobTo(const Types::Index&, const Types::Time& cNewStartTime);

    QDateTime jobListStartDateTime() const;

    void addJobBeforeJobAtIndex ( const Types::Index& , Job::Spt );
    void addJobAfterJobAtIndex  ( const Types::Index& , Job::Spt );
    void deleteJobAtIndex       ( const Types::Index& );

    // helper: tries to find a job with a similar frequency
    Job::Spt cloneJobWithFreqNear( unsigned int );

    // FIXME: need this for sensors config, but should we hand out "raw" job data at all?
    Job::Spt getJobAt( const Types::Index& ) const; // throws std::out_of_range

    // FIXME: there must be a more elegant way...
    inline const HardwareConfig::Spt getHwConfig() const { return mHardwareConfig; }

    inline const SensorChannelConfig::Spt getSensorChannelConfig() const { return mConfiguredSensorCfg; }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // channel / board / sensor stuff ( hardware ) 

    bool    getSensorHWEnabledInputAtChannel     ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorHWSelectedInputAtChannel    ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorHWChannelTypeInputAtChannel ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorHWSensorTypeInputAtChannel  ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorHWConfigInputAtChannel      ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorHWSerialInputAtChannel      ( const Types::Index& ) const; // throws std::out_of_range

    void setSensorHWSelectedInputAtChannel    ( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWChannelTypeInputAtChannel ( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWSensorTypeInputAtChannel  ( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWConfigInputAtChannel      ( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWSerialInputAtChannel      ( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error

    QString getSensorHWPosX1( const Types::Index& ) const;
    QString getSensorHWPosY1( const Types::Index& ) const;
    QString getSensorHWPosZ1( const Types::Index& ) const;
    QString getSensorHWPosX2( const Types::Index& ) const;
    QString getSensorHWPosY2( const Types::Index& ) const;
    QString getSensorHWPosZ2( const Types::Index& ) const;

    void setSensorHWPosX1( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWPosY1( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWPosZ1( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWPosX2( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWPosY2( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error
    void setSensorHWPosZ2( const Types::Index& , const QString& ) ; // throws std::out_of_range , runtime_error

    std::string sensorConfigToStdString() const;

    // get allowed values...
    QStringList allowedDiscreteValuesForGain1AtChannel ( const Types::Index& );
    QStringList allowedDiscreteValuesForGain2AtChannel ( const Types::Index& );
    QStringList allowedLpHpFilterValuesAtChannel       ( const Types::Index& );
    QStringList allowedRfFilterValuesAtChannel         ( const Types::Index& );
    QStringList allowedChopperValuesAtChannel          ( const Types::Index& );

    /**
     * \brief This function returns the allowed sampling frequency according to the jobs stored in the joblist.
     *
     * This function is used to get a list of all sampling frequencies that are
     * either supported by the installed hardware, or are configured as a job
     * inside the template (means should only be used on the template joblists).
     *
     * @param[out] const QStringList = List of all supported sampling frequencies, either supported by jobs or hardware.
     *
     * \date 2014-07-23
     * \author MWI
     */
    const QStringList getAllowedSampleFreqsFromTemplate (void);

    /**
     * \brief This function returns the Joblists start-time in LocalTime.
     *
     * It adds the actual UTC to LocalTime Offset to the start-time,
     * that is stored inside the mJobListStartDateTime attribute.
     *
     * @param[out] const QDateTime = Joblist start-time in LocalTime
     *
     * \date 2014-07-23
     * \author MWI
     */
    const QDateTime getJoblistStartTimeLocalTime (void);

    /**
     * \brief This function is used to extract all valid sample frequencies for all available boards.
     *
     * This function will call the AdbBoardInfo class to get a complete list
     * of all available Sample Frequencies of all available boards.
     *
     * @param[out] QStringList = list of all available sample frequencies
     *
     * \date 2014-01-28
     * \author MWI
     */
    QStringList allowedSampleFreqs (void);

    /**
     * \brief This function is used to check, if the joblist target hardware is a transmitter.
     *
     * Just a wrapper for the implementation inside HwDatabase class.
     *
     * @param[out] const bool = true: target hardware is transmitter (TXM) / false: target hardware is no transmitter
     *
     * \date 2014-07-17
     * \author MWI
     */
    bool isTXM (void);

    /**
     * \brief This function is used to check, if the joblist target hardware is a MF board ADU.
     *
     * Just a wrapper for the implementation inside HwDatabase class.
     *
     * @param[out] const bool = true: target hardware is MF board ADU / false: target hardware is LF/HF board ADU
     *
     * \date 2014-07-17
     * \author MWI
     */
    bool isMF (void);

    /**
     * \brief This function is used to check, if the joblist target hardware is a ADU-08e BB board ADU.
     *
     * Just a wrapper for the implementation inside HwDatabase class.
     *
     * @param[out] const bool = true: target hardware is ADU-08e BB board ADU / false: target hardware is LF/HF board ADU
     *
     * \date 2016-06-02
     * \author MWI
     */
    bool isADU08eBB (void);

    /**
     * \brief This function is used to check, if the joblist target hardware is a ADU-10e.
     *
     * Just a wrapper for the implementation inside HwDatabase class.
     *
     * @param[out] const bool = true: target hardware is ADU-10e / false: target hardware is LF/HF board ADU
     *
     * \date 2021-10-06
     * \author MWI
     */
    bool isADU09u10e (void);

    /**
     * \brief This function is used to extract all valid sample frequencies for the boards of a certain job.
     *
     * This is a an overloaded version of the allowedSampleFreqs (void) function that will
     * return the valid sample frequencies of the boards of a defined job.
     *
     * @param[in] const Types::Index& iIndex= index of job
     * @param[out] QStringList = list of all available sample frequencies
     *
     * \date 2014-01-28
     * \author MWI
     */
    QStringList allowedSampleFreqs (const Types::Index& iIndex);


    /**
     * \brief This is the setter for the "Joblist Mode" flag.
     *
     * -
     *
     * @param[in] const eJobslistMode_t eNewJoblistMode = new mode for Joblist
     * @param[out] const bool = true: OK / false: mode not changed
     *
     * \date 2014-07-17
     * \author MWI
     */
    bool setJoblistMode (const eJoblistModes_t eNewJoblistMode);

    /**
     * \brief This is the getter for the "Joblist Mode" flag.
     *
     * -
     *
     * @param[out] const eJobslistMode_t eNewJoblistMode = actual mode for Joblist
     *
     * \date 2014-07-17
     * \author MWI
     */
    eJoblistModes_t getJoblistMode (void);

    /**
     * \brief This is the getter for the "UTC to LocalTime Offset".
     *
     * -
     *
     * @param[out] const double = actual value for "UTC to LocalTime Offset"
     *
     * \date 2014-07-23
     * \author MWI
     */
    double getUTCLocalTimeOffset (void);

    /**
     * \brief This is the setter for the "UTC to LocalTime Offset".
     *
     * -
     *
     * @param[in] const double dNewUTCLocalTimeOffset = new value for "UTC to LocalTime Offset"
     * @param[out] const bool = true: OK
     *
     * \date 2014-07-23
     * \author MWI
     */
    bool setUTCLocalTimeOffset (const double dNewUTCLocalTimeOffset);


    bool        getHasDac                              ( const Types::Index& ) const ;
    QString     minExtGainAtChannel                    ( const Types::Index& );
    QString     maxExtGainAtChannel                    ( const Types::Index& );
    QString     getMinDacAtChannel                     ( const Types::Index& );
    QString     getMaxDacAtChannel                     ( const Types::Index& );
    

    // set values for the currently selected job.
    bool setExtGainForChannel    ( const Types::Index& , QString );
    bool setChopperForChannel    ( const Types::Index& , QString );
    bool setRfFilterForChannel   ( const Types::Index& , QString );
    bool setGain1ForChannel      ( const Types::Index& , QString );
    bool setDacForChannel        ( const Types::Index& , QString );
    bool setLpHpFilterForChannel ( const Types::Index& , QString );
    bool setGain2ForChannel      ( const Types::Index& , QString );

    // get values for the currently selected job
    QString getExtGainForChannel    ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getChopperForChannel    ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getRfFilterForChannel   ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getGain1ForChannel      ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getDacForChannel        ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getLpHpFilterForChannel ( const Types::Index& ); // throws std::runtime_error , std::out_of_range
    QString getGain2ForChannel      ( const Types::Index& ); // throws std::runtime_error , std::out_of_range

    // for currently selected job!
    ControlData::MeasurementMode getMeasurementMode();
    void setMeasurementMode (ControlData::MeasurementMode, const bool bUpdateAllJobs = false);

    // comments for current job
    QString getCommentsProcessings();
    QString getCommentsWeather();
    QString getCommentsWaveFormName();
    QString getCommentsSurveyId();
    QString getCommentsSiteId();    
    QString getCommentsArea();
    QString getCommentsSiteName();
    QString getCommentsSiteNameRR();
    QString getCommentsSiteNameEMAP();
    QString getCommentsComment();
    QString getCommentsContractor();
    QString getCommentsLineNumber();
    QString getCommentsBaseFrequency();
    QString getCommentsClient();
    QString getCommentsOperator();
    QString getCommentsRunNumber();

    void setCommentsProcessings( const QString& );
    void setCommentsWeather( const QString& );
    void setCommentsWaveFormName( const QString& );
    void setCommentsSurveyId( const QString& );
    void setCommentsSiteId( const QString& );
    void setCommentsArea( const QString& );
    void setCommentsComment( const QString& );
    void setCommentsSiteName( const QString& );
    void setCommentsSiteNameRR( const QString& );
    void setCommentsSiteNameEMAP( const QString& );
    void setCommentsContractor( const QString& );
    void setCommentsLineNumber( const QString& );
    void setCommentsBaseFrequency( const QString& );
    void setCommentsClient( const QString& );
    void setCommentsOperator( const QString& value );
    void setCommentsRunNumber( const QString& );

    /**
     * getter methodes for all TXM parameters.
     */
    QString getTXMNumStacks    ();    ///< number of stacks (dipole switch cycles)
    QString getTXMDipTXTime    ();    ///< dipole TX time (time, one dipole is transmitted)
    QString getTXMNumDipoles   ();    ///< number of dipoles per cycle

    /**
     * setter methodes for all TXM parameters.
     */
    bool setTXMNumStacks     (const QString& qstrValue);    ///< number of stacks (dipole switch cycles)
    bool setTXMDipTXTime     (const QString& qstrValue);    ///< dipole TX time (time, one dipole is transmitted)
    bool setTXMNumDipoles    (const QString& qstrValue);    ///< number of dipoles per cycle

    /**
     * getter methodes for the storage mode settings.
     */
    QString getTargetDirectory (void);  ///< global TargetDirectory (mount point)
    QString getStorageMode     (void);  ///< storage mode (USB_TO_USB, ...)

    /**
     * setter methodes for the storage mode settings.
     */
    bool setTargetDirectory (const QString& qstrTargetDir);   ///< global TargetDirectory (mount point)
    bool setStorageMode     (const QString& qstrStorageMode); ///< storage mode (USB_TO_USB, ...)

    /**
     * setter / getter methode for the "CleanJobTable" flag.
     */
    bool setCleanJobTables (const bool bFlag);
    bool getCleanJobTables (void);

    /**
     * setter / getter methode for the "AdaptChannelConfig" flag
     */
    bool setAdaptChannelConfig (const bool bFlag);
    bool getAdaptChannelConfig (void);

    /**
     * setter / getter methode for the "AdaptSensorType" flag
     */
    bool    setAdaptSensorType (const QString& qstrMode);
    QString getAdaptSensorType (void);

    /**
     * setter / getter methode for the "AdaptSensorSpacing" flag
     */
    bool setAdaptSensorSpacing (const bool bFlag);
    bool getAdaptSensorSpacing (void);

    /**
     * setter / getter methode for reading / writing electrode position
     * values.
     */
    bool getTXMElectrodePosStr (const unsigned int uiElectrodeName, QString& qstrLat, QString& qstrLon, QString& qstrElevation);
    bool getTXMElectrodePos    (const unsigned int uiElectrodeName, long&    lLat,    long&     lLon,   long&    lElevation);
    bool setTXMElectrodePos    (const unsigned int uiElectrodeName, long&    lLat,    long&     lLon,   long&    lElevation);

    Job::Spt getSelectedJob(); // throws std::out_of_range , std::runtime_error

    const QString getTimeFormat (void);
    bool setTimeFormat (const QString& qstrTimeFormat);

    /**
     * \brief This function is used to swap two entries of the joblist vector.
     *
     * This function will swap two entries of the joblist vector. This is necessary, e.g.
     * if one of the entries shall be moved one position up/down within the vector.
     * As parameter it simply gets the index values of the two entries to be swapped.
     *
     * @param[in] const size_t sIndex1 = Index of first item
     * @param[in] const size_t sIndex2 = Index of first item
     * @param[out] const bool = true: OK / false: NOK
     *
     * \date 2014-01-22
     * \author MWI
     */
    bool swapJobPosition (const size_t sIndex1, const size_t sIndex2);


signals:

    void sensorConfigLoaded();
    void sensorConfigChanged(); // emited when sensor config changed (includes initial load of data!)

    void jobDataLoaded();
    void jobDataChanged(); // contents of data changed, not number of records!

    void startInsertJobAtIndex( const Types::Index& );
    void endInsertJob();

    void deleteInsertJobAtIndex( const Types::Index& );
    void endDeleteJob();

public slots:
    
    void shiftStartChanged ( const QDateTime & );
    void setAdjustToMinute( bool );
    void setAdjustPause( ControlData::AdjustMode );

    // use Types::Index here?
    void enableBoardChannelForCurrentJob( unsigned, bool );

private:

    inline void setIsReady( bool isReady ) { mIsReady = isReady; }

    void parseJobList(); // throws std::runtime_error
    void parseJobListStartTime(); 

    void loadJobFiles(); // throws std::runtime_error

    void setupSensorData();

    // updated the changed parts of the xml document
    void updateDocument();
    void updateJobListXmlTag();

    void updateJobList();
    void updateJobList( const QDateTime &  );
    

    SensorInfo::Spt   getDetectedSensorHwForChannel   ( const Types::Index& ); // returns the SensorInfo or an empty Spt if the channel does not exist. throws std::out_of_range
    SensorInfo::Spt   getSensorInfoForChannel( const Types::Index& ); 

    // member below

    typedef std::vector<Job::Spt> JobVector;
    JobVector mJobList;
    QString mJobRootDirectory; // where to find the stuff on disk
    QString mJobSubDir; // what to write into joblist
    QString mJobListTeamplateName;

    JobListModel*        mJobListModel;
    QItemSelectionModel* mSelectionModel;
    

    // must this implemetation detail leak to the outside?
    QDomDocument mJobListDocument;
    QFileInfo    mJobListFileInfo;
    // QFileInfo    mFileInfo;
    
    QDomElement  mTimeFormatElement;      // see updateDocument

    /**
     * These nodes are used to store the settings for writing
     * data to internal storage or external USB device.
     */
    QString mTargetDirectory;   ///< stores target directory (mount point)
    QString mStorageMode;       ///< stores storage mode (write to external USB or internal storage)

    /**
     * This parameter defines the "CleanJobTables" node inside the
     * ADUConf XML file.
     */
    QString mCleanJobTables;

    /**
     * This parameter defines the "AdaptChannelConfig" node inside the
     * ADUConf XML file.
     */
    QString mAdaptChannelConfig;

    /**
     * This parameter defines the "AdaptSensorType" node inside the
     * ADUConf XML file.
     */
    QString mAdaptSensorType;

    /**
     * This parameter defines the "AdaptSensorSpacing" node inside the
     * ADUConf XML file.
     */
    QString mAdaptSensorSpacing;

    /**
     * This parameter is used to check, if the JobListEditor shall work in
     * MT or CSMT mode. According to the mode, the transmitter related
     * input GUI elements are shown / hidden.
     */
    eJoblistModes_t eJoblistMode;

    /**
     * This attribute contains the UTC to Localtime offset as entered by the user.
     */
    double dUTCLocalTime;

    QDomElement  mStartDateElement; // see updateDocument
    QDomElement  mStartTimeElement; // see updateDocument
    bool         mJobListStartTimeChanged;

    QDateTime    mJobListStartDateTime;

    bool mAdjustToMinute;
    ControlData::AdjustMode mAdjustMode;

    HardwareDB::Spt          mHwDb;
    // FIXME rename this so that this is a bit more speaking!
    // HardwareConfig::Spt mDetectedSensorCfg;    
    HardwareConfig::Spt mHardwareConfig;
    SensorChannelConfig::Spt mConfiguredSensorCfg;    

    friend struct UpdateChannelStatusFOB;
    friend struct UpdateTxmParametersStatusFOB;

    //Job::Spt getSelectedJob(); // throws std::out_of_range , std::runtime_error
    
    // helper, dont use them directly...
    Job::Spt getGuiSelectedJob(); // throws std::out_of_range 
    Job::Spt getManuallySelectedJob(); // throws std::out_of_range 
    // FIXME: this is not that beautiful.
    Job::Spt getSelectedJobAndThrowIfThereIsNoSelectedJob(); // throws std::out_of_range , std::runtime_error

    Types::Index mManuallySelectedJobIndex;    
    bool mJobSelecteionUsesGuiSelectionModel; // when set to true, we use the manually selected job. not so beautiful,this...
    
    bool mIsReady;

};

#endif // DATAMODEL_H
