#ifndef FREQUENCYSWEEPTEST_H
#define FREQUENCYSWEEPTEST_H


// Qt
#include <QDir>
#include <QObject>


class FrequencySweepTest : public QObject
{
    Q_OBJECT
public:
    FrequencySweepTest();
    ~FrequencySweepTest();

private slots:


    // Once
    void initTestCase();
    void cleanupTestCase();

    // Once per test
    void init();
    void cleanup();

    // Tests
    void sweep();

private:
    QDir _sourceDir;

};

#endif // FREQUENCYSWEEPTEST_H
