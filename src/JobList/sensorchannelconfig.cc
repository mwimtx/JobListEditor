#include "sensorchannelconfig.h"
#include <sstream>
#include <algorithm>
#include "job.h"

////////////////////////////////////////////////////////////////////////////////
//
// factories SensorChannelConfig 

SensorChannelConfig::Spt SensorChannelConfig::createFromJobList  ( const JobPointerVector& jobPtrVector ) 
{
     std::vector<bool> stillNeedDataForChannel( DefaultValues::MaxChannels , true );

    SensorChannelConfig::Spt result = SensorChannelConfig::Spt( new SensorChannelConfig() );

    SensorAtInputVector tmp( DefaultValues::MaxChannels , SensorAtInput() );
    result -> mSensorAtInputVector.swap( tmp );

    SensorAtInputVector& saiv( result -> mSensorAtInputVector );

    for ( JobPointerVector::const_iterator it = jobPtrVector.begin();
          not ( it == jobPtrVector.end() );
          ++it )
    {
        Job& job( (**it ));

        for( unsigned int channelNumber = 0;
             channelNumber < DefaultValues::MaxChannels;
             ++channelNumber )
        {
            JobChannelConfig* channelCfg = job.getJobChannelConfigForChannel( channelNumber );

            if ( ( not stillNeedDataForChannel.at( channelNumber ) ) or
                 ( not channelCfg -> isEnabled() ) )
                continue;

            stillNeedDataForChannel.at( channelNumber ) = false;

            #ifdef DEBUG_OUTPUT
            qDebug() << "createFromJobList : "  << " Choosing channel sensor config for channel no " << channelNumber << " from jobfile : " << job.getJobFileName();
            #endif

            SingleSensorConfig::Spt ssc = SingleSensorConfig::Spt( new SingleSensorConfig() );
            (*ssc)
                .setHWChannelType  ( channelCfg -> getChannelType() )
                .setHWSensorType   ( channelCfg -> getSensorType()  )
                .setHWSensorConfig ( channelCfg -> getSensorConfig() )
                .setHWSensorSerial ( channelCfg -> getSensorSerialNumber() )
                .setPosX1( channelCfg -> getPosX1() )
                .setPosY1( channelCfg -> getPosY1() )
                .setPosZ1( channelCfg -> getPosZ1() )
                .setPosX2( channelCfg -> getPosX2() )
                .setPosY2( channelCfg -> getPosY2() )
                .setPosZ2( channelCfg -> getPosZ2() );

            saiv
                .at( channelNumber )
                .switchSelectedInputTo( channelCfg -> getInputSelection().compare("1" ) == 0 ? 
                                        SensorAtInput::InputOne : 
                                        SensorAtInput::InputZero ); 

            saiv.at( channelNumber ).setConfigForSelectedInput( ssc );
            saiv.at( channelNumber ).setConfigForUnselectedInput( SingleSensorConfig::Spt( new SingleSensorConfig() ) );

        }
    }

    for( unsigned int channelNumber = 0;
         channelNumber < DefaultValues::MaxChannels;
         ++channelNumber ) 
    {
        if ( not stillNeedDataForChannel.at( channelNumber ) )
            continue;
        saiv
            .at( channelNumber )
            .switchSelectedInputTo( SensorAtInput::InputZero );
        
        saiv.at( channelNumber ).setConfigForSelectedInput( SingleSensorConfig::Spt( new SingleSensorConfig() ) );
        saiv.at( channelNumber ).setConfigForUnselectedInput( SingleSensorConfig::Spt( new SingleSensorConfig() ) );
    }

    return result;
}

