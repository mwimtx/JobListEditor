#ifndef _TST_MODELTESTS_H_
#define _TST_MODELTESTS_H_

#include "domelementwalker.h"
#include "datamodel.h"

#include <stdexcept>

#include <QString>
#include <QtTest>
#include <QCoreApplication>

class ModelTests : public QObject
{
    Q_OBJECT
        
public:
    ModelTests();

// init() will be called before each testfunction is executed.
// cleanup() will be called after every testfunction.

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void domWalkerTest();
    void domWalkerIteratorTest();
    void emptyJobTest();
    void dataModelTest_0();
    void dataModelTest_1();
    void valueCheck();
    void testParseDuration();
    void testIntervalCalculation();
    void testIndexClass();
    void dataModelChannelTest_0 ();
    // void sensorChannelConfigFromJobTest(); 
    void sensorChannelConfigFromJoblistTest(); 
    void sensorChannelConfigFromHwDbTest(); 
    void sensorChannelConfigDataModelPartTest();
    void cloneJobTest();

public:
    QDir mTestDataDir;
private:
    QDomDocument mDomDoc;

    bool loadMeasurementFile();
    bool loadJobListFile();
        
    bool loadXmlFile( const QString&  ) ;

};



#endif /* _TST_MODELTESTS_H_ */
