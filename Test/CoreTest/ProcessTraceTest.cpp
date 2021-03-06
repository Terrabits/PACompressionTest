#include "ProcessTraceTest.h"


// Project
#include "ProcessTrace.h"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QTest>


ProcessTraceTest::ProcessTraceTest(QObject *parent) :
    QObject(parent)
{
//    QDir sourceDir(SOURCE_DIR);
}
ProcessTraceTest::~ProcessTraceTest() {

}

void ProcessTraceTest::init() {
    generateSampleData();
}




const double ProcessTraceTest::startFreq_Hz    =  1.0E9;
const double ProcessTraceTest::stopFreq_Hz     =  2.0E9;
const uint   ProcessTraceTest::freqPoints      =  11;
const double ProcessTraceTest::startPower_dBm  = -10.0;
const double ProcessTraceTest::stopPower_dBm   =   0.0;
const uint   ProcessTraceTest::powerPoints     =  11;
const double ProcessTraceTest::compression_dB  =   1.0;

const double ProcessTraceTest::measuredPinOffset_dBm = 0.0;

const double ProcessTraceTest::match_dB = -10.0;
const double ProcessTraceTest::compressedMatch_dB = -7.0;

const double ProcessTraceTest::maxGain_dB = 10.0;
const double ProcessTraceTest::gain_dB    =  9.5;

const double ProcessTraceTest::isolation_dB = -20.0;
const double ProcessTraceTest::compressedIsolation_dB = -10.0;

void ProcessTraceTest::generateSampleData() {
    // Data notes:
    //             -10 dBm => max gain (10.0 dB)
    // middle power points => gain (9.5 dB)
    //               0 dBm => compressed gain (9.0 dB)
    //
    // One exception to keep it interesting:
    //   Freq = 1.5 GHz compresses at 9 dBm,
    //   measurement stops at compression
    //

    MeasurementSettings settings;
    settings.setStartFrequency(startFreq_Hz);
    settings.setStopFrequency(stopFreq_Hz);
    settings.setFrequencyPoints(freqPoints);
    settings.setStartPower(startPower_dBm);
    settings.setStopPower(stopPower_dBm);
    settings.setPowerPoints(powerPoints);
    settings.setCompressionLevel(compression_dB);
    settings.setStopAtCompression(true);

    _data.clearAllData();
    _data.setSettings(settings);
    _data.frequencies_Hz() = linearSpacing(startFreq_Hz, stopFreq_Hz, freqPoints);
    _data.pin_dBm() = linearSpacing(startPower_dBm, stopPower_dBm, powerPoints);

    _data.powerInAtMaxGain_dBm() = QRowVector(freqPoints, startPower_dBm);
    _data.maxGain_dB() = QRowVector(freqPoints, maxGain_dB);
    _data.powerOutAtMaxGain_dBm() = QRowVector(freqPoints, startPower_dBm + maxGain_dB);
//    _data.sParametersAtMaxGain() = ...

    _data.powerInAtCompression_dBm() = QRowVector(freqPoints, stopPower_dBm);
    _data.gainAtCompression_dB() = QRowVector(freqPoints, maxGain_dB - compression_dB);
    _data.powerOutAtCompression_dBm() = QRowVector(freqPoints, stopPower_dBm + maxGain_dB - compression_dB);
//    _data.sParametersAtCompression() = ...

    _data.measuredPin_dBm().resize(powerPoints);
    _data.data().resize(powerPoints);

    // i = first (max gain)
    uint i = 0;
    double measuredPin_dBm = _data.pin_dBm()[i] + measuredPinOffset_dBm;
    _data.measuredPin_dBm()[i] = QRowVector(freqPoints, measuredPin_dBm);

    ComplexMatrix3D data(freqPoints);
    for (uint j = 0; j < freqPoints; j++) {
        data[j].resize(2);
        data[j][0].resize(2);
        data[j][1].resize(2);

        data[j][0][0] = toMagnitude(match_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S11 (45 deg)
        data[j][1][0] = toMagnitude(maxGain_dB) * ComplexDouble(-1.0, 0.0); // S21 (180 deg)
        data[j][0][1] = toMagnitude(isolation_dB) * ComplexDouble(-1.0, 0); // S12 (180 deg)
        data[j][1][1] = toMagnitude(match_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S22 (45 deg)
    }
    _data.sParametersAtMaxGain() = data;
    _data.data()[i].setData(_data.frequencies_Hz(), data);

    // i = 1:powerPoints-1
    for (i = 1; i < powerPoints-1; i++) {
        measuredPin_dBm = _data.pin_dBm()[i] + measuredPinOffset_dBm;
        _data.measuredPin_dBm()[i] = QRowVector(freqPoints, measuredPin_dBm);

        data = ComplexMatrix3D(freqPoints);
        for (uint j = 0; j < freqPoints; j++) {
            data[j].resize(2);
            data[j][0].resize(2);
            data[j][1].resize(2);

            data[j][0][0] = toMagnitude(match_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S11
            data[j][1][0] = toMagnitude(gain_dB) * ComplexDouble(-1.0, 0.0); // S21
            data[j][0][1] = toMagnitude(isolation_dB) * ComplexDouble(-1.0, 0); // S12
            data[j][1][1] = toMagnitude(match_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S22
        }
        _data.data()[i].setData(_data.frequencies_Hz(), data);
    }

    // i = last (compression)
    i = powerPoints-1;
    measuredPin_dBm = _data.pin_dBm()[i] + measuredPinOffset_dBm;
    _data.measuredPin_dBm()[i] = QRowVector(freqPoints, measuredPin_dBm);

    data = ComplexMatrix3D(freqPoints);
    for (uint j = 0; j < freqPoints; j++) {
        data[j].resize(2);
        data[j][0].resize(2);
        data[j][1].resize(2);

        data[j][0][0] = toMagnitude(compressedMatch_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S11
        data[j][1][0] = toMagnitude(maxGain_dB - compression_dB) * ComplexDouble(-1.0, 0.0); // S21
        data[j][0][1] = toMagnitude(compressedIsolation_dB) * ComplexDouble(-1.0, 0); // S12
        data[j][1][1] = toMagnitude(compressedMatch_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0)); // S22
    }
    _data.sParametersAtCompression() = data;
    _data.data()[i].setData(_data.frequencies_Hz(), data);

    // Create "interesting" feature at 1.5 GHz:
    //   Compression at -1 dBm instead of 0;
    //   Stop at compression
    int iFeature = _data.frequencies_Hz().indexOf(1.5E9);
    if (iFeature != -1) {
        _data.powerInAtCompression_dBm()[iFeature] = -1.0;
        _data.powerOutAtCompression_dBm()[iFeature] = -1.0 + maxGain_dB - compression_dB;
        _data.data()[powerPoints-2].y()[iFeature] = _data.data()[powerPoints-1].y()[iFeature];

        ComplexMatrix3D::iterator iterator = _data.data()[powerPoints-1].y().begin() + iFeature;
        _data.data()[powerPoints-1].x().remove(iFeature);
        _data.data()[powerPoints-1].y().erase(iterator);
    }
}