SensorChannelConfig::Spt SensorChannelConfig::createFromHwDb ( HardwareConfig::Spt hwCfg )
{
    // does not make that much sense... but it does not harm...

    SensorChannelConfig::Spt result = SensorChannelConfig::Spt( new SensorChannelConfig() );

    for ( unsigned channel = 0; channel < DefaultValues::MaxChannels ; ++channel ) {
        result -> mSensorAtInputVector.push_back( SensorAtInput() );
        SensorInfo::Spt sensorInfo = hwCfg -> getSensorInfoForChannel( channel );
        if ( sensorInfo.isNull() )
            continue;
        SingleSensorConfig::Spt ssc0 = SingleSensorConfig::Spt( new SingleSensorConfig() );
        (*ssc0)
            .setHWSensorType( sensorInfo -> getName() );
        SingleSensorConfig::Spt ssc1 = SingleSensorConfig::Spt( new SingleSensorConfig() );
        (*ssc1)
            .setHWSensorType( sensorInfo -> getName() );

        result -> mSensorAtInputVector.rbegin() ->  switchSelectedInputTo( SensorAtInput::InputZero );
        result -> mSensorAtInputVector.rbegin() ->  setConfigForSelectedInput( ssc0 );
        result -> mSensorAtInputVector.rbegin() ->  switchSelectedInputTo( SensorAtInput::InputOne );
        result -> mSensorAtInputVector.rbegin() ->  setConfigForSelectedInput( ssc1 );
        // result -> mSensorAtInputVector.rbegin() ->  switchSelectedInputTo( SensorAtInput::Off );
        result -> mSensorAtInputVector.rbegin() ->  switchSelectedInputTo( SensorAtInput::InputZero );
        
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////
//
// class SensorChannelConfig 

SensorChannelConfig::SensorChannelConfig()
{
    
}

QString SensorChannelConfig::getSelectedInputAtChannel( const Types::Index& index ) const
{
    return QString::fromStdString( selectedInput2StdString( mSensorAtInputVector.at( index.get() ).getSelectedInput() ) ); 
}

QString SensorChannelConfig::getChannelTypeInputAtChannel( const Types::Index& index ) const
{
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getHWChannelType();
}

QString SensorChannelConfig::getSensorTypeInputAtChannel( const Types::Index& index ) const
{
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getHWSensorType();
}

QString SensorChannelConfig::getSensorConfigInputAtChannel ( const Types::Index& index ) const
{
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getHWSensorConfig();    
}

QString SensorChannelConfig::getSensorSerialInputAtChannel( const Types::Index& index) const
{
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getHWSensorSerial();    
}


QString SensorChannelConfig::getPosX1AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosX1(); 
}
QString SensorChannelConfig::getPosY1AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosY1(); 
}
QString SensorChannelConfig::getPosZ1AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosZ1(); 
}
QString SensorChannelConfig::getPosX2AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosX2(); 
}
QString SensorChannelConfig::getPosY2AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosY2(); 
}
QString SensorChannelConfig::getPosZ2AtChannel( const Types::Index& index ) const 
{ 
    SingleSensorConfig::Spt ssc = mSensorAtInputVector.at( index.get() ).getSelectedSensorConfigData();
    return ssc.isNull() ? DefaultValues::Undefined : ssc -> getPosZ2(); 
}

void SensorChannelConfig::setHWSelectedInputAtChannel     (const Types::Index& index , const QString& value)
{
    // MWI: add here check, if entry for target input exists, if not, create one!
    //      otherwise the input will not be selected at all, only in GUI

    mSensorAtInputVector.at(index.get()).setHWChannelTypeInputAsQString (value);
    mSensorAtInputVector.at(index.get()).getOrCreateSelectedSensorConfigData ();
}

void SensorChannelConfig::setHWChannelTypeInputAtChannel ( const Types::Index& index, const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setHWChannelType( value );
    
}

void SensorChannelConfig::setHWSensorTypeInputAtChannel  ( const Types::Index& index , const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setHWSensorType( value );
    
}

void SensorChannelConfig::setHWSensorConfigInputAtChannel  ( const Types::Index& index , const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setHWSensorConfig( value );
    
}

