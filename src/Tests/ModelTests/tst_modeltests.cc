 #include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "tst_modeltests.h"
#include "sensorchannelconfig.h"

bool ModelTests::loadMeasurementFile()
{
    return loadXmlFile( mTestDataDir.absoluteFilePath( "ADU07JobFile.xml" ) );

}

bool ModelTests::loadJobListFile() 
{
    return loadXmlFile( mTestDataDir.absoluteFilePath( "ADU-07/ADUConf/ADUConf.xml" ) );
}


bool ModelTests::loadXmlFile( const QString& fileName ) 
{

    QFileInfo fileInfo( fileName );

    QFile jobXmlFile( fileInfo.canonicalFilePath() );
    
    if ( not jobXmlFile.open(QIODevice::ReadOnly) ) {
        std::cout << "unable to open file : " << jobXmlFile.fileName().toStdString() << std::endl;
        return false;
    }

    if ( not mDomDoc.setContent(&jobXmlFile) ) {
        jobXmlFile.close();
        return false;
    }
    return true;
    
}

////////////////////////////////////////////////////////////////////////////////

ModelTests::ModelTests()
{
}

void ModelTests::initTestCase()
{
    mTestDataDir.setPath (mTestDataDir.canonicalPath ().toStdString().c_str());
    mDomDoc = QDomDocument("JobXml");
}

void ModelTests::cleanupTestCase()
{
}

void ModelTests::domWalkerTest()
{

    QVERIFY2( loadMeasurementFile(), "Unable to load measurement file!" );

    Tools::DomElementWalker w0( mDomDoc );

    bool haveException = false;
    try {
        w0.goToFirstChild( "BLAH" );        
    } catch ( const std::runtime_error& e ) {
        haveException = true;
        // std::cout << "Exception : [" << e.what() << "]" << std::endl;
    }
     
    QVERIFY( haveException );

    QVERIFY( w0.getCurrentElementName().compare( "measurement" ) == 0 );
   
    try {
        w0
            .goToFirstChild( "recording" )
            .goToFirstChild( "input" )
            .goToFirstChild( "Hardware" )
            .goToFirstChild( "channel_config" );

        QVERIFY( w0.getCurrentElementName().compare( "channel_config" ) == 0 );

        Tools::DomElementWalker w1( w0 );

        // test the "hasChild method"
        QVERIFY( w0.hasChild( "channel" ) == true );
        QVERIFY( w0.hasChild( "somethingElse" ) == false );

        w0
            .goToFirstChild( "channel" );

        w0.goToFirstSiblingWithAttributeAndAttributeValue( "id" , "2" );

        QVERIFY( w0.getCurrentElementName().compare( "channel" ) == 0 );
        QVERIFY( w1.getCurrentElementName().compare( "channel_config" ) == 0 );

        #ifdef DEBUG_OUTPUT
        std::cout << "w0.getCurrentElementName() [" << w1.getCurrentElementNameStd() << "]" << std::endl;
        std::cout << "w1.getCurrentElementName() [" << w1.getCurrentElementNameStd() << "]" << std::endl;
        #endif

    } catch ( const std::runtime_error& e ) {
        std::cout << "Exception : [" << e.what() << "]" << std::endl;
    }
    
    #ifdef DEBUG_OUTPUT
    std::cout << "w0.getCurrentElementName() [" << w0.getCurrentElementName().toStdString() << "]" << std::endl;
    #endif

}

void ModelTests::domWalkerIteratorTest()
{
    QVERIFY2( loadJobListFile() , "Unable to load loblist!" );

    Tools::DomElementWalker walker( mDomDoc );

    walker.assertElementName( "ADUConf" );

    #ifdef DEBUG_OUTPUT
    std::cout << __PRETTY_FUNCTION__ << " walker : " << walker.getCurrentElementNameStd() << std::endl;
    #endif

    walker
        .goToFirstChild( "JobList" )
        .goToFirstChild( "Job" );

    Tools::DomElementSiblingIterator jobIterator( walker );

    while ( jobIterator.isValid() ) {
        #ifdef DEBUG_OUTPUT
        std::cout << "jobIterator.getCurrentElementNameStd(): " << jobIterator.getCurrentElementNameStd() << std::endl;
        #endif
        jobIterator.assertElementName( "Job" );
        jobIterator.next();
    }


}

