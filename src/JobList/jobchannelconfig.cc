#include "jobchannelconfig.h"

#include <stdexcept>
#include <sstream>
#include "domelementwalker.h"
#include "xml_parse_helper.h"
#include "default_values.h"
#include "datamodel.h"

////////////////////////////////////////////////////////////////////////////////
//
// Job channel config

JobChannelConfig::JobChannelConfig( const DataModel* dataModel ) : 
    mIsValid( false ),
    mIsEnabled( false ),
    mIsConstrutedFromDefaults( true ), // ok, at this point, this is a lie..
    mOutputChannelInformationParsed( false ),
    mData( dataModel )
{}

JobChannelConfig::~JobChannelConfig()
{
}

JobChannelConfig::Spt JobChannelConfig::createFromXmlChannelTag( const DataModel* dataModel, const QDomElement& jobElement )
{
    JobChannelConfig::Spt result = JobChannelConfig::Spt( new JobChannelConfig( dataModel ) );
    result -> initFromDefaults();
    result -> parse( jobElement );
    result -> mIsConstrutedFromDefaults = false;
    return result;
}
    
JobChannelConfig::Spt JobChannelConfig::createFromDefaults     ( const DataModel* dataModel /* maybe we need to choose the set of defaults to use here... */ )
{
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__;
    #endif
    JobChannelConfig::Spt result = JobChannelConfig::Spt( new JobChannelConfig( dataModel ) );
    result -> initFromDefaults();
    result -> mIsConstrutedFromDefaults = true;
    return result;
}

unsigned int JobChannelConfig::parseOutputChannelId( const QDomElement& outputChannelElement )
{
    return JobChannelConfig::parseIdAttribute( outputChannelElement );
}


void JobChannelConfig::initFromDefaults()
{
    mExtGain    = DefaultValues::Undefined;
    mChopper    = DefaultValues::Undefined;
    mGain1      = DefaultValues::Undefined;
    mGain2      = DefaultValues::Undefined;
    mDacVal     = DefaultValues::Undefined;
    mDacOn      = DefaultValues::Undefined;

    mRfFilter   = DefaultValues::Off;
    mLpHpFilter = DefaultValues::Off;

    mPosX1  = DefaultValues::Undefined;
    mPosY1  = DefaultValues::Undefined;
    mPosZ1  = DefaultValues::Undefined;
    mPosX2  = DefaultValues::Undefined;
    mPosY2  = DefaultValues::Undefined;
    mPosZ2  = DefaultValues::Undefined;

}

void JobChannelConfig::parse( const QDomElement& channelElement )
{
    try { 
        mInputChannelElement = channelElement;

        parseId( mInputChannelElement );
        parseExtGain( mInputChannelElement );
        parseHChopper( mInputChannelElement ); 
        parseGain1( mInputChannelElement );
        parseGain2( mInputChannelElement );
        parseDac( mInputChannelElement );
        parseFilter( mInputChannelElement );
        parseInputSelection( mInputChannelElement );
    } catch ( const std::runtime_error& e ) {
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ 
           << " unable to parse job input channel config, reason : "
           << e.what();
        throw std::runtime_error( ss.str() );
    }
    mIsValid = true;
    mIsEnabled = ( ( mInputSelection.compare( DefaultValues::Zero ) == 0 ) or ( mInputSelection.compare( DefaultValues::One ) == 0 ) );
}

void JobChannelConfig::setIsEnabled( bool isEnabled ) 
{ 
    mIsEnabled = isEnabled; 
}


bool JobChannelConfig::isEnabled() const
{
    // IMPORTANT: 
    // if the datamodel is not ready ( it is loading and parsing data ), the jobchannel config uses the data that come from the xml it just parses.
    // after the loading has finished, the information if a channel is enabled or not comes from the datamodel, not from here.
    // when the datamodel has finished, it uses the sensorconfig information (see SensorChannelConfig for details) to tell if a channel can be enabled here!
    if ( ( mData == 0 ) or
         ( mData -> getIsReady() == false ) )
        return mIsEnabled;

    return mIsEnabled and ( mData -> getSensorHWEnabledInputAtChannel( getChannelId() ) );
}


// these poor man's get implementations are good enough for the moment.
QString JobChannelConfig::getExtGain()    const 
{
    return mExtGain;
}

QString JobChannelConfig::getHChopper()    const
{
    return mChopper;
}

QString JobChannelConfig::getGain1()      const
{
    return mGain1;
}

QString JobChannelConfig::getGain2()      const
{
    return mGain2;
}

bool JobChannelConfig::getHasDac()        const
{
    return mData -> getHasDac( getChannelId() );
}


