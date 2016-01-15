#ifndef MEASUREDATATEST_H
#define MEASUREDATATEST_H


// Project
#include "MeasurementData.h"

// Qt
#include <QString>
#include <QObject>


class MeasureDataTest : public QObject
{
    Q_OBJECT
public:
    explicit MeasureDataTest(QObject *parent = 0);
    ~MeasureDataTest();

private slots:
    void init();

    void sParameterVsPin_data();
    void sParameterVsPin();

//    void sParameterVsPout_data();
//    void sParameterVsPout();

//    void sParameterAtCompression_data();
//    void sParameterAtCompression();

//    void sParameterAtMaxGain_data();
//    void sParameterAtMaxGain();

//    void sParameterVsFrequencyAtPin_data();
//    void sParameterVsFrequencyAtPin();

//    void poutVsFrequency_data();
//    void poutVsFrequency();

//    void poutVsPin_data();
//    void poutVsPin();

private:
    static const double startFreq_Hz;
    static const double stopFreq_Hz;
    static const uint   freqPoints;
    static const double startPower_dBm;
    static const double stopPower_dBm;
    static const uint   powerPoints;
    static const double compression_dB;

    static const double nonSquareFreq_Hz;
    static const double measuredPinOffset_dBm;

    static const double match_dB;
    static const double compressedMatch_dB;
    static const RsaToolbox::ComplexDouble match_complex;
    static const RsaToolbox::ComplexDouble compressedMatch_complex;

    static const double maxGain_dB;
    static const double gain_dB;
    static const double compressedGain_dB;
    static const RsaToolbox::ComplexDouble maxGain_complex;
    static const RsaToolbox::ComplexDouble gain_complex;
    static const RsaToolbox::ComplexDouble compressedGain_complex;

    static const double isolation_dB;
    static const double compressedIsolation_dB;
    static const RsaToolbox::ComplexDouble isolation_complex;
    static const RsaToolbox::ComplexDouble compressedIsolation_complex;

    MeasurementData _data;
    void generateSampleData();

};

#endif // MEASUREDATATEST_H
