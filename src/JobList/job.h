#ifndef _JOB_H_
#define _JOB_H_

#include "tools_types.h"
class JobChannelConfig;

#include "jobchannelconfig.h"

#include <QString>
#include <QDomDocument>
#include <QTime>
#include <QSharedPointer>
#include <QStringList>

#include <vector>

#include "controldata.h"

class DataModel;

class Job 
{
public:

    typedef QSharedPointer<Job> Spt;
    // Q: create a real factory here..? setJobDirectory ... hmm... 
    // i think, we should use a setDatamodel method..
    static Job::Spt create( QDomElement );
    static Job::Spt createFromTemplate( ); // FIXME: hardcode the template here?! dunno
    static Job::Spt deepCopy( Job::Spt );

    inline void setDataModel( DataModel* dataModel ) { mDataModel = dataModel; }

    unsigned int getId() const { return mId; }
    void setId( unsigned int newId ) { mId = newId; }

    void setJobDirectory( const QString&  );
    void loadAndParseFile();
    void saveFile();

    QString getJobFileName() const;
    inline Job&    setJobFileName( const QString& jobFileName ) { mJobFileName = jobFileName; return *this; }
    inline Job&    clearFileName() { mJobFileName.clear(); return *this; }
    inline QString getJobFileNameAsReadFromJoblist() const { return mJobFileName; }
    void updateJobFileName();

    // helper: genetate a filename accoring to job config. Does not change anything! 
    QString generateFileNameForJobSubDir( const QString& ) const;
    inline void commitChanges() { updateDocument(); }

    Types::Frequency     getFrequency(QString& qstrDigFilType, double& dDigFilDecRate) const;
    Types::Time          getStartTime() const;
    Types::Date          getStartDate() const;
    Types::Time          getStopTime()  const;
    Types::Date          getStopDate()  const;
    Types::TimeInterval  getDuration()  const; 
    time_t               getStartDateTimeUnix () const;

    ControlData::MeasurementMode getMeasurementMode() const;
    void setMeasurementMode( ControlData::MeasurementMode );

    /**
     * getter methodes for all TXM parameters.
     */
    QString getTXMNumStacks    ();    ///< number of stacks (dipole switch cycles)
    QString getTXMDipTXTime    ();    ///< dipole TX time (time, one dipole is transmitted)
    QString getTXMNumDipoles   ();    ///< number of dipoles per cycle
    QString getTXMBaseFreq     ();    ///< base frequency of output signal waveform
    QString getTXMIterations   ();    ///< number of iterations (periodes) of the output signal waveform
    QString getTXMMaxAmpl      ();    ///< maximum amplitude of output current
    QString getTXMWaveformName ();    ///< waveform name of output signal waveform
    QVector<double> getTXMPolarisations ();    ///< waveform name of output signal waveform

    /**
     * setter methodes for all TXM parameters.
     */
    bool setTXMNumStacks     (const QString& qstrValue);    ///< number of stacks (dipole switch cycles)
    bool setTXMDipTXTime     (const QString& qstrValue);    ///< dipole TX time (time, one dipole is transmitted)
    bool setTXMNumDipoles    (const QString& qstrValue);    ///< number of dipoles per cycle
    bool setTXMBaseFreq      (const QString& qstrValue);    ///< base frequency of output signal waveform
    bool setTXMIterations    (const QString& qstrValue);    ///< number of iterations (periodes) of the output signal waveform
    bool setTXMMaxAmpl       (const QString& qstrValue);    ///< maximum amplitude of output current
    bool setTXMWaveformName  (const QString& qstrValue);    ///< waveform name of output signal waveform
    bool setTXMPolarisations (const QVector<double> qvecValue);    ///< waveform name of output signal waveform

    /**
     * getter methodes for the cyclic job parameters.
     */
    QString getCyclicEvent    (void);     ///< is a cyclic event (0/1)
    QString getCyclicDuration (void);     ///< duration (number of "units")
    QString getCyclicUnit     (void);     ///< unit (hours/days/...)

    /**
     * setter methodes for the cyclic job parameters.
     */
    bool setCyclicEvent    (const QString& qstrEvent);      ///< is a cyclic event (0/1)
    bool setCyclicDuration (const QString& qstrDuration);   ///< duration (number of "units")
    bool setCyclicUnit     (const QString& qstrUnit);       ///< unit (hours/days/...)

    /**
     * This function is used to update the cyclic job parameters
     * inside the output XML job file.
     */
    bool updateCyclicJobParameters (void);

    /**
     * setter / getter methode for reading / writing electrode position
     * values.
     */
    bool getTXMElectrodePosStr (const unsigned int uiElectrodeName, QString& qstrLat, QString& qstrLon, QString& qstrElevation);
    bool getTXMElectrodePos    (const unsigned int uiElectrodeName, long&    lLat,    long&    lLon,    long&    lElevation);
    bool setTXMElectrodePos    (const unsigned int uiElectrodeName, long&    lLat,    long&    lLon,    long&    lElevation);

