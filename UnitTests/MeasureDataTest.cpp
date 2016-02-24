#include "MeasureDataTest.h"


// Project
#include "ProcessTrace.h"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QTest>


MeasureDataTest::MeasureDataTest(QObject *parent) :
    QObject(parent)
{
//    QDir sourceDir(SOURCE_DIR);
}
MeasureDataTest::~MeasureDataTest() {

}

void MeasureDataTest::init() {
    generateSampleData();
}

void MeasureDataTest::sParameterVsPin_data() {
    QTest::addColumn<double>("freq_Hz");
    QTest::addColumn<int>("outputPort");
    QTest::addColumn<int>("inputPort");
    QTest::addColumn<QRowVector>("expectedPin_dBm");
    QTest::addColumn<ComplexRowVector>("expectedSParameter");

    QRowVector expectedPin_dBm = linearSpacing(startPower_dBm, stopPower_dBm, powerPoints);
    ComplexRowVector expectedSParameter(powerPoints, match_complex);
    expectedSParameter[powerPoints-1] = compressedMatch_complex;

    //            Test name         freq            out  in   Pin                SParam
    QTest::newRow("S11_at_1GHz") << startFreq_Hz << 1 << 1 << expectedPin_dBm << expectedSParameter;
    QTest::newRow("S11_at_2GHz") << stopFreq_Hz  << 1 << 1 << expectedPin_dBm << expectedSParameter;

    expectedPin_dBm.removeLast();
    expectedSParameter[powerPoints-2] = expectedSParameter[powerPoints-1];
    expectedSParameter.pop_back();
    //            Test name           freq             out     in   Pin                SParam
    QTest::newRow("S11_at_1_5GHz") << nonSquareFreq_Hz << 1 << 1 << expectedPin_dBm << expectedSParameter;


}
void MeasureDataTest::sParameterVsPin() {
    QFETCH(double, freq_Hz);
    QFETCH(int, outputPort);
    QFETCH(int, inputPort);
    QFETCH(QRowVector, expectedPin_dBm);
    QFETCH(ComplexRowVector, expectedSParameter);

    QRowVector pin_dBm;
    ComplexRowVector sParameter;
    _data.sParameterVsPin(freq_Hz, outputPort, inputPort, pin_dBm, sParameter);
    QCOMPARE(pin_dBm.size(), expectedPin_dBm.size());
    QCOMPARE(pin_dBm, expectedPin_dBm);
    QCOMPARE(sParameter.size(), expectedSParameter.size());
    for (uint i = 0; i < sParameter.size(); i++) {
        QCOMPARE(sParameter[i].real(), expectedSParameter[i].real());
        QCOMPARE(sParameter[i].imag(), expectedSParameter[i].imag());
    }
}


void MeasureDataTest::sParameterVsPout() {
    _data.open(QDir(SOURCE_DIR).filePath("MessedUpPlots2.dat"));
    QRowVector pout_dBm;
    ComplexRowVector s21;
    _data.sParameterVsPout(2.1E9, 2, 1, pout_dBm, s21);
    qDebug() << "*** vs Pout:";
    qDebug() << "S21.first: " << pout_dBm.first() << " dBm, " << toDb(s21[0]) << " dB";
    qDebug() << "S21.last:  " << pout_dBm.last() << " dBm, " << toDb(s21[s21.size()-1]) << " dB";
    qDebug() << "points: " << pout_dBm.size();
}


// Data notes:
//             -10 dBm => max gain (10.0 dB)
// middle power points => gain (9.5 dB)
//               0 dBm => compressed gain (9.0 dB)
//
// One exception to keep it interesting:
//   Freq = 1.5 GHz compresses at 9 dBm,
//   measurement stops at compression
//
const double MeasureDataTest::startFreq_Hz    =  1.0E9;
const double MeasureDataTest::stopFreq_Hz     =  2.0E9;
const uint   MeasureDataTest::freqPoints      =  11;
const double MeasureDataTest::startPower_dBm  = -10.0;
const double MeasureDataTest::stopPower_dBm   =   0.0;
const uint   MeasureDataTest::powerPoints     =  11;
const double MeasureDataTest::compression_dB  =   1.0;

const double MeasureDataTest::nonSquareFreq_Hz = 1.5E9;
const double MeasureDataTest::measuredPinOffset_dBm = 0.0;

const double MeasureDataTest::match_dB = -10.0;
const double MeasureDataTest::compressedMatch_dB = -7.0;
const ComplexDouble MeasureDataTest::match_complex = toMagnitude(match_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0));
const ComplexDouble MeasureDataTest::compressedMatch_complex = toMagnitude(compressedMatch_dB) * ComplexDouble(1.0/sqrt(2.0), 1.0/sqrt(2.0));

const double MeasureDataTest::maxGain_dB = 10.0;
const double MeasureDataTest::gain_dB    =  9.5;
const double MeasureDataTest::compressedGain_dB = 9.0;
const ComplexDouble MeasureDataTest::maxGain_complex = toMagnitude(maxGain_dB) * ComplexDouble(-1.0, 0.0);
const ComplexDouble MeasureDataTest::gain_complex = toMagnitude(gain_dB) * ComplexDouble(-1.0, 0.0);
const ComplexDouble MeasureDataTest::compressedGain_complex = toMagnitude(compressedGain_dB) * ComplexDouble(-1.0, 0.0);

const double MeasureDataTest::isolation_dB = -20.0;
const double MeasureDataTest::compressedIsolation_dB = -10.0;
const ComplexDouble MeasureDataTest::isolation_complex = toMagnitude(isolation_dB) * ComplexDouble(-1.0, 0);
const ComplexDouble MeasureDataTest::compressedIsolation_complex = toMagnitude(compressedIsolation_dB) * ComplexDouble(-1.0, 0);

void MeasureDataTest::generateSampleData() {
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

        data[j][0][0] = match_complex; // S11
        data[j][1][0] = maxGain_complex; // S21
        data[j][0][1] = isolation_complex; // S12
        data[j][1][1] = match_complex; // S22
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

            data[j][0][0] = match_complex; // S11
            data[j][1][0] = gain_complex; // S21
            data[j][0][1] = isolation_complex; // S12
            data[j][1][1] = match_complex; // S22
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

        data[j][0][0] = compressedMatch_complex; // S11
        data[j][1][0] = compressedGain_complex; // S21
        data[j][0][1] = compressedIsolation_complex; // S12
        data[j][1][1] = compressedMatch_complex; // S22
    }
    _data.sParametersAtCompression() = data;
    _data.data()[i].setData(_data.frequencies_Hz(), data);

    // Create "interesting" feature at 1.5 GHz:
    //   Compression at -1 dBm instead of 0;
    //   Stop at compression
    int iFeature = _data.frequencies_Hz().indexOf(nonSquareFreq_Hz);
    if (iFeature != -1) {
        _data.powerInAtCompression_dBm()[iFeature] = -1.0;
        _data.powerOutAtCompression_dBm()[iFeature] = -1.0 + compressedGain_dB;
        _data.data()[powerPoints-2].y()[iFeature] = _data.data()[powerPoints-1].y()[iFeature];

        ComplexMatrix3D::iterator iterator = _data.data()[powerPoints-1].y().begin() + iFeature;
        _data.data()[powerPoints-1].x().remove(iFeature);
        _data.data()[powerPoints-1].y().erase(iterator);
    }
}
