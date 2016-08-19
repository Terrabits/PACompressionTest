#ifndef PROCESSTRACETEST_H
#define PROCESSTRACETEST_H


// Project
#include "MeasurementData.h"

// Qt
#include <QString>
#include <QObject>


class ProcessTraceTest : public QObject
{
    Q_OBJECT
public:
    explicit ProcessTraceTest(QObject *parent = 0);
    ~ProcessTraceTest();

private slots:
    void init();

private:
    static const double startFreq_Hz;
    static const double stopFreq_Hz;
    static const uint   freqPoints;
    static const double startPower_dBm;
    static const double stopPower_dBm;
    static const uint   powerPoints;
    static const double compression_dB;

    static const double measuredPinOffset_dBm;

    static const double match_dB;
    static const double compressedMatch_dB;

    static const double maxGain_dB;
    static const double gain_dB;

    static const double isolation_dB;
    static const double compressedIsolation_dB;

    MeasurementData _data;
    void generateSampleData();

};

#endif // PROCESSTRACETEST_H
