#include "hwdatabase.h"

#include <sstream>
#include <stdexcept>

#include <domelementwalker.h>
#include <xml_parse_helper.h>

////////////////////////////////////////////////////////////////////////////////
//
// sensor specific data

void SensorInfo::parse()
{
}



////////////////////////////////////////////////////////////////////////////////
//
// Board specific data

AdbBoardInfo::AdbBoardInfo( HWDBQueryResult::Spt queryResult ) :
    HwInfo( queryResult ),
    mChopperList( QStringList() << "0" << "1" << "auto" )
{}

void AdbBoardInfo::parse()
{
    if (this->getType() == 255)
    {
        this->mDacHigh = 0;
        this->mDacLow  = 0;
        this->mExtGain.clear();
        this->mGainStage1.clear();
        this->mGainStage2.clear();
        this->mLpHpFilter.clear();
        this->mRfFilter.clear();
        this->mSampleFreqs.clear();
    }
    else
    {
        try {
            Tools::DomElementWalker walker( getDomElement() );
            walker.assertElementName( "Board" );
            parseExtGain    ();
            parseFilters    ();
            parseGainStage1 ();
            parseGainStage2 ();
            parseSampleFreqs();
            parseDac();
        } catch ( const std::runtime_error& e ) {
            std::stringstream ss;
            ss << __PRETTY_FUNCTION__ << " parse of adb board info failed, reason : " << e.what();
            throw std::runtime_error( ss.str() );
        }
    }
}

void AdbBoardInfo::parseExtGain()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild( "ExtGain" );
    mExtGain = Tools::splitStringToStringList( walker.get().text() );
}

void AdbBoardInfo::parseFilters()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild( "Filters" );
    Tools::parseFilterTypeTag( mRfFilter, mLpHpFilter, walker.get().text() );

    // for LF board, do not append "Off" as on LF board there is not possibility to
    // completely switch off RF filter
    if (this->getType() != ADB_LF)
    {
        mRfFilter.append( DefaultValues::Off );
    }

    mLpHpFilter.append( DefaultValues::Off );
}

void AdbBoardInfo::parseGainStage1()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild( "GainsStage1" );
    mGainStage1 = Tools::splitStringToStringList( walker.get().text() );    
}

void AdbBoardInfo::parseGainStage2()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild( "GainsStage2" );
    mGainStage2 = Tools::splitStringToStringList( walker.get().text() );        
}

void AdbBoardInfo::parseSampleFreqs()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild ("SampleFreqs");
    mSampleFreqs = Tools::splitStringToStringList( walker.get().text() );
}

void AdbBoardInfo::parseDac()
{
    Tools::DomElementWalker walker( getDomElement() );
    walker.goToFirstChild( "DAC" );
    QStringList dacVal = Tools::splitStringToStringList( walker.get().text() );   

    if ( dacVal.size() > 2 )
        throw std::runtime_error(" unable to parse DAC tag" );

    if ( dacVal.size() < 2 ) { // no dac val available.
        mDacLow = mDacHigh = 0;
        return;
    }        

    bool p0 ( false ); mDacLow  = dacVal[ 0 ].toInt( &p0 );
    bool p1 ( false ); mDacHigh = dacVal[ 1 ].toInt( &p1 );
    if  ( p0 and p1 )
        return;

    std::stringstream ss;
    ss << "unable to parse DAC tag, numeric values [" << dacVal[ 0 ].toStdString() << "]"
       << " or "
       << "[" << dacVal[ 1 ].toStdString() << "] not parsable!" ;
    throw std::runtime_error( ss.str() );
}


QStringList AdbBoardInfo::getExtGainRawList() const
{
    return mExtGain;
}

QString     AdbBoardInfo::getExtGainMin()   const
{
    return mExtGain[0]; // this implementation is good enough for now...
}

QString     AdbBoardInfo::getExtGainMax()   const
{
    return mExtGain[1]; 
}


QStringList AdbBoardInfo::getChopper() const
{
    // fake till now; does not exist in HHDB
    return mChopperList;
}

QStringList AdbBoardInfo::getRfFilter() const
{
    return mRfFilter;
}

QStringList AdbBoardInfo::getGainStage1() const
{
    return mGainStage1;
}

QStringList AdbBoardInfo::getSampleFreqs() const
{
    return mSampleFreqs;
}

QStringList AdbBoardInfo::getGainStage2() const
{
    return mGainStage2;    
}

int         AdbBoardInfo::getDacRangeLow() const
{
    return mDacLow;
}

int         AdbBoardInfo::getDacRangeHigh() const
{
    return mDacHigh;
}

bool        AdbBoardInfo::getHasDac()       const // does this board have a dac? Or, more precisely, we guess that from getDacRangeLow and getDacRangeHigh.
{
    return getDacRangeLow() < getDacRangeHigh(); 
}