    /**
     * setter / gtter methode for "Locked" flag that shields this jobs from
     * beeing updated, e.g. with AutoGain / AutoOffset values.
     */
    bool getLocked (void);
    bool setLocked (const bool bIsLocked);

    bool isInCSAMTMode() const { return mMeasurementMode == ControlData::CSAMTMeasurementMode; }

    Job& setStartDateTime( const QDateTime & );
    Job& setDuration( const Types::TimeInterval& );
    Job& setFrequency( const Types::Frequency& );

    // this is a temporary id for the use within this software, does have *nothing* to do with job id!
    inline Types::ObjectId getJobObjectId() const { return reinterpret_cast<Types::ObjectId>( this ); }

    // convenience methods
    void setEnableBoardChannel ( unsigned  , bool ); // throws std::out_of_range
    void setExtGainForChannel    ( const Types::Index& , QString value ); // throws std::out_of_range
    void setChopperForChannel    ( const Types::Index& , QString value ); // throws std::out_of_range
    void setRfFilterForChannel   ( const Types::Index& , QString value ); // throws std::out_of_range
    void setGain1ForChannel      ( const Types::Index& , QString value ); // throws std::out_of_range
    void setDacForChannel        ( const Types::Index& , QString value ); // throws std::out_of_range
    void setLpHpFilterForChannel ( const Types::Index& , QString value ); // throws std::out_of_range
    void setGain2ForChannel      ( const Types::Index& , QString value ); // throws std::out_of_range

    bool getIsChannelEnabled ( const Types::Index& ) const; // throws  std::out_of_range

    QString getExtGainForChannel    ( const Types::Index& ); // throws  std::out_of_range
    QString getChopperForChannel    ( const Types::Index& ); // throws  std::out_of_range
    QString getRfFilterForChannel   ( const Types::Index& ); // throws  std::out_of_range
    QString getGain1ForChannel      ( const Types::Index& ); // throws  std::out_of_range
    QString getDacForChannel        ( const Types::Index& ); // throws  std::out_of_range
    QString getLpHpFilterForChannel ( const Types::Index& ); // throws  std::out_of_range
    QString getGain2ForChannel      ( const Types::Index& ); // throws  std::out_of_range

    // FIXME: make this private and offer friendship where needed!
    JobChannelConfig* getJobChannelConfigForChannel( unsigned ) const; // throws std::out_of_range
    unsigned int getNumChannels (void);

    // positions stuff
    QString getPosX1ForChannel( const Types::Index& ) const;
    QString getPosY1ForChannel( const Types::Index& ) const;
    QString getPosZ1ForChannel( const Types::Index& ) const;
    QString getPosX2ForChannel( const Types::Index& ) const;
    QString getPosY2ForChannel( const Types::Index& ) const;
    QString getPosZ2ForChannel( const Types::Index& ) const;

    void setPosX1ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range
    void setPosY1ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range
    void setPosZ1ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range
    void setPosX2ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range
    void setPosY2ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range
    void setPosZ2ForChannel( const Types::Index& , QString value ) ; // throws  std::out_of_range


    // comments stuff
    QString getLineNumber() const { return mLineNumber ; }
    QString getRunNumber() const { return mRunNumber ; }
    QString getClient() const { return mClient ; }
    QString getOperator() const {  return mOperator; }
    QString getContractor() const { return mContractor ; }
    QString getArea() const { return mArea ; }
    QString getSurveyId() const { return mSurveyId ; }
    QString getSiteId() const { return mSiteId; }
    QString getWeather() const { return mWeather ; }
    QString getComment() const { return mComment ; }
    QString getSiteName() const { return mSiteName ; }
    QString getSiteNameRR() const { return mSiteNameRR ; }
    QString getSiteNameEMAP() const { return mSiteNameEMAP ; }
    QString getBaseFrequency() const { return mBaseFrequency ; }
    QString getWaveFormName() const { return mTXMWaveformName ; }
    QString getProcessings() const { return mProcessings ; }

    void setLineNumber( const QString& value ) {  mLineNumber = value.trimmed(); }
    void setRunNumber( const QString& value ) {  mRunNumber = value.trimmed(); }
    void setClient( const QString& value ) {  mClient = value.trimmed(); }
    void setOperator( const QString& value ) {  mOperator = value.trimmed(); }
    void setContractor( const QString& value ) {  mContractor = value.trimmed(); }
    void setArea( const QString& value ) {  mArea = value.trimmed(); }
    void setSurveyId( const QString& value ) {  mSurveyId = value.trimmed(); }
    void setSiteId( const QString& value )  {  mSiteId = value.trimmed(); }
    void setWeather( const QString& value ) {  mWeather = value.trimmed(); }
    void setComment( const QString& value ) {  mComment = value.trimmed(); }
    void setSiteName( const QString& value ) {  mSiteName = value.trimmed(); }
    void setSiteNameRR( const QString& value ) {  mSiteNameRR = value.trimmed(); }
    void setSiteNameEMAP( const QString& value ) {  mSiteNameEMAP = value.trimmed(); }
    void setBaseFrequency( const QString& value ) {  mBaseFrequency = value.trimmed(); }
    void setWaveFormName( const QString& value ) {  mTXMWaveformName = value.trimmed(); }
    void setProcessings( const QString& value ) {  mProcessings = value.trimmed(); }
    
private:

