

#include "hwdatabase.h"

#include "tst_hwdbteststest.h"

#include <QString>
#include <QtTest>
#include <QFile>
#include <QDomDocument>


#include <sstream>
#include <stdexcept>

#include <domelementwalker.h>
#include <xml_parse_helper.h>



HWDbTestsTest::HWDbTestsTest()
{
}

void HWDbTestsTest::initTestCase()
{
}

void HWDbTestsTest::cleanupTestCase()
{
}

void HWDbTestsTest::testHWDB_0()
{
    QFile HWFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwDatabase.xml" );

    HardwareDB::Spt hwdb = HardwareDB::createFromFile( HWFile );

    QVERIFY( not hwdb.isNull() );

    QVERIFY( hwdb -> getVersion().compare( QString( "2.14" ) ) == 0 );

    // HWDBKeyData come from HwConfig.xml and *not* from HwDatabase.xml (this is the file to be queried.
    HWDBKeyData q0;

    // find "MFS06e"

    q0
        .setGSM    (   7 )
        .setType   ( 200 )
        .setRevMain(   1 )
        .setRevSub (   0 )
        .setId     (  42 ); // id is ignored when querying the hw db.

    QVERIFY( q0.hasId() and 
             ( q0.getId() == 42 ) );

    HWDBQueryResult::Spt result = hwdb -> query( q0 );

    QVERIFY( not result.isNull() );

    QVERIFY( result -> isVaild() );

    // does not exist...!    
    HWDBKeyData q1;
    q1
        .setGSM    ( 666 )
        .setType   ( 666 )
        .setRevMain( 666 )
        .setRevSub ( 666 );

    HWDBQueryResult::Spt resultInvalid = hwdb -> query( q1 );

    QVERIFY( not( resultInvalid -> isVaild() ) );

}

void HWDbTestsTest::testADUHardwareDescription_0()
{
    QFile HWFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwDatabase.xml" );

    HardwareDB::Spt hwdb = HardwareDB::createFromFile( HWFile );

    QFile ADUHardwareFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwConfig.xml" );

    HardwareConfig::Spt cfg = HardwareConfig::createFromFile( ADUHardwareFile , hwdb );

    QVERIFY( not cfg.isNull() );

    QVERIFY( cfg -> getVersion().compare( QString( "1.0" ) ) == 0 );

    SensorInfo::Spt si_0 = cfg -> getSensorInfoForChannel( 3 );

    QVERIFY( not si_0.isNull() );

    // hope this enough checks...
    QVERIFY( si_0 -> getName().compare( QString( "UNKN_E" ) ) == 0 );
    QVERIFY( si_0 -> getChannelId() == 3 );

}

void HWDbTestsTest::testADUHardwareDescription_1()
{
    QFile HWFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwDatabase.xml" );

    HardwareDB::Spt hwdb = HardwareDB::createFromFile( HWFile );

    // this file only contains sensor definitions for channel 5 
    QFile ADUHardwareFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwConfig-testversion.1.xml" );

    HardwareConfig::Spt cfg = HardwareConfig::createFromFile( ADUHardwareFile , hwdb );

    QVERIFY( not cfg.isNull() );

    QVERIFY( cfg -> getVersion().compare( QString( "1.0" ) ) == 0 );

    SensorInfo::Spt si_3 = cfg -> getSensorInfoForChannel( 3 );

    QVERIFY( si_3.isNull() );

    SensorInfo::Spt si_5 = cfg -> getSensorInfoForChannel( 5 );

    QVERIFY( not si_5.isNull() );
    QVERIFY( si_5 -> getName().compare( QString( "UNKN_E" ) ) == 0 );
    QVERIFY( si_5 -> getChannelId() == 5 );

}

void HWDbTestsTest::testADUHardwareDescription_2()
{
    QFile HWFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwDatabase.xml" );
    HardwareDB::Spt hwdb = HardwareDB::createFromFile( HWFile );
    QFile ADUHardwareFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/HWConfigData/HwConfig.xml" );
    HardwareConfig::Spt cfg = HardwareConfig::createFromFile( ADUHardwareFile , hwdb );
    QVERIFY( not cfg.isNull() );
    QVERIFY( cfg -> getVersion().compare( QString( "1.0" ) ) == 0 );

    AdbBoardInfo::Spt boardCfg = cfg -> getAdbBoardInfoForChannel( 1 );
    QVERIFY( not boardCfg.isNull() );
    // sould be : GMS=7,Type=10,RevMain=1, RevSub=0
    QVERIFY( boardCfg -> getName().compare( QString( "ADU-07-LF" ) ) == 0 );
    
    QStringList radioFilter = boardCfg -> getRfFilter();
    QVERIFY( radioFilter.indexOf( "LF_RF_1" ) >= 0 );
    QVERIFY( radioFilter.indexOf( "LF_RF_2" ) >= 0 );
    QVERIFY( radioFilter.indexOf( "LF_RF_3" ) >= 0 );
    QVERIFY( radioFilter.indexOf( "LF_RF_4" ) >= 0 );
    QVERIFY( radioFilter.indexOf("LF_LP_4HZ" ) < 0 ); // this is a LP/HP Filter, must not be here...

    QStringList lphpFilter = boardCfg -> getLpHpFilter();
    QVERIFY( lphpFilter.indexOf("LF_LP_4HZ" ) >= 0 );
    QVERIFY( lphpFilter.indexOf( "LF_RF_1" )  < 0 );

    {
        QStringList stringList = boardCfg -> getExtGainRawList();
        QVERIFY( stringList[ 0 ].compare( "0.0" ) == 0 );
        QVERIFY( stringList[ 1 ].compare( "1000.0" ) == 0 );
        QVERIFY( boardCfg -> getExtGainMin().compare( "0.0" )    == 0 );
        QVERIFY( boardCfg -> getExtGainMax().compare( "1000.0" ) == 0 );
        
    }

    {
        QStringList stringList = boardCfg -> getGainStage1();
        QVERIFY( stringList[ 0 ].compare( "1" ) == 0 );
        QVERIFY( stringList[ 1 ].compare( "2" ) == 0 );
        QVERIFY( stringList[ 2 ].compare( "4" ) == 0 );
        QVERIFY( stringList[ 3 ].compare( "8" ) == 0 );
        QVERIFY( stringList[ 4 ].compare( "16" ) == 0 );
        QVERIFY( stringList[ 5 ].compare( "32" ) == 0 );
        QVERIFY( stringList[ 6 ].compare( "64" ) == 0 );
    }

    {
        QStringList stringList = boardCfg -> getGainStage2();
        QVERIFY( stringList[ 0 ].compare( "1" ) == 0 );
        QVERIFY( stringList[ 1 ].compare( "2" ) == 0 );
        QVERIFY( stringList[ 2 ].compare( "4" ) == 0 );
        QVERIFY( stringList[ 3 ].compare( "8" ) == 0 );
        QVERIFY( stringList[ 4 ].compare( "16" ) == 0 );
        QVERIFY( stringList[ 5 ].compare( "32" ) == 0 );
        QVERIFY( stringList[ 6 ].compare( "64" ) == 0 );
    }

    QVERIFY( boardCfg -> getDacRangeLow() == -2500 );
    QVERIFY( boardCfg -> getDacRangeHigh() == 2500 );

}


int main(int argc, char *argv[]) 
{ 
    #ifdef DISABLE_KEYPAD_NAVIGATION
    QApplication::setNavigationMode(Qt::NavigationModeNone);
    #endif
    QCoreApplication app(argc, argv); 
    HWDbTestsTest testObject; 
    return QTest::qExec(&testObject, argc, argv); 
}

