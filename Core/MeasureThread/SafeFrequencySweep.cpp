#include "SafeFrequencySweep.h"

// logging
#include "logging.hpp"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>


SafeFrequencySweep::SafeFrequencySweep(QObject *parent) :
    MeasureThread(parent)
{

}

SafeFrequencySweep::~SafeFrequencySweep()
{

}

void SafeFrequencySweep::run() {
    LOG(info) << "starting SafeFrequencySweep";
    LOG(info) << "initializing vna";
    if (!prepareVna()) {
      LOG(error) << "vna initialization failed";
      return;
    }


    emit progress(0);

    LOG(info) << "initializing for measurement";

    // Frequency
    const double start_Hz = _settings.startFrequency_Hz();
    const double stop_Hz = _settings.stopFrequency_Hz();
    const uint freqPoints = _settings.frequencyPoints();
    QRowVector sweptFreq_Hz = linearSpacing(start_Hz, stop_Hz, freqPoints);

    // Power
    const double start_dBm = _settings.startPower_dBm();
    const double stop_dBm = _settings.stopPower_dBm();
    const uint powerPoints = _settings.powerPoints();
    QRowVector pin_dBm = linearSpacing(start_dBm, stop_dBm, powerPoints);

    const double compressionLevel_dB = _settings.compressionLevel_dB();
    const bool isGainExpansion = _settings.isGainExpansion();

    // Channel, ports
    const uint channel = _measurementChannel;
    const uint outputPort = _settings.outputPort();
    const uint inputPort = _settings.inputPort();

    const bool shouldFlipPorts = outputPort < inputPort;


    LOG(info) << "configuring VNA channels";
    _vna->channel(channel).setFrequencies(sweptFreq_Hz);
    sweptFreq_Hz = _vna->channel(channel).segmentedSweep().frequencies_Hz(); // <------- Will this fix the index problem?
    _results->frequencies_Hz() = sweptFreq_Hz;

    LOG(info) << "configure VNA traces";
    QString a1Trace = _vna->createTrace(channel);
    _vna->trace(a1Trace).setWaveQuantity(WaveQuantity::a, inputPort, inputPort);

    // Handle interrupt
    if (isInterruptionRequested()) {
        LOG(info) << "interrupt requested";
        _results->clearAllData();
        setError("*Measurement cancelled");
        restoreVna();
        return;
    }

    LOG(info) << "performing first sweep";
    uint iPower = 0;
    double power_dBm = pin_dBm[iPower];
    _results->pin_dBm() << power_dBm;

    VnaSegmentedSweep sweep = _vna->channel(channel).segmentedSweep();
    sweep.setPower(power_dBm);
    _vna->channel(channel).manualSweepOn();
    emit startingSweep(QString("Sweep %1").arg(iPower+1), sweep.sweepTime_ms());
    NetworkData data = sweep.measure(outputPort, inputPort);
    if (data.empty()) {
      LOG(error) << "first sweep failed";
      return;
    }

    LOG(info) << "first sweep complete";

    if (data.points() == 0) {
        LOG(error) << "sweep failed";
        emit finishedSweep();
        _results->clearAllData();
        setError("*Could not perform sweep.");
        restoreVna();
        return;
    }

    LOG(info) << "processing first sweep";
    _results->data() << data;
    QRowVector measuredPin_dBm;
    _vna->trace(a1Trace).y(measuredPin_dBm);
    _results->measuredPin_dBm() << measuredPin_dBm;
    emit finishedSweep();

    if (shouldFlipPorts)
        flipPorts(_results->data()[iPower]);

    _results->powerInAtMaxGain_dBm() = measuredPin_dBm;
    _results->maxGain_dB() = _results->data()[iPower].y_dB(2, 1);
    _results->sParametersAtMaxGain() = _results->data()[iPower].y();
    _results->powerOutAtMaxGain_dBm() = add(_results->powerInAtMaxGain_dBm(), _results->maxGain_dB());

    _results->powerInAtCompression_dBm() = _results->powerInAtMaxGain_dBm();
    _results->gainAtCompression_dB() = _results->maxGain_dB();
    _results->sParametersAtCompression() = _results->sParametersAtMaxGain();
    _results->powerOutAtCompression_dBm() = _results->powerOutAtMaxGain_dBm();

    emit progress(int((100.0 * (iPower+1))/powerPoints));
    emit plotMaxGain(_results->frequencies_Hz(), _results->maxGain_dB());
    emit plotPinAtCompression(_results->frequencies_Hz(), _results->powerInAtCompression_dBm());

    for (iPower = 1; iPower < powerPoints; iPower++) {
        if (sweptFreq_Hz.isEmpty())
            break;

        // log sweep
        QString message;
        message  = "performing sweep %1";
        message  = message.arg(iPower + 1);
        QByteArray bytes;
        bytes    = message.toUtf8();
        LOG(info) << bytes.constData();

        power_dBm = pin_dBm[iPower];
        _results->pin_dBm() << power_dBm;

        if (isInterruptionRequested()) {
            LOG(info) << "interrupt requested";
            _results->clearAllData();
            setError("*Measurement cancelled");
            restoreVna();
            return;
        }

        // perform sweep
        emit startingSweep(QString("Sweep %1").arg(iPower+1), sweep.sweepTime_ms());
        sweep.setPower(power_dBm);
        data = sweep.measure(outputPort, inputPort);

        // log sweep complete
        message = "completed sweep %1";
        message = message.arg(iPower + 1);
        bytes   = message.toUtf8();
        LOG(info) << bytes.constData();

        if (data.points() == 0) {
            LOG(error) << "sweep failed";
            emit finishedSweep();
            _results->clearAllData();
            setError("*Could not perform sweep.");
            restoreVna();
            return;
        }

        // log data processing
        message = "processing sweep %1 data";
        message = message.arg(iPower + 1);
        bytes   = message.toUtf8();
        LOG(info) << bytes.constData();

        _results->data() << data;
        _vna->trace(a1Trace).y(measuredPin_dBm);
        _results->measuredPin_dBm() << measuredPin_dBm;
        emit finishedSweep();

        if (shouldFlipPorts)
            flipPorts(_results->data()[iPower]);

//        const double previousPower_dBm = powers_dBm[iPower-1];
        QRowVector previousMeasuredPowers_dBm = _results->measuredPin_dBm()[iPower-1]; // NEW
        QRowVector previousFrequencies_Hz = _results->data()[iPower-1].x();
        QRowVector previousGains_dB = _results->data()[iPower-1].y_dB(2,1);
        ComplexMatrix3D previousSParams = _results->data()[iPower-1].y();

        // QRowVector measuredPowers_dBm = _results->measuredPowers_dBm[iPower];
        QRowVector gains_dB = _results->data()[iPower].y_dB(2, 1);
        ComplexMatrix3D sParams = _results->data()[iPower].y();

        for (int iCurrentFreq = 0; iCurrentFreq < sweptFreq_Hz.size(); iCurrentFreq++) {
            const double measuredPower_dBm = measuredPin_dBm[iCurrentFreq];
            const double freq_Hz = sweptFreq_Hz[iCurrentFreq];
            const double gain_dB = gains_dB[iCurrentFreq];
            const ComplexMatrix2D sParam = sParams[iCurrentFreq];

            const int iPrevFreq = previousFrequencies_Hz.indexOf(freq_Hz);
            const double previousMeasuredPower_dBm = previousMeasuredPowers_dBm[iPrevFreq]; // NEW
            const double previousGain_dB = previousGains_dB[iPrevFreq];
            const ComplexMatrix2D previousSParam = previousSParams[iPrevFreq];

            const int iTotalFreq = _results->frequencies_Hz().indexOf(freq_Hz);

            // Check for Max Gain
            if (isGainExpansion) {
                if (_results->maxGain_dB()[iTotalFreq] < gain_dB) {
                    _results->powerInAtMaxGain_dBm()[iTotalFreq] = measuredPower_dBm;
                    _results->maxGain_dB()[iTotalFreq] = gain_dB;
                    _results->sParametersAtMaxGain()[iTotalFreq] = sParam;
                    _results->powerOutAtMaxGain_dBm()[iTotalFreq] = measuredPower_dBm + gain_dB;
                }
            }

            // Check for compression
            const double maxGain_dB = _results->maxGain_dB()[iTotalFreq];
            const double compressedGain_dB = maxGain_dB - compressionLevel_dB;
            if (gain_dB <= compressedGain_dB) {
                const double pinCompression_dBm
                        = linearInterpolateX(previousMeasuredPower_dBm, previousGain_dB, // i-1
                                             measuredPower_dBm,         gain_dB, // i
                                                                compressedGain_dB); // Desired Y value

                _results->powerInAtCompression_dBm()[iTotalFreq] = pinCompression_dBm;
                _results->gainAtCompression_dB()[iTotalFreq] = compressedGain_dB;
                _results->sParametersAtCompression()[iTotalFreq] = linearInterpolateYMagPhase(previousMeasuredPower_dBm, previousSParam, measuredPower_dBm, sParam, pinCompression_dBm);
                _results->powerOutAtCompression_dBm()[iTotalFreq] = pinCompression_dBm + compressedGain_dB;

                sweptFreq_Hz.removeAt(iCurrentFreq);
                measuredPin_dBm.removeAt(iCurrentFreq);
                gains_dB.removeAt(iCurrentFreq);
                sParams.erase(sParams.begin() + iCurrentFreq);
                sweep.deleteSegment(iCurrentFreq+1);
                iCurrentFreq--;
            }
            else {
                LOG(warning) << "compression not found";
                // Update progress plot with *closest* (current) value
                _results->powerInAtCompression_dBm()[iTotalFreq] = measuredPower_dBm;
                _results->gainAtCompression_dB()[iTotalFreq] = gain_dB;
                _results->sParametersAtCompression()[iTotalFreq] = sParam;
                _results->powerOutAtCompression_dBm()[iTotalFreq] = measuredPower_dBm + gain_dB;
            }
        }

        emit progress(int((100.0 * (iPower+1))/powerPoints));
        emit plotMaxGain(_results->frequencies_Hz(), _results->maxGain_dB());
        emit plotPinAtCompression(_results->frequencies_Hz(), _results->powerInAtCompression_dBm());
    }

    emit progress(100);
    restoreVna();

    LOG(info) << "sweeps complete";
    LOG(info) << "processing data";

    // Check if any compression points not found
    if (!sweptFreq_Hz.isEmpty()) {
      const QString freqStr = formatValue(sweptFreq_Hz.first(), 3, Units::Hertz);

      // log error
      QString message;
      message = "compression not found for %1";
      message = message.arg(freqStr);
      QByteArray bytes;
      bytes   = message.toUtf8();
      LOG(error) << bytes.constData();

      // show error
      message = "*Could not find compression point for %1";
      message = message.arg(freqStr);
      setError(message);
    }
}