QStringList AdbBoardInfo::getLpHpFilter() const
{
    return mLpHpFilter;
}


////////////////////////////////////////////////////////////////////////////////
//
// the hardware db itself

// static factory
HardwareDB::Spt HardwareDB::createFromFile( QFile& hwXmlFile )
{

    // FIXME: get this into tools... static stuff?
    if ( not hwXmlFile.open(QIODevice::ReadOnly) ) {
        std::stringstream ss;
        ss << "unable to open file : " << hwXmlFile.fileName().toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }

    HardwareDB::Spt hwdb = HardwareDB::Spt( new HardwareDB() );

    QString errorMsg;
    int     errorLine;
    int     errorColumn;
    if (not hwdb->mHWDbDocument.setContent(&hwXmlFile,true,&errorMsg,&errorLine,&errorColumn))
    {
        hwXmlFile.close();
        std::stringstream ss;
        ss << "unable to parse file : " << hwXmlFile.fileName().toStdString() << " reason : " << errorMsg.toStdString() << " line : " << errorLine << " column " << errorColumn << std::endl;
        throw std::runtime_error( ss.str() );
    }

    hwdb->setup();

    return hwdb;
}

HardwareDB::HardwareDB() 
{
    
}

void HardwareDB::setup()
{
    QDomAttr clAttr;

    clAttr = mHWDbDocument.documentElement().attributeNode("Version");
    if (clAttr.isNull() == false)
    {
        mVersionString = clAttr.value();
    }
    else
    {
        mVersionString = "N/A";
    }

    qDebug () << __PRETTY_FUNCTION__ << "HwDatabase Version:" << mVersionString;
}

HWDBQueryResult::Spt HardwareDB::query( const HWDBKeyData& hwQuery )
{
    // we could improve this for performance, but like this it is more readable.

    Tools::DomElementWalker startAt( mHWDbDocument );

    // we dont query items labled device till now!
    startAt.goToFirstChild( "Board" );

    HWDBQueryResult::Spt result = HWDBQueryResult::Spt( new HWDBQueryResult( hwQuery ) );

    Tools::DomElementSiblingIterator iterator( startAt );

    while ( iterator.isValid() ) 
    {
        Tools::DomElementWalker GMS( iterator );
        GMS.goToFirstChild( "GMS" );

        Tools::DomElementWalker type( iterator );
        type.goToFirstChild( "Type" );

        Tools::DomElementWalker revMain( iterator );
        revMain.goToFirstChild( "RevMain" );

        Tools::DomElementWalker revSub( iterator );
        revSub.goToFirstChild( "RevSub" );

        if ( ( GMS.get().text().toUInt()     == hwQuery.getGSM()     ) and
             ( type.get().text().toUInt()    == hwQuery.getType()    ) and
             ( revMain.get().text().toUInt() == hwQuery.getRevMain() ) and
             ( revSub.get().text().toUInt()  == hwQuery.getRevSub()  ) 
            )
        {
            // this is the start node in the hw db, not from the current cfg!
            result -> setResultDomElement( iterator.get() );
            break;
        }
        iterator.next();
    }

    return result;

}

////////////////////////////////////////////////////////////////////////////////
//
// HardwareConfig

HardwareConfig::HardwareConfig()
{}

HardwareConfig::Spt HardwareConfig::createFromFile( QFile& aduHwConfigFile , HardwareDB::Spt hwdb )
{
    if ( not aduHwConfigFile.open(QIODevice::ReadOnly) ) {
        std::stringstream ss;
        ss << "unable to open file : " << aduHwConfigFile.fileName().toStdString() << std::endl;
        throw std::runtime_error( ss.str() );
    }

    HardwareConfig::Spt cfg = HardwareConfig::Spt( new HardwareConfig() );

    QString errorMsg;
    int     errorLine;
    int     errorColumn;
    if ( not cfg -> mHWCfgDocument.setContent(&aduHwConfigFile,true,&errorMsg,&errorLine,&errorColumn) ) {
        aduHwConfigFile.close();
        std::stringstream ss;
        ss << "unable to parse file : " << aduHwConfigFile.fileName().toStdString() << " reason : " << errorMsg.toStdString() << " line : " << errorLine << " column " << errorColumn << std::endl;
        throw std::runtime_error( ss.str() );
    }

    if ( hwdb.isNull() ) 
        throw std::runtime_error( "HardwareConfig: hardware db is null; dont know where to look up stuff. " );

    cfg -> mHwDb = hwdb;

    cfg -> setup();

    return cfg;
    
}