    QDomDocument mJobDocument;

    QDomElement mStartTimeElement;
    QDomElement mStartDateElement;    
    
    QDomElement mStopDateElement;    
    QDomElement mStopTimeElement; // this is computed when storing, defined as starttime + duration

    QDomElement mSampleFreqElement;

    explicit Job( );

    // meta stuff
    DataModel* mDataModel;
    bool mUseGeneratedJobFileName;

    // job data 
    unsigned int mId;
    QString mJobFileName;
    QString mJobRootDirectory;
    // the real values are stored here, the dom elements above are only refernces to the document and are updated on writing the file!
    Types::TimeInterval mDurationSec; 
    Types::Frequency    mSampleFreq;
    ControlData::MeasurementMode mMeasurementMode;

    QString mTXMNumStacks;
    QString mTXMDipTXTime;
    QString mTXMNumDipoles;
    QString mTXMBaseFreq;
    QString mTXMIterations;
    QString mTXMWaveformName;
    QString mTXMMaxAmpl;
    QVector<double> mTXMPolarisations;

    // FIXME: merge this into a QDateTime!
    QTime mStartTime; // updated in xml
    QTime mStopTime;  // updated in xml
    QDate mStartDate; // updated in xml
    QDate mStopDate;  // updated in xml

    // Cyclic job options
    QString mCyclicEvent;
    QString mCyclicDuration;
    QString mCyclicUnit;

    // comments stuff
    QString mLineNumber;
    QString mRunNumber;
    QString mOperator;
    QString mClient;
    QString mContractor;
    QString mArea;
    QString mSurveyId;
    QString mSiteId;
    QString mWeather;
    QString mSiteName;
    QString mSiteNameRR;
    QString mSiteNameEMAP;
    QString mComment;
    QString mBaseFrequency;
    QString mProcessings;

    // DigitalFilter Stuff
    QString mDigFilType;
    double  mDigFilDecrate;

    /**
     * These parameters contains the TXM Electrode positions.
     */
    QVector<int> qvecTXMElecPosLat;    ///< vector contains latitude values
    QVector<int> qvecTXMElecPosLon;    ///< vector contains longitude values
    QVector<int> qvecTXMElecPosElev;   ///< vector contains elevation values

    typedef std::vector<JobChannelConfig::Spt> JobChannelConfigVector;
    JobChannelConfigVector mJobChannelConfigVector;

    /**
     * This attribute is set to lock the job from automatic updates, such as
     * AutoGain / AutoOffset values.
     */
    bool bLocked;

    void parseXML(); // parseGeneric, prepareJobChannelConfigVector, parseChannelData

    // this makes sure that the container is filled with reasonable default constructed values.
    void prepareJobChannelConfigVector();

    void parseGeneric();
    void parseChannelData();
    void parseComments();

    void calculateData();
    void updateDocument();
    void updateChannelConfigTag();
    // void updateDetectedHardwareTag();
    void updateMeasTypeTag();
    void updateCommentsTag();
    void updateCalibrationTags();



    /**
     * \brief This function will write back all the TXM stuff to the XML file.
     *
     * This function will write back all the TXM-22 sepcific stuff to the XML job
     * file. This is done by reading the according values from the private data
     * structure.
     * Special care has to be taken on the "Iterations" node. It is computed from
     * the selected base frequency, the number of dipoles and the dipole tx time
     * together with the number of stacks.
     *
     * \author MWI
     * \date 2013-10-22
     */
    void updateTXMTags();

    /**
     * \brief This function will go to the ATSWriter node.
     *
     * This function will try to go to the ATSWriter node. It will additionally
     * check for the DigitalFilter node, if this is a DigitalFilter job.
     *
     * @param[in] QDomDocument& clDOMDoc = reference to DOMDocument
     * @param[in] QSharedPointer<Tools::DomElementWalker>& pclWalker = reference to pointer to DOM Element Walker
     * @param[in] QString& qstrDigFilType = reference to string, the filter type shall be stored in
     * @param[in] double& dDecRate = reference to double, the decimation rate shall be stored in
     * @param[out] bool = true: OK / false: NOK
     *
     * \author MWI
     * \date 2014-01-23
     */
    bool goToATSWriterNode (QDomDocument& clDOMDoc, QSharedPointer<Tools::DomElementWalker>& pclWalker, QString& qstrDigFilType, double& dDecRate);

    /**
     * \brief This function returns the correct value for the input divider according to sensor type.
     *
     * -
     *
     * \author MWI
     * \date 2014-07-21
     */
    const QString getInputDivForSensor (const QString qstrSensorType);


    // copy ctor helper: make a deep copy of xml document
    void cloneDocument();

};


#endif /* _JOB_H_ */