QString JobChannelConfig::getDacVal()        const
{
    return mDacVal;
}

QString JobChannelConfig::getDacOn()      const 
{
    return mDacOn;
}

QString JobChannelConfig::getRfFilter  () const
{
    return mRfFilter;
}

QString JobChannelConfig::getLpHpFilter() const
{
    return mLpHpFilter;
}

QString JobChannelConfig::getInputSelection() const
{
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getSelectedInputAtChannel( getChannelId() );

    return mInputSelection;
}


QString JobChannelConfig::getChannelType()        const
{
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getChannelTypeInputAtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mChannelType : DefaultValues::Undefined; // Ex,Ey...
}

QString JobChannelConfig::getSensorType()         const
{
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getSensorTypeInputAtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mSensorType : DefaultValues::Undefined;
}

QString JobChannelConfig::getSensorConfig()       const
{
    return isOutputChannelInformationParsed() ? QString("NOT YET IMPLEMENTED!") : DefaultValues::Undefined;
}

QString JobChannelConfig::getSensorSerialNumber() const
{
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getSensorSerialInputAtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mSensorSerialNumber : DefaultValues::DefaultSerialNumber;    
}



////////////////////////////////////////////////////////////////////////////////
//
// set methods

void JobChannelConfig::setExtGain( const QString& value )
{
    mExtGain = value;
}

void JobChannelConfig::setChopper( const QString& value )
{
    mChopper = value; 
}
void JobChannelConfig::setGain1( const QString&  value ) 
{ 
    mGain1 = value; 
}

void JobChannelConfig::setDac( const QString&  value ) 
{
    mDacVal = value;
    if (value.toInt() != 0)
    {
        this->mDacOn = "1";
    }
    else
    {
        this->mDacOn = "0";
    }
}

void JobChannelConfig::setGain2( const QString&  value ) 
{ 
    mGain2 = value;
}

void JobChannelConfig::setRfFilter   ( const QString& value )
{
    mRfFilter = value;
}

void JobChannelConfig::setLpHpFilter ( const QString& value )
{
    mLpHpFilter = value;
}

void JobChannelConfig::parseOutputChannelElement( const QDomElement& outputChannelElement )
{
    try {
        mOutputChannelElement = outputChannelElement;
        
        parseChannelType        ( outputChannelElement ); // Ex,Ey...
        parseSensorType         ( outputChannelElement );
        parseSensorSerialNumber ( outputChannelElement );
        parseStartTime          ( outputChannelElement );
        parseStartDate          ( outputChannelElement );
        parseSampleFreq         ( outputChannelElement );
        parseNumSamples         ( outputChannelElement );
        parseAtsDataFile        ( outputChannelElement );
        parseTsLsb              ( outputChannelElement );
        parseSensorCalFile      ( outputChannelElement );
        parsePosX1              ( outputChannelElement );
        parsePosY1              ( outputChannelElement );
        parsePosZ1              ( outputChannelElement );
        parsePosX2              ( outputChannelElement );
        parsePosY2              ( outputChannelElement );
        parsePosZ2              ( outputChannelElement );   
        
    } catch ( const std::runtime_error& e ) {
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ 
           << " unable to parse job output channel config, reason : "
           << e.what();
        throw std::runtime_error( ss.str() );
    }
    mOutputChannelInformationParsed = true;
}


// private

unsigned int JobChannelConfig::parseIdAttribute( const QDomElement& elemWithIdAttribute )
{
    Tools::DomElementWalker walker( elemWithIdAttribute );

    walker.assertElementName( "channel" );
    QString id;
    if ( not walker.getAttribute( "id" , id ) ) {
        std::stringstream ss;
        ss << __PRETTY_FUNCTION__ << " unable to parse job channel tag : no id attribute ";    
        throw std::runtime_error( ss.str() );
    }

    bool conversionOk( false );
    unsigned int resultIdValue = id.toUInt( &conversionOk );
    if ( conversionOk )
        return resultIdValue;

    std::stringstream ss;
    ss << __PRETTY_FUNCTION__ << " unable to parse job channel tag : id attribute is not convertible to unsigned int! " << id.toStdString() ;    
    throw std::runtime_error( ss.str() );
    
}


// private
void JobChannelConfig::parseId( const QDomElement& inputChannelElement )
{

    mId = parseIdAttribute( inputChannelElement );

}

void JobChannelConfig::parseExtGain( const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "ext_gain" );
    mExtGain = walker.get().text();
}

void JobChannelConfig::parseHChopper( const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "hchopper" );
    mChopper = walker.get().text();
}