void HardwareConfig::setup()
{
    QDomAttr clAttr;

    clAttr = mHWCfgDocument.documentElement().attributeNode("Version");
    if (clAttr.isNull() == false)
    {
        mVersionString = clAttr.value();
    }
    else
    {
        mVersionString = "N/A";
    }

    qDebug () << __PRETTY_FUNCTION__ << "HwConfig Version:" << mVersionString;

    // init ADB boards from HwDatabase
    loadAdbBoards();

    // init rest of boards from HwDatabase
    loadBoards();

    // init sensors from HwDatabase
    loadSensors();
}

void HardwareConfig::loadAdbBoards()
{
    Tools::DomElementWalker walker( mHWCfgDocument );
    
    walker
        .goToFirstChild( "AdbBoards" )
        .goToFirstChild( "channel" );

    Tools::DomElementSiblingIterator si( walker );

    loadHardwareConfigData( si , mAdbBoardVector );

}

void HardwareConfig::loadBoards()
{
    {
        Tools::DomElementWalker walker( mHWCfgDocument );

        // seems that :
        // Type == 10 --> LF board
        // Type == 11 --> HF board
        // Type == 12 --> MF board

        walker
            .goToFirstChild( "Board" );

        Tools::DomElementSiblingIterator si( walker );
        loadHardwareConfigData( si , mHwDataVector );
    }

    {
        Tools::DomElementWalker walker( mHWCfgDocument );

        // seems that :
        // Type == 10 --> LF board
        // Type == 11 --> HF board
        // Type == 12 --> MF board

        walker
            .goToFirstChild( "Device" );

        Tools::DomElementSiblingIterator si( walker );
        loadHardwareConfigData (si, this->mHwDeviceVector);
    }

}

void HardwareConfig::loadSensors()
{

    Tools::DomElementWalker walker( mHWCfgDocument );
    
    walker
        .goToFirstChild( "Sensors" )
        .goToFirstChild( "channel" );

        Tools::DomElementSiblingIterator si( walker );

        // query the database...
        loadHardwareConfigData( si , mSensorsVector );

        createSensorInfoVector();

        createAdbBoardInfoVector();
}

void HardwareConfig::createSensorInfoVector()
{
    if ( mSensorsVector.size() > DefaultValues::MaxChannels )
        throw std::runtime_error( "Input file contains more channels than 10!" );

    // empty SensorInfo::Spt means: channel not detected
    mSensorInfoVector.reserve( DefaultValues::MaxChannels );
    mSensorInfoVector = SensorInfoVector( 10 , SensorInfo::Spt() );

    for ( HwDataVector::iterator it = mSensorsVector.begin();
          it != mSensorsVector.end();
          ++it )
    {
        HWDBQueryResult::Spt queryResult = (*it);
        if ( not queryResult -> hasId() ) 
        {
            printf ("============>>>>>>>>>>>>>> 111111\n");
            fflush (stdout);

            std::stringstream ss;
            ss << __PRETTY_FUNCTION__ << " unable to create hardware information for sensor \"" << (*it) -> getName().toStdString() << "\" without attribute id!";
            throw std::runtime_error( ss.str() );
        }
        SensorInfo::Spt sensorInfo = SensorInfoFactory::create( queryResult );
        sensorInfo -> parse();
        
        try {
            mSensorInfoVector.at( sensorInfo -> getChannelId() ) = sensorInfo;
        } catch ( const std::out_of_range& ) {

            printf ("============>>>>>>>>>>>>>> 111111\n");
            fflush (stdout);

            std::stringstream ss;
            ss << __PRETTY_FUNCTION__ << " channel id for \"" << (*it) -> getName().toStdString() << "\" seems to be out of range or invalid: [" << sensorInfo -> getChannelId() << "]";
            throw std::runtime_error( ss.str() );
        }
        #ifdef DEBUG_OUTPUT
        qDebug()  << __PRETTY_FUNCTION__ << " Name : " << sensorInfo -> getName() << " channel id : " << sensorInfo -> getChannelId();
        #endif
    }


}

// if we need to create one more instance of this, we d go the template way, i think.
void HardwareConfig::createAdbBoardInfoVector()
{
    // FIXME (maybe): now we parse every entry regardless if we had parsed it before: maybe we should create a cache here (if necessary, i doubt it).
    if ( mAdbBoardVector.size() > DefaultValues::MaxChannels )
    {
        throw std::runtime_error( "Input file contains more channels than 10!" );
    }
    
    mAdbBoardInfoVector.reserve( DefaultValues::MaxChannels );
    mAdbBoardInfoVector = AdbBoardInfoVector( 10 , AdbBoardInfo::Spt() );

    for ( HwDataVector::iterator it = mAdbBoardVector.begin();
          it != mAdbBoardVector.end();
          ++it )
    {
        HWDBQueryResult::Spt queryResult = (*it);
        if ( not queryResult -> hasId() ) 
        {
            std::stringstream ss;
            ss << __PRETTY_FUNCTION__ << " unable to create hardware information for adb board \"" << (*it) -> getName().toStdString() << "\" without attribute id!";
            throw std::runtime_error( ss.str() );
        }

        AdbBoardInfo::Spt adbBoardInfo = AdbBoardInfoFactory::create( queryResult );
        adbBoardInfo -> parse();
        
        qDebug()  << __PRETTY_FUNCTION__ << " Name : " << adbBoardInfo -> getName() << " channel id : " << adbBoardInfo -> getChannelId();

        if (adbBoardInfo -> getChannelId() < mAdbBoardInfoVector.size())
        {
            try
            {
                mAdbBoardInfoVector.at( adbBoardInfo -> getChannelId() ) = adbBoardInfo;
            } catch ( const std::out_of_range& )
            {
                std::stringstream ss;
                ss << __PRETTY_FUNCTION__ << " channel id for \"" << (*it) -> getName().toStdString() << "\" seems to be out of range or invalid: [" << adbBoardInfo -> getChannelId() << "]";
                throw std::runtime_error( ss.str() );
            }
        }
    }

    printf ("lalalalalala\n");

}


