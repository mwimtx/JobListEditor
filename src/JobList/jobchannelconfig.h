#ifndef _JOBCHANNELCONFIG_H_
#define _JOBCHANNELCONFIG_H_

struct JobChannelConfigFromDefault;

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QDomElement>

#include "sensorchannelconfig.h"

class DataModel;

class JobChannelConfig
{

    friend struct JobChannelConfigFromDefault;

public:

    ~JobChannelConfig();

    typedef QSharedPointer<JobChannelConfig> Spt;

    static Spt createFromXmlChannelTag( const DataModel* , const QDomElement& jobElement );
    static Spt createFromDefaults     ( const DataModel* ); // throw std::runtime_error

    static unsigned int parseOutputChannelId( const QDomElement& outputChannelElement );

    QString getExtGain()    const ;
    QString getHChopper()   const ;
    QString getGain1()      const ;
    QString getGain2()      const ;
    bool getHasDac()        const ;
    QString getDacVal()     const ;
    QString getDacOn()      const ;

    QString getRfFilter  () const; // there can only be at most one active filter!
    QString getLpHpFilter() const; // there can only be at most one active filter!

    QString getInputSelection() const;

    // sensor stuff...
    QString getChannelType()        const; // Ex,Ey...
    QString getSensorType()         const; // 
    QString getSensorConfig()       const;
    QString getSensorSerialNumber() const;

    // output stuff...
    inline QString getStartTime()     const { return mStartTime; }
    inline QString getStartDate()     const { return mStartDate; }
    inline QString getSampleFreq()    const { return mSampleFreq; }
    inline QString getNumSamples()    const { return mNumSamples; }
    inline QString getAtsDataFile()   const { return mAtsDataFile; }
    inline QString getTsLsb()         const { return mTsLsb; }
    inline QString getSensorCalFile() const { return mSensorCalFile; }

    QString getPosX1() const;
    QString getPosY1() const;
    QString getPosZ1() const;
    QString getPosX2() const;
    QString getPosY2() const;
    QString getPosZ2() const;



    // meta stuff...
    inline unsigned int getChannelId() const { return mId; }
    void setIsEnabled( bool isEnabled );
    
    inline bool isValid()   const { return mIsValid; }
    bool isEnabled() const;

    void setExtGain  ( const QString& );
    void setChopper  ( const QString& );
    void setGain1    ( const QString& );
    void setDac      ( const QString& );
    void setGain2    ( const QString& );
    
    void setRfFilter   ( const QString& );
    void setLpHpFilter ( const QString& );

    inline void setPosX1( const QString& value ) { mPosX1 = value.trimmed(); }
    inline void setPosY1( const QString& value ) { mPosY1 = value.trimmed(); }
    inline void setPosZ1( const QString& value ) { mPosZ1 = value.trimmed(); }
    inline void setPosX2( const QString& value ) { mPosX2 = value.trimmed(); }
    inline void setPosY2( const QString& value ) { mPosY2 = value.trimmed(); }
    inline void setPosZ2( const QString& value ) { mPosZ2 = value.trimmed(); }

    // sensor info from output section (why is this an output?)

    // please note: this is *not* a static method!
    void parseOutputChannelElement( const QDomElement& );
    
    inline bool isOutputChannelInformationParsed() const { return mOutputChannelInformationParsed; }

    void setSensorChannelConfig( SensorChannelConfig::Spt sensorChannelConfig ) { mSensorChannelConfig = sensorChannelConfig; }

private:

    JobChannelConfig( const DataModel* ); // parse from default.
    
    void parse( const QDomElement& channelElement ); // throw std::runtime_error
    void initFromDefaults();

    void parseId             ( const QDomElement& );
    void parseExtGain        ( const QDomElement& );
    void parseHChopper       ( const QDomElement& );
    void parseGain1          ( const QDomElement& );
    void parseGain2          ( const QDomElement& );
    void parseDac            ( const QDomElement& );
    void parseFilter         ( const QDomElement& );
    void parseInputSelection ( const QDomElement& );

    void parseChannelType       ( const QDomElement& ); // Ex,Ey...
    void parseSensorType        ( const QDomElement& );
    // void parseSensorConfig();
    void parseSensorSerialNumber( const QDomElement& ) ;

    void parseStartTime     ( const QDomElement& );
    void parseStartDate     ( const QDomElement& );
    void parseSampleFreq    ( const QDomElement& );
    void parseNumSamples    ( const QDomElement& );
    void parseAtsDataFile   ( const QDomElement& );
    void parseTsLsb         ( const QDomElement& );
    void parseSensorCalFile ( const QDomElement& );
    void parsePosX1         ( const QDomElement& );
    void parsePosY1         ( const QDomElement& );
    void parsePosZ1         ( const QDomElement& ); // not needed?
    void parsePosX2         ( const QDomElement& );
    void parsePosY2         ( const QDomElement& );
    void parsePosZ2         ( const QDomElement& ); // not needed?

    static unsigned int parseIdAttribute( const QDomElement& elemWithIdAttribute );

    QString mExtGain;
    QString mChopper;
    QString mGain1;
    QString mGain2;
    QString mDacVal;
    QString mDacOn;

    QString mRfFilter;
    QString mLpHpFilter;

    QString mInputSelection; // 0/1 

    QString mChannelType; // Ex,Ey...
    QString mSensorType;
    QString mSensorSerialNumber;
    QString mSensorConfig; // auto is some sort of special sensor type.. auto or sensor name.. or use a bool here?

    QString mStartTime;
    QString mStartDate;
    QString mSampleFreq;
    QString mNumSamples;
    QString mAtsDataFile;
    QString mTsLsb;
    QString mSensorCalFile;
    QString mPosX1;
    QString mPosY1;
    QString mPosZ1;
    QString mPosX2;
    QString mPosY2;
    QString mPosZ2;


    QDomElement mInputChannelElement;  // this is the rootnode of the input channel tag in xml document with the mId as channel id
    QDomElement mOutputChannelElement; // this is the rootnode of the output channel tag in xml document with the mId as channel id

    unsigned int mId;
    bool         mIsValid;    
    bool         mIsEnabled;
    bool         mIsConstrutedFromDefaults;
    bool         mOutputChannelInformationParsed;

    SensorChannelConfig::Spt mSensorChannelConfig;

    const DataModel* mData;

};

#endif /* _JOBCHANNELCONFIG_H_ */
