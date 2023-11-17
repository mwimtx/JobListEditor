#ifndef _SENSORCHANNELCONFIG_H_
#define _SENSORCHANNELCONFIG_H_

#include "hwdatabase.h"
#include "default_values.h"
// #include "job.h"

#include <QSharedPointer>
#include <QStringList>
#include <QString>
#include <vector>
#include <string>

// <sensor_type>EFP06</sensor_type Kommt aus Folie 2 – “Type” – gültige Werte siehe Popup Folie 3 – Hard kodiert

class Job;

class SensorChannelConfig 
{
public:

    typedef QSharedPointer<SensorChannelConfig> Spt;

    static Spt createFromHwDb ( HardwareConfig::Spt );

    typedef std::vector<Job*> JobPointerVector;
    static Spt createFromJobList  ( const JobPointerVector&  );

    QString getSelectedInputAtChannel     ( const Types::Index& ) const; // throws std::out_of_range
    QString getChannelTypeInputAtChannel  ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorTypeInputAtChannel   ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorConfigInputAtChannel ( const Types::Index& ) const; // throws std::out_of_range
    QString getSensorSerialInputAtChannel ( const Types::Index& ) const; // throws std::out_of_range

    QString getPosX1AtChannel( const Types::Index& ) const;  // throws std::out_of_range
    QString getPosY1AtChannel( const Types::Index& ) const;  // throws std::out_of_range
    QString getPosZ1AtChannel( const Types::Index& ) const;  // throws std::out_of_range
    QString getPosX2AtChannel( const Types::Index& ) const;  // throws std::out_of_range
    QString getPosY2AtChannel( const Types::Index& ) const;  // throws std::out_of_range
    QString getPosZ2AtChannel( const Types::Index& ) const;  // throws std::out_of_range


    void setHWSelectedInputAtChannel     ( const Types::Index& , const QString& );  // throws std::out_of_range
    void setHWChannelTypeInputAtChannel  ( const Types::Index& , const QString& );  // throws std::out_of_range
    void setHWSensorTypeInputAtChannel   ( const Types::Index& , const QString& );  // throws std::out_of_range
    void setHWSensorConfigInputAtChannel ( const Types::Index& , const QString& );  // throws std::out_of_range
    void setHWSensorSerialInputAtChannel ( const Types::Index& , const QString& );  // throws std::out_of_range

    void setPosX1AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range
    void setPosY1AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range
    void setPosZ1AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range
    void setPosX2AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range
    void setPosY2AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range
    void setPosZ2AtChannel( const Types::Index&  , const QString& );  // throws std::out_of_range

    
    // debug stuff...
    std::string toStdString() const;

private:
    
    SensorChannelConfig();

    class SingleSensorConfig 
    {
    private:
        QString mChannelType; // aka channel name, ExEy...
        QString mSensorType;
        QString mConfig;
        QString mSerial;

        QString mPosX1;
        QString mPosY1;
        QString mPosZ1;
        QString mPosX2;
        QString mPosY2;
        QString mPosZ2;

        const DefaultValues::VectorOfQStrings& mAllowedSensorTypes;
        const DefaultValues::VectorOfQStrings& mAllowedChannelTypes;

        bool foundQStringInVector( const DefaultValues::VectorOfQStrings& , const QString& ) const;

    public:

        SingleSensorConfig();

        typedef QSharedPointer<SingleSensorConfig> Spt;

        std::string toStdString() const;

        inline QString getHWChannelType  () const { return mChannelType;} // aka channel name, ExEy...
        inline QString getHWSensorType   () const { return mSensorType; }
        inline QString getHWSensorConfig () const { return mConfig;     }
        inline QString getHWSensorSerial () const { return mSerial;     }

        SingleSensorConfig& setHWChannelType  ( const QString& ); // aka channel name, ExEy...
        SingleSensorConfig& setHWSensorType   ( const QString& ); // sensor hardware...
        SingleSensorConfig& setHWSensorConfig ( const QString& ); // sensor config...
        SingleSensorConfig& setHWSensorSerial ( const QString& ); // sensor serial number

        inline QString getPosX1() const { return mPosX1; }
        inline QString getPosY1() const { return mPosY1; }
        inline QString getPosZ1() const { return mPosZ1; }
        inline QString getPosX2() const { return mPosX2; }
        inline QString getPosY2() const { return mPosY2; }
        inline QString getPosZ2() const { return mPosZ2; }

        SingleSensorConfig& setPosX1( const QString& );
        SingleSensorConfig& setPosY1( const QString& );
        SingleSensorConfig& setPosZ1( const QString& );
        SingleSensorConfig& setPosX2( const QString& );
        SingleSensorConfig& setPosY2( const QString& );
        SingleSensorConfig& setPosZ2( const QString& );
        
    };

    class SensorAtInput
    {
    public:
        enum SelectedInput {
            InputZero,
            InputOne,
            Undefined,
            Off
        };

        SensorAtInput() :
            mSelectedInput( Undefined ),
            mSensorAtInputZero( SingleSensorConfig::Spt() ),
            mSensorAtInputOne( SingleSensorConfig::Spt() )
            {}

        inline bool haveDefinedInput() const { return not ( mSelectedInput == Undefined) ; }

        // returns an empty SingleSensorConfig::Spt() if mSelectedInput == Undefined
        SingleSensorConfig::Spt getSelectedSensorConfigData() const ;

        // precondition : haveDefinedInput() ==  true
        // returns a SingleSensorConfig::Spt for the given input
        SingleSensorConfig::Spt getOrCreateSelectedSensorConfigData(); // throws runtime_error

        inline SingleSensorConfig::Spt getSensorAtInputZero() const { return mSensorAtInputZero; }
        inline SingleSensorConfig::Spt getSensorAtInputOne()  const { return mSensorAtInputOne; }

        void setHWChannelTypeInputAsQString ( const QString& );
        void switchSelectedInputTo( const SelectedInput& );
        void setConfigForSelectedInput( SingleSensorConfig::Spt );
        void setConfigForUnselectedInput( SingleSensorConfig::Spt );
        SelectedInput getSelectedInput() const { return mSelectedInput; }

    private:

        SelectedInput mSelectedInput;
        SingleSensorConfig::Spt mSensorAtInputZero;
        SingleSensorConfig::Spt mSensorAtInputOne;


    };

    std::string selectedInput2StdString( SensorAtInput::SelectedInput ) const;

    typedef std::vector<SensorAtInput> SensorAtInputVector;
    SensorAtInputVector mSensorAtInputVector;
};

#endif /* _SENSORCHANNELCONFIG_H_ */