void ModelTests::emptyJobTest()
{
    // declaration of variables
    QString qstrTmp;
    double  dTmp;

    Job::Spt job = Job::createFromTemplate();

    job -> setFrequency( 666 );
    QVERIFY( job -> getFrequency(qstrTmp, dTmp) == 666 );

}


void ModelTests::dataModelTest_0()
{
    DataModel dataModel;

    try {
        dataModel.setAdjustPause( ControlData::AdjustAuto ); // we need to enbale time calculation, since we test it here...
        dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
        dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    } catch ( const std::runtime_error& e ) {
        std::cout << "\n********************************************************************************\n"
                  << e.what() << "\n"
                  << std::endl;
        QFAIL( "Hulk" );
    }
    
    QDateTime shiftStart( QDateTime::currentDateTimeUtc() );
    
    ////////////////////////////////////////
    //
    // test adjust to minute
    unsigned int numberOfJobs = dataModel.jobCount();
    dataModel.setAdjustToMinute( true );
    for ( unsigned jobNumber = 0; jobNumber < numberOfJobs; ++jobNumber ) 
    {
        QTime startTime = dataModel.startTimeOfJob( jobNumber );
        QVERIFY( startTime.second() == 0 );
    }

    ////////////////////////////////////////
    //
    // test adjust pause
    dataModel.setAdjustPause( ControlData::Adjust5Min );
    #ifdef DEBUG_OUTPUT
    qDebug() << " ControlData::Adjust5Min " << ControlData::Adjust5Min;
    #endif
    QTime prevStopTime;
    for ( unsigned jobNumber = 0; jobNumber < numberOfJobs; ++jobNumber ) 
    {
        QTime startTime = dataModel.startTimeOfJob( jobNumber );
        QTime stopTime  = dataModel.stopTimeOfJob( jobNumber );
        // qDebug()  << " startTime " << startTime << " stopTime " << stopTime;
        if ( jobNumber > 0 ) // make sure that there is more than ControlData::Adjust5Min time between start and stop
            QVERIFY( startTime >= prevStopTime.addSecs( 60 * ControlData::Adjust5Min ) ); 
        prevStopTime = stopTime;
    }


    ////////////////////////////////////////
    //
    // test modify job duration
    // FIXME: this only works if we dont pass midnight!
    QVERIFY( dataModel.jobCount() > 2 );
    QTime oldLastJobStart = dataModel.startTimeOfJob( 1 );    
    Types::TimeInterval currentDuration = dataModel.durationOfJob( 0 );
    currentDuration.addHours( 1 );
    dataModel.setDurationOfJobTo( 0 , currentDuration );    
    QVERIFY( dataModel.startTimeOfJob( 1 ) == oldLastJobStart.addSecs( 3600 ) );
    

}