void SensorChannelConfig::setPosX1AtChannel( const Types::Index& index  , const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosX1( value );    
}
void SensorChannelConfig::setPosY1AtChannel( const Types::Index& index , const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosY1( value );    
}

void SensorChannelConfig::setPosZ1AtChannel( const Types::Index& index , const QString& value )
{
        if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosZ1( value );
}

void SensorChannelConfig::setPosX2AtChannel( const Types::Index& index , const QString& value )
{
        if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosX2( value );
}

void SensorChannelConfig::setPosY2AtChannel( const Types::Index& index , const QString& value )
{
        if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosY2( value );
}

void SensorChannelConfig::setPosZ2AtChannel( const Types::Index& index , const QString& value )
{
        if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setPosZ2( value );
}



void SensorChannelConfig::setHWSensorSerialInputAtChannel( const Types::Index& index , const QString& value )
{
    if ( not mSensorAtInputVector.at( index.get() ).haveDefinedInput() )
        return;

    mSensorAtInputVector.at( index.get() )
        .getOrCreateSelectedSensorConfigData()
        -> setHWSensorSerial( value );
}


SensorChannelConfig::SingleSensorConfig::Spt SensorChannelConfig::SensorAtInput::getSelectedSensorConfigData() const 
{
    switch ( mSelectedInput ) 
    {
    case InputZero:
        return mSensorAtInputZero;
    case InputOne:
        return mSensorAtInputOne;
    case Off:
    case Undefined:
        return SingleSensorConfig::Spt();
    }
    // prevent braindead compiler from moaning...
    return SingleSensorConfig::Spt();
}

SensorChannelConfig::SingleSensorConfig::Spt SensorChannelConfig::SensorAtInput::getOrCreateSelectedSensorConfigData()
{
    switch (mSelectedInput)
    {
        case InputZero:
            if (mSensorAtInputZero.isNull())
            {
                mSensorAtInputZero = SingleSensorConfig::Spt(new SingleSensorConfig());
            }
            return mSensorAtInputZero;

        case InputOne:
            if (mSensorAtInputOne.isNull())
            {
                mSensorAtInputOne = SingleSensorConfig::Spt( new SingleSensorConfig());
            }
            return mSensorAtInputOne;

        case Undefined:
        default:
            break;
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] called with undefined or unknown input selection!";

    std::stringstream ss; 
    ss << __PRETTY_FUNCTION__ << " called with undefined or unknown input selection!";
    throw std::runtime_error( ss.str() );
}


void SensorChannelConfig::SensorAtInput::setHWChannelTypeInputAsQString (QString const& value)
{
    if (value.toUInt () == 0)
    {
        mSelectedInput = InputZero;
    }
    else if (value.toUInt () == 1)
    {
        mSelectedInput = InputOne;
    }
    else if (value.contains (DefaultValues::Off) == true)
    {
        mSelectedInput = InputZero;
    }
    else
    {
        mSelectedInput = InputZero;
    }
}


void SensorChannelConfig::SensorAtInput::switchSelectedInputTo( const SelectedInput& selectedInput )
{
    mSelectedInput = selectedInput;
}

void SensorChannelConfig::SensorAtInput::setConfigForSelectedInput( SingleSensorConfig::Spt ssc )
{
    switch( mSelectedInput ) 
    {
    case InputZero:
        mSensorAtInputZero = ssc;
        break;
    case InputOne:
        mSensorAtInputOne = ssc;
        break;
    case Undefined:
    case Off:
        break;
    }
}