void HardwareConfig::loadHardwareConfigData( Tools::DomElementSiblingIterator& iterator, HwDataVector& hwDataVector )
{
    while ( iterator.isValid() )
    {
        Tools::DomElementWalker visitor( iterator );

        HWDBKeyData keyData;

        QString idAttribute;
        if ( visitor.getAttribute( "id" , idAttribute ) ) 
        {
             keyData.setId( idAttribute.toUInt() );
        }

        visitor.goToFirstChild( "GMS" );
        keyData.setGSM( visitor.get().text().toUInt() );
        visitor.walkTo( iterator );

        visitor.goToFirstChild( "Type" );
        keyData.setType( visitor.get().text().toUInt() );
        visitor.walkTo( iterator );

        visitor.goToFirstChild( "RevMain" );
        keyData.setRevMain( visitor.get().text().toUInt() );
        visitor.walkTo( iterator );

        visitor.goToFirstChild( "RevSub" );
        keyData.setRevSub( visitor.get().text().toUInt() );
        visitor.walkTo( iterator ); // not needed

        HWDBQueryResult::Spt result = mHwDb -> query( keyData );

#ifdef DEBUG_OUTPUT
        qDebug()  << result -> toStdString().c_str();
#endif
        hwDataVector.push_back( result );
        
        iterator.next();
    }
    
}

SensorInfo::Spt HardwareConfig::getSensorInfoForChannel( const Types::Index& index )
{
    return mSensorInfoVector.at( index.get() );
}

AdbBoardInfo::Spt HardwareConfig::getAdbBoardInfoForChannel( const Types::Index& index )
{
    return mAdbBoardInfoVector.at( index.get() );
}

QDomElement HardwareConfig::getXMLHwConfigNode() const
{
    return mHWCfgDocument.documentElement();
}


bool HardwareConfig::isTXM(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (mHwDeviceVector.size() > 0)
    {
        for ( HwDataVector::iterator it = mHwDeviceVector.begin();
              it != mHwDeviceVector.end();
              ++it )
        {
            if (((*it)->getGSM () == GMS_07) && ((*it)->getType () == DEVICE_TXB07))
            {
                bRetValue = true;
            }
        }
    }

    return (bRetValue);
}


bool HardwareConfig::isMF(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (mAdbBoardInfoVector.size() > 0)
    {
        for (AdbBoardInfoVector::iterator it = mAdbBoardInfoVector.begin(); it != mAdbBoardInfoVector.end(); ++it)
        {
            if (it->isNull() == false)
            {
                if (((*it)->getGSM () == GMS_07) && ((*it)->getType () == ADB_MF))
                {
                    bRetValue = true;
                }
            }
        }
    }

    return (bRetValue);
}


bool HardwareConfig::isADU08eBB(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (mAdbBoardInfoVector.size() > 0)
    {
        for (AdbBoardInfoVector::iterator it = mAdbBoardInfoVector.begin(); it != mAdbBoardInfoVector.end(); ++it)
        {
            if (it->isNull() == false)
            {
                if (((*it)->getGSM () == GMS_08) && ((*it)->getType () == ADB_ADU08_BB))
                {
                    bRetValue = true;
                }
            }
        }
    }

    return (bRetValue);
}


bool HardwareConfig::isADU09u10e(void)
{
    // declaration of variables
    bool bRetValue = false;

    if (mAdbBoardInfoVector.size() > 0)
    {
        for (AdbBoardInfoVector::iterator it = mAdbBoardInfoVector.begin(); it != mAdbBoardInfoVector.end(); ++it)
        {
            if (it->isNull() == false)
            {
                if ((((*it)->getGSM () == GMS_09) || ((*it)->getGSM () == GMS_10)) && ((*it)->getType () == ADB_ADU09U_10E))
                {
                    bRetValue = true;
                }
            }
        }
    }

    return (bRetValue);
}

