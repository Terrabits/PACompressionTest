#ifndef SAFEFREQUENCYSWEEPTEST_H
#define SAFEFREQUENCYSWEEPTEST_H


// Project
#include "MeasurementData.h"

// RsaToolbox
#include <qcustomplot.h>

// Qt
#include <QDir>
#include <QObject>


class SafeFrequencySweepTest : public QObject
{
    Q_OBJECT
public:
    SafeFrequencySweepTest();
    ~SafeFrequencySweepTest();

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

#endif // SAFEFREQUENCYSWEEPTEST_H