void SensorChannelConfig::SensorAtInput::setConfigForUnselectedInput( SingleSensorConfig::Spt ssc )
{
    switch( mSelectedInput ) 
    {
    case InputZero:
        mSensorAtInputOne = ssc;
        break;
    case InputOne:
        mSensorAtInputZero = ssc;
        break;
    case Undefined:
    case Off:
        break;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Single sensor config

SensorChannelConfig::SingleSensorConfig::SingleSensorConfig() :
    mChannelType         (DefaultValues::DefaultChannelType),
    mSensorType          (DefaultValues::DefaultSensor),
    mConfig              (DefaultValues::Undefined),
    mSerial              ("0"),
    mAllowedSensorTypes  (DefaultValues::SensorTypeVector()),
    mAllowedChannelTypes (DefaultValues::ChannelTypeVector()),
    mPosX1               ("0"),
    mPosX2               ("0"),
    mPosY1               ("0"),
    mPosY2               ("0"),
    mPosZ1               ("0"),
    mPosZ2               ("0")
{
}


bool SensorChannelConfig::SingleSensorConfig::foundQStringInVector( const DefaultValues::VectorOfQStrings& vector, const QString& value ) const
{
    return std::find( vector.begin() , vector.end() , value ) != vector.end();
}


SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setHWChannelType  ( QString const& value )
{
    if ( foundQStringInVector( mAllowedChannelTypes, value ) )
         mChannelType = value;
    return *this;
}

SensorChannelConfig::SingleSensorConfig&  SensorChannelConfig::SingleSensorConfig::setHWSensorType   ( QString const& value )
{
    if ( foundQStringInVector( mAllowedSensorTypes, value ) )
         mSensorType = value;
    return *this;
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setHWSensorConfig ( QString const& value )
{    
    mConfig = value;
    return *this;
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setHWSensorSerial ( const QString& value )
{
    mSerial = value;
    return *this;
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosX1( const QString& value ) 
{ 
    mPosX1 = value; 
    return *this; 
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosY1( const QString& value ) 
{ 
    mPosY1 = value; 
    return *this; 
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosZ1( const QString& value ) 
{ 
    mPosZ1 = value; 
    return *this; 
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosX2( const QString& value ) 
{ 
    mPosX2 = value; 
    return *this; 
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosY2( const QString& value ) 
{ 
    mPosY2 = value; 
    return *this; 
}

SensorChannelConfig::SingleSensorConfig& SensorChannelConfig::SingleSensorConfig::setPosZ2( const QString& value ) { 
    mPosZ2 = value; 
    return *this; 
}



////////////////////////////////////////////////////////////////////////////////
//
// debug stuff below

std::string SensorChannelConfig::selectedInput2StdString( SensorChannelConfig::SensorAtInput::SelectedInput selInput ) const
{
    switch ( selInput ) {
    case SensorAtInput::InputZero:
        return std::string( "0" );
    case SensorAtInput::InputOne:
        return std::string( "1" );
    case SensorAtInput::Off:
        return std::string( "Off" );
    case SensorAtInput::Undefined:
        return std::string( "Undefined" );
    }
    return std::string("suppress braindead compiler warnings");
}

std::string SensorChannelConfig::toStdString() const
{
    std::stringstream ss;
    unsigned channelNumber = 0;
    for ( SensorAtInputVector::const_iterator it = mSensorAtInputVector.begin();
          it != mSensorAtInputVector.end();
          ++it , ++channelNumber ) {
        ss << " Channel  number : " << channelNumber << "\n"
           << " Selected input  : " << selectedInput2StdString ( it -> getSelectedInput() ) << "\n"
           << " Input Zero : " << ( it -> getSensorAtInputZero ().isNull() ? DefaultValues::Undefined.toStdString() : it -> getSensorAtInputZero () -> toStdString() ) << "\n"
           << " Input one  : " << ( it -> getSensorAtInputOne  ().isNull() ? DefaultValues::Undefined.toStdString() : it -> getSensorAtInputOne  () -> toStdString() ) << "\n";
    }
    return ss.str();
}

std::string SensorChannelConfig::SingleSensorConfig::toStdString() const
{
    std::stringstream ss;
    ss << "Type: " << mChannelType.toStdString() << " "
       << "Sensor : " << mSensorType.toStdString() << " " 
       << "Config : " << mConfig.toStdString();
    return ss.str();
}
