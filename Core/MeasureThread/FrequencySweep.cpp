#include "FrequencySweep.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QBitArray>
#include <QDebug>


FrequencySweep::FrequencySweep(QObject *parent)
    : MeasureThread(parent)
{

}

FrequencySweep::~FrequencySweep()
{

}

void FrequencySweep::run() {
    QString msg;
    if (!_settings.isValid(*_vna, msg)) {
        setError(msg);
        _vna->settings().displayOn();
        return;
    }

    _vna->isError();
    _vna->clearStatus();
    _vna->settings().displayOff();

    emit progress(0);

    // Frequency
    const double start_Hz = _settings.startFrequency_Hz();
    const double stop_Hz = _settings.stopFrequency_Hz();
    const uint freqPoints = _settings.frequencyPoints();
    QRowVector sweptFreq_Hz = linearSpacing(start_Hz, stop_Hz, freqPoints);

    // Power
    const double start_dBm = _settings.startPower_dBm();
    const double stop_dBm = _settings.stopPower_dBm();
    const uint powerPoints = _settings.powerPoints();
    QRowVector powers_dBm = linearSpacing(start_dBm, stop_dBm, powerPoints);

    const double compressionLevel_dB = _settings.compressionLevel_dB();
    const bool isGainExpansion = _settings.isGainExpansion();

    // Channel, ports
    const uint channel = _settings.channel();
    const uint outputPort = _settings.outputPort();
    const uint inputPort = _settings.inputPort();

    const bool shouldFlipPorts = outputPort < inputPort;

    freezeChannels();
    _vna->settings().rfOutputPowerOn();

    // Setup channel
    _vna->channel(channel).select();
    uint c = _vna->createChannel();

    _vna->channel(c).setFrequencies(sweptFreq_Hz);
    _results->frequencies_Hz() = sweptFreq_Hz;

    // First point
    uint iPower = 0;
    double power_dBm = powers_dBm[iPower];
    _results->powers_dBm() << power_dBm;

    VnaSegmentedSweep sweep = _vna->channel(c).segmentedSweep();
    sweep.setPower(power_dBm);

    if (isInterruptionRequested()) {
        setError("*Measurement aborted");
        _results->clearAllData();
        _vna->deleteChannel(c);
        _vna->settings().displayOn();
        return;
    }
    _vna->channel(c).manualSweepOn();
    emit startingSweep(QString("Sweep %1").arg(iPower+1), sweep.sweepTime_ms());
    _results->data() << sweep.measure(outputPort, inputPort);
    emit finishedSweep();

    if (shouldFlipPorts)
        flipPorts(_results->data()[iPower]);

    _results->powerInAtMaxGain_dBm() = QRowVector(freqPoints, power_dBm);
    _results->maxGain_dB() = _results->data()[iPower].y_dB(2, 1);
    _results->powerOutAtMaxGain_dBm() = add(_results->powerInAtMaxGain_dBm(), _results->maxGain_dB());

    _results->powerInAtCompression_dBm() = _results->powerInAtMaxGain_dBm();
    _results->gainAtCompression_dB() = _results->maxGain_dB();
    _results->powerOutAtCompression_dBm() = _results->powerOutAtMaxGain_dBm();

    emit plotMaxGain(_results->frequencies_Hz(), _results->maxGain_dB());
    emit plotPinAtCompression(_results->frequencies_Hz(), _results->powerInAtCompression_dBm());

    QBitArray isCompression(freqPoints, false);
    for (iPower = 1; iPower < powerPoints; iPower++) {
        emit progress(int((100.0 * iPower)/powerPoints));

        power_dBm = powers_dBm[iPower];
        _results->powers_dBm() << power_dBm;
        sweep.setPower(power_dBm);

        if (isInterruptionRequested()) {
            setError("*Measurement cancelled");
            _results->clearAllData();
            _vna->deleteChannel(c);
            _vna->settings().displayOn();
            return;
        }
        emit startingSweep(QString("Sweep %1").arg(iPower+1), sweep.sweepTime_ms());
        _results->data() << sweep.measure(outputPort, inputPort);
        emit finishedSweep();

        if (shouldFlipPorts)
            flipPorts(_results->data()[iPower]);

        const double previousPower_dBm = powers_dBm[iPower-1];
        QRowVector previousGains_dB = _results->data()[iPower-1].y_dB(2, 1);
        QRowVector gains_dB = _results->data()[iPower].y_dB(2, 1);
        for (uint iFreq = 0; iFreq < freqPoints; iFreq++) {
            const double previousGain_dB = previousGains_dB[iFreq];
            const double gain_dB = gains_dB[iFreq];

            // Check for Max Gain
            if (isGainExpansion) {
                if (_results->maxGain_dB()[iFreq] < gain_dB) {
                    _results->powerInAtMaxGain_dBm()[iFreq] = power_dBm;
                    _results->maxGain_dB()[iFreq] = gain_dB;
                    _results->powerOutAtMaxGain_dBm()[iFreq] = power_dBm + gain_dB;
                    isCompression[iFreq] = false;
                }
            }

            // Check for compression
            const double maxGain_dB = _results->maxGain_dB()[iFreq];
            const double compressedGain_dB = maxGain_dB - compressionLevel_dB;
            if (!isCompression[iFreq]) {
                if (gain_dB <= compressedGain_dB) {
                    // Compression found
                    const double pinCompression_dBm
                            = linearInterpolateX(previousPower_dBm, previousGain_dB, // i-1
                                                 power_dBm,         gain_dB, // i
                                                 compressedGain_dB); // Desired Y value

                    _results->powerInAtCompression_dBm()[iFreq] = pinCompression_dBm;
                    _results->gainAtCompression_dB()[iFreq] = compressedGain_dB;
                    _results->powerOutAtCompression_dBm()[iFreq] = pinCompression_dBm + compressedGain_dB;
                    isCompression[iFreq] = true;
                }
                else {
                    // Compression not found
                    // Update progress plot with *closest* (current) value
                    _results->powerInAtCompression_dBm()[iFreq] = power_dBm;
                    _results->gainAtCompression_dB()[iFreq] = gain_dB;
                    _results->powerOutAtCompression_dBm()[iFreq] = power_dBm + gain_dB;
                }
            }
        }
        emit plotMaxGain(_results->frequencies_Hz(), _results->maxGain_dB());
        emit plotPinAtCompression(_results->frequencies_Hz(), _results->powerInAtCompression_dBm());
    }

    emit progress(100);

    _vna->deleteChannel(c);
    unfreezeChannels();
    if (_settings.isRfOffPostCondition()) {
        _vna->startSweeps();
        _vna->pause();
        _vna->settings().rfOutputPowerOff();
    }

    // Check if any compression points not found
    if (isCompression.count(false) > 0) {
        int iFailure = 0;
        for (int i = 0; i < isCompression.size(); i++) {
            if (!isCompression[i]) {
                iFailure = i;
                break;
            }
        }
        QString msg = "*Could not find compression point for %1";
        msg = msg.arg(formatValue(sweptFreq_Hz[iFailure], 3, Units::Hertz));
        setError(msg);
    }
    else {
        displayResultsOnInstrument();
    }
    _vna->settings().displayOn();
}