void ModelTests::dataModelTest_1()
{

    DataModel dataModel;

    // dataModel.setAdjustPause( ControlData::AdjustAuto );
    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );

    unsigned int numberOfJobs = dataModel.jobCount();
    
    QVERIFY( numberOfJobs > 0 ); // loaded anything ?
    QVERIFY2( numberOfJobs > 4 , "Sorry, not enough job data to execute this test. This is not a software error!" );

    Job::Spt newJob = Job::createFromTemplate();
    
    Types::ObjectId oldFirstJobId = dataModel.getObjectIdOfJob( 0 );
    Types::ObjectId newJobId = newJob -> getJobObjectId();

    dataModel.addJobBeforeJobAtIndex( 0, newJob );

    QVERIFY((newJobId      = dataModel.getObjectIdOfJob( 0 )));
    QVERIFY((oldFirstJobId = dataModel.getObjectIdOfJob( 1 )));

    // and now we insert in between...

    Types::ObjectId idJob2    = dataModel.getObjectIdOfJob( 2 );
    Types::ObjectId idJob3old = dataModel.getObjectIdOfJob( 3 );
    Types::ObjectId idJob4old = dataModel.getObjectIdOfJob( 4 );
    
    dataModel.addJobBeforeJobAtIndex( 3, newJob );

    QVERIFY( dataModel.getObjectIdOfJob( 2 ) == idJob2   );
    QVERIFY( dataModel.getObjectIdOfJob( 3 ) == newJobId );
    QVERIFY( dataModel.getObjectIdOfJob( 4 ) == idJob3old );
    QVERIFY( dataModel.getObjectIdOfJob( 5 ) == idJob4old );

    // and now for something completly different....
    // test the append function...

    newJob = Job::createFromTemplate();
    newJobId = newJob -> getJobObjectId();

    idJob2    = dataModel.getObjectIdOfJob( 2 );
    idJob3old = dataModel.getObjectIdOfJob( 3 );
    idJob4old = dataModel.getObjectIdOfJob( 4 );

    dataModel.addJobAfterJobAtIndex  ( 3 , newJob );

    QVERIFY( dataModel.getObjectIdOfJob( 2 ) == idJob2    );
    QVERIFY( dataModel.getObjectIdOfJob( 3 ) == idJob3old );
    QVERIFY( dataModel.getObjectIdOfJob( 4 ) == newJobId  );
    QVERIFY( dataModel.getObjectIdOfJob( 5 ) == idJob4old );

    // and now delete stuff...

    Types::ObjectId idDelJob0 = dataModel.getObjectIdOfJob( 0 );
    Types::ObjectId idDelJob2 = dataModel.getObjectIdOfJob( 2 );

    dataModel.deleteJobAtIndex( 1 );
    QVERIFY( dataModel.getObjectIdOfJob( 0 ) == idDelJob0 );
    QVERIFY( dataModel.getObjectIdOfJob( 1 ) == idDelJob2 );    

    dataModel.deleteJobAtIndex( 0 );
    QVERIFY( dataModel.getObjectIdOfJob( 0 ) == idDelJob2 );
    
}


// FIXME move this to Tools
#include "../Gui/valueCheck.cc"

