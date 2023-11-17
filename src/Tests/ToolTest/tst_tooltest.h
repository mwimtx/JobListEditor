#ifndef _TST_TOOLTEST_H_
#define _TST_TOOLTEST_H_

#include <QObject>
#include <QTest>
#include <QStringList>

class ToolTest : public QObject
{
        Q_OBJECT
        
    public:
        ToolTest();
        
    private Q_SLOTS:
    void initTestCase();
    void testCLIParser();
    void testCodeMapper();
    void testDefaultValues();
    void cleanupTestCase();

public:

};

#endif /* _TST_TOOLTEST_H_ */

