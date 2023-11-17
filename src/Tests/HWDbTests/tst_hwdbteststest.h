#ifndef _TST_HWDBTESTSTEST_H_
#define _TST_HWDBTESTSTEST_H_

#include "hwdatabase.h"

#include "tst_hwdbteststest.h"

#include <QString>
#include <QtTest>
#include <QFile>





class HWDbTestsTest : public QObject
{
        Q_OBJECT
        
    public:
        HWDbTestsTest();
        
    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

    void testHWDB_0();
    void testADUHardwareDescription_0();
    void testADUHardwareDescription_1();
    void testADUHardwareDescription_2();

};

#endif /* _TST_HWDBTESTSTEST_H_ */