void ModelTests::valueCheck()
{
    // QString doValueCheck( const QString& minValue , const QString& maxValue, const QString& currentInput , bool floatNumberMode = false )


    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "1") ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "001") ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "2") ).compare( QString( "0123" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "3") ).compare( QString( "" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "23") ).compare( QString( "" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "0" ), QString( "23"), QString( "9") ).compare( QString( "" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "1" ) ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "99" ) ).compare( QString( "" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "12" ) ).compare( QString( "0123" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "13" ) ).compare( QString( "" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "03" ) ).compare( QString( "0123456789" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "0003" ) ).compare( QString( "0123456789" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "0" ), QString( "123" ) , QString ( "0003" ) ).compare( QString( "0123456789" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "-2500" ), QString( "2500" ) , QString ( "1" ) ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "-2500" ), QString( "2500" ) , QString ( "2" ) ).compare( QString( "0123456789" ) ) == 0 );

    // float mode...
    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "-99" )        , true ).compare( QString( "." ) ) == 0 );
    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "-50" )        , true ).compare( QString( "." ) ) == 0 );

    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "99" )         , true ).compare( QString( "." ) ) == 0 );
    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "50" )         , true ).compare( QString( "." ) ) == 0 );

    QVERIFY( doValueCheck( QString( "-100.0" ), QString( "100.0" ) , QString( "-99" )        , true ).compare( QString( "." ) ) == 0 );

    QVERIFY( doValueCheck( QString( "-100.0" ), QString( "100.0" ) , QString( "-99." )       , true ).compare( QString( "0123456789" ) ) == 0 );
    
    QVERIFY( doValueCheck( QString( "-100.0" ), QString( "100.0" ) , QString( "-99.9" )      , true ).compare( QString( "0123456789" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "-100.0" ), QString( "100.0" ) , QString( "100" )        , true ).compare( QString( "" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "-100.0" ), QString( "100.0" ) , QString( "-100" )       , true ).compare( QString( "" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "-80" )        , true ).compare( QString( "." ) ) == 0 );
    
    QVERIFY( doValueCheck( QString( "-100" )  , QString( "100" )   , QString( "-80.8888" )   , true ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" )  , QString( "1000" )   , QString( "1" )   , true ).compare( QString( ".0123456789" ) ) == 0 );

    QVERIFY( doValueCheck( QString( "0" )  , QString( "1.5" )   , QString( "1." )   , true ).compare( QString( "01234" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" )  , QString( "1.5" )   , QString( "1.49" )   , true ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" )  , QString( "1.5" )   , QString( "1.4899" )   , true ).compare( QString( "0123456789" ) ) == 0 );
    QVERIFY( doValueCheck( QString( "0" )  , QString( "1.5" )   , QString( "1.5" )   , true ).compare( QString( "" ) ) == 0 );

}


void ModelTests::testParseDuration()
{

    QString t0( " 00 : 00 : 00 " );
    QVERIFY( Types::TimeInterval::parseIntervalString( t0 ).asSeconds() == 0 );

    // must be three parts:
    QString t1( "00:00" );
    bool haveUnparsableException = false;
    try {
        Types::TimeInterval::parseIntervalString( t1 );
    } catch ( const std::runtime_error& e ) {
        #ifdef DEBUG_
        qDebug()  << e.what();
        #endif
        haveUnparsableException = true;
    }
    QVERIFY2( haveUnparsableException , " t1 ");

    haveUnparsableException = false;
    QString t2( "xx12:12:12");
    try {
        Types::TimeInterval::parseIntervalString( t2 );
    } catch ( const std::runtime_error& e ) {
        #ifdef DEBUG_OUTPUT
        qDebug()  << e.what();
        #endif
        haveUnparsableException = true;
    }
    QVERIFY2( haveUnparsableException , " t2 ");


    QString t3( "00 : 00 : 00" );
    QVERIFY( Types::TimeInterval::parseIntervalString( t3 ).asSeconds() == 0 );

    QString t4( "1:0:0" );
    QVERIFY( Types::TimeInterval::parseIntervalString( t4 ).asSeconds() == 3600 );

    QString t5( "01:01:10" );
    QVERIFY( Types::TimeInterval::parseIntervalString( t5 ).asSeconds() == 3670 );

}

void ModelTests::testIntervalCalculation()
{
    using namespace Types;

    TimeInterval i0; // defaults to zero

    i0
        .addHours( 1 )
        .addMinutes( 30 );

    QVERIFY( i0.asSeconds() == 5400 );

    i0
        .setSeconds( 60 )
        .addMinutes( 2 )
        .addSeconds( 1 );

    QVERIFY( i0.asSeconds() == 181 );

    TimeInterval i1;

    i1.setSeconds( 200 );
    QVERIFY( i0 < i1 );
    i0.addSeconds( 19 );
    QVERIFY( not( i0 < i1 ) );
    QVERIFY( i0 == i1 );

    TimeInterval i2;
    i2
        .addHours( 11 )
        .addMinutes( 22 )
        .addSeconds( 33 );

    #ifdef DEBUG_OUTPUT
    qDebug()  << "i2.getHoursRemainder() " << i2.getHoursRemainder();
    qDebug()  << "i2.getMinutesRemainder() " << i2.getMinutesRemainder();
    qDebug()  << "i2.getSecondsRemainder() " << i2.getSecondsRemainder();
    #endif

    QVERIFY( i2.getHoursRemainder() == 11 );
    QVERIFY( i2.getMinutesRemainder() == 22 );
    QVERIFY( i2.getSecondsRemainder() == 33 );

}

void ModelTests::testIndexClass()
{
    Types::Index i0;

    bool haveInvalidAccessException = false;

    try {
        i0.get();
    } catch ( const std::runtime_error& e ) {
        haveInvalidAccessException = true;
    }
    QVERIFY( haveInvalidAccessException );
    
    Types::Index i1( 1 );
    Types::Index i1a( 1 );
    Types::Index i2( 2 );
    Types::Index i3( 3 );

    QVERIFY( i1 < i2 );
    QVERIFY( i2 < i3 );
    
    i3 = i2;

    QVERIFY( i2 == i3 );
    // test prefix
    ++i1;
    QVERIFY( i1 == i2 );
    
    // test postfix
    i1a++;
    QVERIFY( i1a == i1 ); 

}

void ModelTests::dataModelChannelTest_0 () 
{
    DataModel dataModel;

    // dataModel.setAdjustPause( ControlData::AdjustAuto );
    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

    unsigned int numberOfJobs = dataModel.jobCount();

    QVERIFY( numberOfJobs > 4 );

    QStringList dvGain1 = dataModel.allowedDiscreteValuesForGain1AtChannel( 0 );

    QVERIFY( dvGain1.size() == 7 );
    
    // should be: 1, 2, 4, 8, 16, 32, 64
    QVERIFY( dvGain1[ 0 ].compare( "1" )  == 0 );
    QVERIFY( dvGain1[ 1 ].compare( "2" )  == 0 );
    QVERIFY( dvGain1[ 2 ].compare( "4" )  == 0 );
    QVERIFY( dvGain1[ 3 ].compare( "8" )  == 0 );
    QVERIFY( dvGain1[ 4 ].compare( "16" ) == 0 );
    QVERIFY( dvGain1[ 5 ].compare( "32" ) == 0 );
    QVERIFY( dvGain1[ 6 ].compare( "64" ) == 0 );

    // switch model to manual selection, and set an index for it...
    dataModel.setJobSelectionUsesGuiSelectionModel( false ); // we dont have a gui here...
    QVERIFY( dataModel.setGain1ForChannel( 23, QString("Wont work!" )) == false );
    dataModel.setSelectedJobIndex( 0 );

    const QString testValue_0( "TEST_VALUE_0" );
    const QString testValue_1( "TEST_VALUE_1" );
    const QString testValue_2( "TEST_VALUE_2" );
    const QString testValue_3( "TEST_VALUE_3" );
    const QString testValue_4( "TEST_VALUE_4" );
    const QString testValue_5( "TEST_VALUE_5" );
    const QString testValue_6( "TEST_VALUE_6" );

    QVERIFY( dataModel.setExtGainForChannel ( 0, testValue_0 ) ==  true );
    QVERIFY( testValue_0.compare( dataModel.getExtGainForChannel ( 0 ) ) == 0 ); // throws std::runtime_error , std::out_of_range

    QVERIFY( dataModel.setChopperForChannel( 0 , testValue_1 ) == true );
    QVERIFY( testValue_1.compare( dataModel.getChopperForChannel( 0 ) ) == 0 );

    QVERIFY( dataModel.setGain1ForChannel( 0, testValue_2 ) == true );
    QVERIFY( testValue_2.compare( dataModel.getGain1ForChannel( 0 ) ) == 0 );

    QVERIFY( dataModel.setDacForChannel( 0, testValue_3 ) == true );
    QVERIFY( testValue_3.compare( dataModel.getDacForChannel( 0 ) ) == 0 );

    QVERIFY( dataModel.setGain2ForChannel( 0, testValue_4 ) == true );
    QVERIFY( testValue_4.compare( dataModel.getGain2ForChannel( 0 ) ) == 0 );

    QVERIFY( dataModel.setRfFilterForChannel( 0 , testValue_5 ) == true );
    QVERIFY( testValue_5.compare( dataModel.getRfFilterForChannel ( 0 ) ) == 0 );

    QVERIFY( dataModel.setLpHpFilterForChannel( 0 , testValue_6 ) == true );
    QVERIFY( testValue_6.compare( dataModel.getLpHpFilterForChannel ( 0 ) ) == 0 );

}

// void ModelTests::sensorChannelConfigFromJobTest() // go to hw tests?
// {

//     DataModel dataModel;

//     // dataModel.setAdjustPause( ControlData::AdjustAuto );
//     dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
//     dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
//     dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

//     unsigned int numberOfJobs = dataModel.jobCount();

//     QVERIFY( numberOfJobs == 13);

//     Job::Spt testJob = dataModel.getJobAt( 0 ); // this is our test job.

//     SensorChannelConfig::Spt scc = SensorChannelConfig::createFromJob( testJob.data() );

//     // JobChannelConfig* cfg = testJob -> getJobChannelConfigForChannel( 0 ); ??

//     // we expect: 
//     // Channel  number : 0
//     // Selected input  : 1
//     // Input Zero : Undefined
//     // Input one  : Type: RHz Sensor : SHFT03 Config : NOT YET IMPLEMENTED!
//     QVERIFY( scc -> getSelectedInputAtChannel( 0 )    .compare( "1" )      == 0 );    
//     QVERIFY( scc -> getChannelTypeInputAtChannel( 0 ) .compare( "RHz" )    == 0 );    
//     QVERIFY( scc -> getSensorTypeInputAtChannel( 0 )  .compare( "SHFT03" ) == 0 );    
    
// }

void ModelTests::sensorChannelConfigFromJoblistTest()
{
    DataModel dataModel;

    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

    unsigned int numberOfJobs = dataModel.jobCount();

    QVERIFY( numberOfJobs == 13);

    std::vector<Job*> jobList;

    for ( unsigned int i = 0; i < numberOfJobs; ++i )
    {
        jobList.push_back( dataModel.getJobAt( i ).data() );
    }

    SensorChannelConfig::Spt scc = SensorChannelConfig::createFromJobList( jobList );

    QVERIFY( not scc.isNull() );

//    std::cout << scc -> toStdString() << std::endl;

}


void ModelTests::sensorChannelConfigFromHwDbTest()
{

    // NOTE: we clould do this test without using the datamodel (it acts as a factory here),
    // but we use it to make sure we test on the same data...
    // test what you fly... fly what you test...
    DataModel dataModel;

    // dataModel.setAdjustPause( ControlData::AdjustAuto );
    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

    SensorChannelConfig::Spt scc = SensorChannelConfig::createFromHwDb( dataModel.getHwConfig() );

    QVERIFY( not scc.isNull() );

    // no useable info at channel 0
    // 04092013: change accoring to https://rs0.yasc.de/trac/metronix/ticket/30 
    // QVERIFY( scc -> getSelectedInputAtChannel( 0 )    .compare( DefaultValues::Off ) == 0 );    
    QVERIFY( scc -> getSelectedInputAtChannel( 0 )    .compare( DefaultValues::Zero ) == 0 );    
    // std::cout << scc -> toStdString() << std::endl;
    // QVERIFY( scc -> getChannelTypeInputAtChannel( 0 ) .compare( DefaultValues::Undefined ) == 0 );  // should return default : Ex
    QVERIFY( scc -> getChannelTypeInputAtChannel( 0 ) .compare( DefaultValues::DefaultChannelType ) == 0 );  // should return default : Ex
    // QVERIFY( scc -> getSensorTypeInputAtChannel( 0 )  .compare( DefaultValues::Undefined ) == 0 );  // should return default : EFP06  
    QVERIFY( scc -> getSensorTypeInputAtChannel( 0 )  .compare( DefaultValues::DefaultSensor ) == 0 );  // should return default : EFP06  

    // a little bit useable info at channel 5
    // QVERIFY( scc -> getSelectedInputAtChannel( 5 )    .compare( DefaultValues::Off ) == 0 );    
    QVERIFY( scc -> getSelectedInputAtChannel( 5 )    .compare( DefaultValues::Zero ) == 0 ); // should return the new default "input zero"
    // QVERIFY( scc -> getChannelTypeInputAtChannel( 5 ) .compare( DefaultValues::Undefined ) == 0 ); 
    QVERIFY( scc -> getChannelTypeInputAtChannel( 5 ) .compare( DefaultValues::DefaultChannelType ) == 0 ); // should return the new default sensor type.

    // no input selected yet, so we dont know what data to hand out..
    // QVERIFY( scc -> getSensorTypeInputAtChannel( 5 )  .compare( DefaultValues::Undefined ) == 0 );  // old behaviour
    QVERIFY( scc -> getSensorTypeInputAtChannel( 5 )  .compare( DefaultValues::MFS06e ) == 0 ); // by default, now input channel 0 is selected!

    // selected input 0:
    scc -> setHWSelectedInputAtChannel( 5 , "0" );
    QVERIFY( scc -> getSensorTypeInputAtChannel( 5 )  .compare( DefaultValues::MFS06e ) == 0 );    
    // selected input 1: ( same stuff as in 0 here..! )
    scc -> setHWSelectedInputAtChannel( 5 , "1" );
    QVERIFY( scc -> getSensorTypeInputAtChannel( 5 )  .compare( DefaultValues::MFS06e ) == 0 );    

}

void ModelTests::sensorChannelConfigDataModelPartTest()
{

    DataModel dataModel;

    // dataModel.setAdjustPause( ControlData::AdjustAuto );
    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

    unsigned int numberOfJobs = dataModel.jobCount();

    QVERIFY( numberOfJobs == 13);

    // std::cout << "********************************************************************************" << std::endl;
    // std::cout << dataModel.sensorConfigToStdString() << std::endl;
    // std::cout << "********************************************************************************" << std::endl;
    
    // we should other data than above, since we init from job 0, not from 12....
    QVERIFY( dataModel.getSensorHWSelectedInputAtChannel    ( 0 ).compare( "1" )      == 0 );     
    QVERIFY( dataModel.getSensorHWChannelTypeInputAtChannel ( 0 ).compare( "RHz" )    == 0 );    
    QVERIFY( dataModel.getSensorHWSensorTypeInputAtChannel  ( 0 ).compare( "SHFT03" ) == 0 );    

    dataModel.setSensorHWSelectedInputAtChannel    ( 0 , "0" ) ;
    dataModel.setSensorHWChannelTypeInputAtChannel ( 0 , "ChannelType" ) ; 
    dataModel.setSensorHWSensorTypeInputAtChannel  ( 0 , "SensorType" ) ; 
    dataModel.setSensorHWConfigInputAtChannel      ( 0 , "SensorConfig" ) ; 
    
    // std::cout << "********************************************************************************" << std::endl;
    // std::cout << dataModel.sensorConfigToStdString() << std::endl;
    // std::cout << "********************************************************************************" << std::endl;
    
    QVERIFY( dataModel.getSensorHWSelectedInputAtChannel    ( 0 ).compare( "0" )            == 0 );     
    // ignore illegal values
    QVERIFY( dataModel.getSensorHWChannelTypeInputAtChannel ( 0 ).compare( "ChannelType" )  != 0 );    
    QVERIFY( dataModel.getSensorHWSensorTypeInputAtChannel  ( 0 ).compare( "SensorType"  )  != 0 );    
    // this is not checked till now
    QVERIFY( dataModel.getSensorHWConfigInputAtChannel      ( 0 ).compare( "SensorConfig" ) == 0 );

    dataModel.setSensorHWSelectedInputAtChannel    ( 0 , "0" ) ;
    dataModel.setSensorHWChannelTypeInputAtChannel ( 0 , "RHz" ) ; 
    dataModel.setSensorHWSensorTypeInputAtChannel  ( 0 , "SHFT03" ) ; 

    QVERIFY( dataModel.getSensorHWSelectedInputAtChannel    ( 0 ).compare( "0" )            == 0 );     
    QVERIFY( dataModel.getSensorHWChannelTypeInputAtChannel ( 0 ).compare( "RHz" )          == 0 );    
    QVERIFY( dataModel.getSensorHWSensorTypeInputAtChannel  ( 0 ).compare( "SHFT03" )       == 0 );    
    QVERIFY( dataModel.getSensorHWConfigInputAtChannel      ( 0 ).compare( "SensorConfig" ) == 0 );

    // switch back to input channel 1 (as configured in xml)
    dataModel.setSensorHWSelectedInputAtChannel    ( 0 , "1" ) ;
    QVERIFY( dataModel.getSensorHWPosX1    ( 0 ).compare( "0" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosY1    ( 0 ).compare( "1" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ1    ( 0 ).compare( "2" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosX2    ( 0 ).compare( "3" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosY2    ( 0 ).compare( "4" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ2    ( 0 ).compare( "5" )            == 0 );     

    QVERIFY( dataModel.getSensorHWPosX1    ( 5 ).compare( "01" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosY1    ( 5 ).compare( "02" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ1    ( 5 ).compare( "03" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosX2    ( 5 ).compare( "04" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosY2    ( 5 ).compare( "05" )            == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ2    ( 5 ).compare( "06" )            == 0 );     

    dataModel.setSensorHWPosX1    ( 5 , "X1" );
    dataModel.setSensorHWPosY1    ( 5 , "Y1" );
    dataModel.setSensorHWPosZ1    ( 5 , "Z1" );
    dataModel.setSensorHWPosX2    ( 5 , "X2" );
    dataModel.setSensorHWPosY2    ( 5 , "Y2" );
    dataModel.setSensorHWPosZ2    ( 5 , "Z2" );

    QVERIFY( dataModel.getSensorHWPosX1 ( 5 ).compare( "X1" ) == 0 );     
    QVERIFY( dataModel.getSensorHWPosY1 ( 5 ).compare( "Y1" ) == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ1 ( 5 ).compare( "Z1" ) == 0 );     
    QVERIFY( dataModel.getSensorHWPosX2 ( 5 ).compare( "X2" ) == 0 );     
    QVERIFY( dataModel.getSensorHWPosY2 ( 5 ).compare( "Y2" ) == 0 );     
    QVERIFY( dataModel.getSensorHWPosZ2 ( 5 ).compare( "Z2" ) == 0 );     

}

void ModelTests::cloneJobTest() 
{
    // this tests cloning a job file, and, as a side effect, if we can read the xml files we write.
    // declaration of variables
    QString     qstrTmp;
    double      dTmp;
    DataModel   dataModel;

    // dataModel.setAdjustPause( ControlData::AdjustAuto );
    dataModel.setJobDirectory( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07" );
    dataModel.loadAndParseJobListFile( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf/ADUConf.xml" );
    dataModel.loadAndParseHwData( BuildManifest::ProjectRootAsQString + "/Tests/testData/ADU-07/ADUConf" );

    unsigned int numberOfJobs = dataModel.jobCount();

    QVERIFY( numberOfJobs == 13);

    // job 0 should be out testjob0.xml!

    Job::Spt testJob = dataModel.getJobAt( 0 );
    QVERIFY( not testJob.isNull() );

    Types::ObjectId testJobId = testJob -> getJobObjectId();
    Job::Spt copyOfTestJob    = Job::deepCopy( testJob );

    QVERIFY( not copyOfTestJob.isNull() );
    QVERIFY( not ( copyOfTestJob -> getJobObjectId() == testJobId ) );
    
    copyOfTestJob -> setFrequency( 999 );
    QVERIFY( not ( copyOfTestJob -> getFrequency(qstrTmp, dTmp) == testJob -> getFrequency(qstrTmp, dTmp)  ));

}


// see qtest.h
int main(int argc, char *argv[]) 
{ 
    #ifdef DISABLE_KEYPAD_NAVIGATION
    QApplication::setNavigationMode(Qt::NavigationModeNone);
    #endif
    QCoreApplication app(argc, argv); 
    ModelTests testObject; 
    testObject.mTestDataDir = QDir( "/home/kliemt/src/projects/metronix-JLE/JLE/Tests/testData/" );
    return QTest::qExec(&testObject, argc, argv); 
}

