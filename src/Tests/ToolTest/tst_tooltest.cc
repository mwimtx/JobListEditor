#include "tst_tooltest.h"

#include <QCoreApplication>

#include <QStringList>
#include <cli_parser.h>
#include "codemapper.h"
#include "default_values.h"

#include <iostream>
#include <QDebug>

ToolTest::ToolTest()
{
}

void ToolTest::initTestCase()
{
}

void ToolTest::cleanupTestCase()
{
}

void ToolTest::testCLIParser()
{
    QStringList testArgs;

    testArgs << "applicationName" << "--opt1" << "val1" << "--opt2" << "val2";

    Tools::CLIParser parser;

    parser
        .addOption( "--opt1" , "opt1" )
        .addOption( "--opt2" , "opt2" )
        .addOption( "--opt3" , "opt3" );

    QVERIFY( parser.isValid() == false );
    
    parser.parse( testArgs );

    QVERIFY( parser.isValid() == true );

    QVERIFY( parser.getValue( "--opt1" ).compare( "val1" ) == 0 );
    QVERIFY( parser.getValue( "--opt2" ).compare( "val2" ) == 0 );
    QVERIFY( parser.getValue( "--opt3" ).compare( "opt3" ) == 0 );

}

void ToolTest::testCodeMapper() {

    // LF_RF_1 -> 3000 , LF_RF_2 -> 1000 LF_RF_3 -> 500 , LF_RF_4 -> 200 

    Tools::CodeMapper cm;

    cm
        .addMapping( "LF_RF_1" , "3000" )
        .addMapping( "LF_RF_2" , "1000" )
        .addMapping( "LF_RF_3" , "500" )
        .addMapping( "LF_RF_4" , "200" );

    QVERIFY( cm.toVisual( "LF_RF_1" ).compare( "3000" ) == 0 );

    QVERIFY( cm.toCode( "3000" ).compare( "LF_RF_1" ) == 0 );

    QVERIFY( cm.toVisual ( "DOES_NOT_EXIST" ).compare( DefaultValues::Off ) == 0 );
    QVERIFY( cm.toCode   ( "DOES_NOT_EXIST" ).compare( DefaultValues::Off ) == 0 );

    bool haveException( false );
    try {
        cm.addMapping( "LF_RF_1" , "3000" );
    } catch ( const std::runtime_error& ) {
        haveException = true;
    }
    QVERIFY( haveException );
}

#include <algorithm>

void ToolTest::testDefaultValues()
{
    const DefaultValues::VectorOfQStrings& channelTypes ( DefaultValues::ChannelTypeVector() );
    QVERIFY( channelTypes.size() == 10 );

    DefaultValues::VectorOfQStrings::const_iterator it = std::find( channelTypes.begin() , channelTypes.end() , QString( "RHz" ) );

    
    std::cout << " it " << (*it).toStdString();
                                                                    

}



int main(int argc, char *argv[]) 
{ 
    #ifdef DISABLE_KEYPAD_NAVIGATION
    QApplication::setNavigationMode(Qt::NavigationModeNone);
    #endif
    QCoreApplication app(argc, argv); 
    ToolTest testObject; 
    return QTest::qExec(&testObject, argc, argv); 
}