void JobChannelConfig::parseGain1(  const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "gain_stage1" );
    mGain1 = walker.get().text();
}

void JobChannelConfig::parseGain2( const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "gain_stage2" );
    mGain2 = walker.get().text();
}

void JobChannelConfig::parseDac( const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "dac_val" );
    mDacVal = walker.get().text();
    walker.walkTo( inputChannelElement );
    walker.goToFirstChild( "dac_on" );
    mDacOn = walker.get().text();    
}

void JobChannelConfig::parseFilter( const QDomElement& inputChannelElement )
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "filter_type" );
    // we can have at most one tag here!
    QStringList rfFilterList;
    QStringList lpHpFilterList;
    // FIXME: sync that with allowed values..
    Tools::parseFilterTypeTag( rfFilterList, lpHpFilterList, walker.get().text() );
    mRfFilter   = rfFilterList.size()   == 0 ? DefaultValues::Off : rfFilterList[0];
    mLpHpFilter = lpHpFilterList.size() == 0 ? DefaultValues::Off : lpHpFilterList[0];
}

void JobChannelConfig::parseInputSelection ( const QDomElement& inputChannelElement)
{
    Tools::DomElementWalker walker( inputChannelElement );
    walker.goToFirstChild( "input" );
    mInputSelection = walker.get().text().trimmed();
    if ( ( mInputSelection.compare("0") != 0 ) and
         ( mInputSelection.compare("1") != 0 ) ) // we read strange stuff here...
        mInputSelection = DefaultValues::Undefined;
}

////////////////////////////////////////////////////////////////////////////////
//
// output section 

void JobChannelConfig::parseChannelType( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "channel_type" );
    mChannelType = walker.get().text();
}

void JobChannelConfig::parseSensorType( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "sensor_type" );
    mSensorType = walker.get().text();
}

void JobChannelConfig::parseSensorSerialNumber( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "sensor_sernum" ); 
    mSensorSerialNumber = walker.get().text();
    // qDebug() << __PRETTY_FUNCTION__  << " mSensorSerialNumber " << mSensorSerialNumber;
}

void JobChannelConfig::parseStartTime( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "start_time" ); 
    mStartTime = walker.get().text();
}

void JobChannelConfig::parseStartDate( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "start_date" ); 
    mStartDate = walker.get().text();
}

void JobChannelConfig::parseSampleFreq( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "sample_freq" ); 
    mSampleFreq = walker.get().text();
}

void JobChannelConfig::parseNumSamples( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "num_samples" ); 
    mNumSamples = walker.get().text();
}

void JobChannelConfig::parseAtsDataFile( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "ats_data_file" ); 
    mAtsDataFile = walker.get().text();
}
       
void JobChannelConfig::parseTsLsb( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "ts_lsb" ); 
    mTsLsb = walker.get().text();
}

void JobChannelConfig::parseSensorCalFile( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "sensor_cal_file" ); 
    mSensorCalFile = walker.get().text();
}

void JobChannelConfig::parsePosX1( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_x1" ); 
    mPosX1 = walker.get().text();
}

void JobChannelConfig::parsePosY1( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_y1" ); 
    mPosY1 = walker.get().text();
}
void JobChannelConfig::parsePosZ1( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_z1" ); 
    mPosZ1 = walker.get().text();
}

void JobChannelConfig::parsePosX2( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_x2" ); 
    mPosX2 = walker.get().text();
}

void JobChannelConfig::parsePosY2( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_y2" ); 
    mPosY2 = walker.get().text();
}
void JobChannelConfig::parsePosZ2( const QDomElement& outputChannelElem )
{
    Tools::DomElementWalker walker( outputChannelElem );
    walker.goToFirstChild( "pos_z2" ); 
    mPosZ2 = walker.get().text();
}

QString JobChannelConfig::getPosX1() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosX1AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosX1  : DefaultValues::Undefined; 
}
QString JobChannelConfig::getPosY1() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosY1AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosY1  : DefaultValues::Undefined; 
}
QString JobChannelConfig::getPosZ1() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosZ1AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosZ1  : DefaultValues::Undefined; 
}
QString JobChannelConfig::getPosX2() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosX2AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosX2  : DefaultValues::Undefined; 
}
QString JobChannelConfig::getPosY2() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosY2AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosY2  : DefaultValues::Undefined; 
}
QString JobChannelConfig::getPosZ2() const 
{ 
    if ( not mSensorChannelConfig.isNull() )
        return mSensorChannelConfig -> getPosZ2AtChannel( getChannelId() );

    return isOutputChannelInformationParsed() ? mPosZ2  : DefaultValues::Undefined; 
}


