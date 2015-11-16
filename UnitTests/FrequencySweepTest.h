#ifndef FREQUENCYSWEEPTEST_H
#define FREQUENCYSWEEPTEST_H


// Project
#include "MeasurementData.h"

// RsaToolbox
#include <qcustomplot.h>

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

    bool generatePlot(MeasurementData *results, const QString &filename);

};

#endif // FREQUENCYSWEEPTEST_H
