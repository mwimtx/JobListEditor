#ifndef _TOOLS_TYPES_H_
#define _TOOLS_TYPES_H_

#include <QTime>
#include <QDateTime>
#include <stdexcept>
#include <QSettings>
#include <QString>
#include <QStringList>


// constants and defines

/**
 * These constants define the names of the entries inside the registry (QSettings) class.
 */
#define C_SETTINGS_GROUP_MAINWINDOW             "MainWindow"
#define C_SETTINGS_LAST_JOBLIST_DIR             "LastJoblistDir"
#define C_SETTINGS_LAST_ADU07CHANNELCONFIG_DIR  "LastADU07ChannelConfigDir"
#define C_SETTINGS_GROUP_IMPORT_AUTOGAIN        "ImportAutoGainAutoOffset"

/**
 * If beeing defined all Debug output will be active.
 */
//#define DEBUG_OUTPUT 1


/**
 * This is a global QSettings object for the compltete application. It is used to store
 * all settings, like last paths, ... .
 */
extern QSettings* pclSettings;


namespace Types {
    typedef QTime Time;
    typedef QDate Date;
    // typedef qint64 TimeInterval; // this is meant to be an interval in seconds
    typedef quint64 Frequency;
    // typedef unsigned int Index;


    /**
     * This enum defines the single start modes that are defined inside
     * the "Start Mode" combo box of the "Shift Start Time" dialog.
     */
    class StartModes
    {
        public:
            static const unsigned int Adjusted      = 0;
            static const unsigned int AdjustedToday = 1;
            static const unsigned int Relative      = 2;
            static const unsigned int Grid          = 3;
            static const unsigned int Invalid       = 255;
    };
    const QStringList qstrlStartModeStrings = QStringList() << "adjusted" << "adjusted" << "relative" << "grid";

    /**
     * This enum defines the single units for the cyclic job settings.
     */
    class CyclicModeUnits
    {
        public:
            static const unsigned int Seconds  = 0;
            static const unsigned int Minutes  = 1;
            static const unsigned int Hours    = 2;
            static const unsigned int Days     = 3;
            static const unsigned int Invalid  = 4;
    };
    const QStringList qstrlCyclicModeUnitsStrings = QStringList() << "seconds" << "minutes" << "hours" << "days";

    /**
     * This enum defines the single settings for the storage mode.
     */
    class StorageModes
    {
        public:
            static const unsigned int WriteToInternal = 0;
            static const unsigned int WriteToUSB      = 1;
    };
    const QStringList qstrlStorageModeStrings = QStringList() << "USB_TO_DEFAULT" << "USB_TO_USB";
    const QStringList qstrlTargetDirStrings   = QStringList() << ""               << "/mtdata/usb/data1";

    /**
     * This enum defines the single settings for the AdaptSensorType mode.
     */
    class AdaptSensorTypes
    {
        public:
            static const unsigned int None    = 0;
            static const unsigned int ESeries = 1;
            static const unsigned int All     = 2;
    };
    const QStringList qstrlAdaptSensorTypesStrings = QStringList() << "NONE" << "E_SERIES" << "ALL";


    class Index
    {
    public:

        typedef unsigned int ValueType;

        const unsigned int aruiParallelChannels [10] = {5, 6, 7, 8, 9, 0, 1, 2, 3, 4};

        // this is used for a converstion from the integer family, no "explicit" here
        Index( const ValueType& value , bool isValid = true ) :
            mValue( value ),
            mIsValid( isValid )
            {}

        Index() :
            mValue( 0 ),
            mIsValid( 0 )
            {}

        inline ValueType get() const {
            checkIsValid();
            return mValue;
        }


        inline ValueType getParallelChannel() const
        {
            return (this->aruiParallelChannels[this->mValue]);
        }


        inline bool isValid() const { return mIsValid; }

        Index& setInValid() { mIsValid = false ; return *this; }

        inline bool operator == ( const Index& other ) { // throw std::runtime_error
            checkIsValid();
            other.checkIsValid();
            return mValue == other.mValue;
        }

        inline bool operator < ( const Index& other ) { // throw std::runtime_error
            checkIsValid();
            other.checkIsValid();
            return mValue < other.mValue;
        }

        inline Index& operator = ( const Index& other ) {
            mValue = other.mValue;
            mIsValid = other.mIsValid;
            return *this;
        }

        // prefix
        inline Index& operator++ () // throw std::runtime_error
            { 
                checkIsValid(); 
                ++mValue; 
                return *this;
            }
        // postfix
        inline Index operator++ ( int ) // throw std::runtime_error
            { 
                checkIsValid(); 
                Index unmoidifiedObject = *this;
                this -> operator++();
                return unmoidifiedObject;
            }

        

    private:

        inline void checkIsValid() const { if ( not isValid() ) throw std::runtime_error( "attempt to access invalid index" ); }             


        ValueType mValue;
        bool      mIsValid;

    };

    typedef unsigned long long int ObjectId;

    class TimeInterval 
    {
    public:
        // TimeInterval( qint64 seconds ) : // hu? better only provide a default ctor, and then add values...
        //     mSeconds( seconds ) {}

        typedef qint64 Seconds;

        TimeInterval() : mSeconds( 0 ) {}

        TimeInterval( const Seconds& secs ) :
            mSeconds( secs ) {} 
        
        inline Seconds asSeconds() const { return mSeconds; }

        inline TimeInterval& setSeconds( const Seconds& seconds ) { mSeconds = seconds; return *this; }
        
        inline TimeInterval& addHours   ( const unsigned& hours )   { mSeconds += hours * 3600 ; return *this ; }
        inline TimeInterval& addMinutes ( const unsigned& minutes ) { mSeconds += minutes * 60 ; return *this ; }
        inline TimeInterval& addSeconds ( const Seconds&  seconds ) { mSeconds += seconds; return *this ; }
        // factory, if we ge some more of these, we'll donate the Types namespace a compilation unit of its own.
        static TimeInterval parseIntervalString( const QString& intervalString ); // throws std::runtime_error

        inline unsigned getHoursRemainder   () const { return mSeconds / 3600; }
        inline unsigned getMinutesRemainder () const { return ( mSeconds - getHoursRemainder() * 3600 ) / 60 ; }
        inline unsigned getSecondsRemainder () const { return mSeconds % 60; }

        bool operator == ( const TimeInterval& other ) const { return mSeconds == other.mSeconds; }
        bool operator <  ( const TimeInterval& other ) const { return mSeconds <  other.mSeconds; }

    private:

        Seconds mSeconds;
       
    };


}


#endif /* _TOOLS_TYPES_H__ */
