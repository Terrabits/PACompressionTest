#include "FrequencySweep.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

FrequencySweep::FrequencySweep(QObject *parent) :
    MeasureThread(parent)
{

}

FrequencySweep::~FrequencySweep()
{

}

void FrequencySweep::run() {
    QString msg;
    if (!_settings.isValid(*_vna, msg)) {
        setError(msg);
        return;
    }

    _vna->isError();
    _vna->clearStatus();

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
    const bool isStopAtCompression = _settings.isStopAtCompression();

    // Channel, ports
    const uint channel = _settings.channel();
    const uint outputPort = _settings.outputPort();
    const uint inputPort = _settings.inputPort();

    const bool shouldFlipPorts = outputPort < inputPort;

    freezeChannels();

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
        setError("*Measurement cancelled");
        _results->clearAllData();
        _vna->deleteChannel(c);
        return;
    }
    _vna->channel(c).manualSweepOn();
    _results->data() << sweep.measure(outputPort, inputPort);
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

    for (iPower = 1; iPower < powerPoints; iPower++) {
        if (sweptFreq_Hz.isEmpty())
            break;

        emit progress(int((100.0 * iPower)/powerPoints));

        power_dBm = powers_dBm[iPower];
        _results->powers_dBm() << power_dBm;
        sweep.setPower(power_dBm);

        if (isInterruptionRequested()) {
            setError("*Measurement cancelled");
            _results->clearAllData();
            _vna->deleteChannel(c);
            return;
        }
        _results->data() << sweep.measure(outputPort, inputPort);
        if (shouldFlipPorts)
            flipPorts(_results->data()[iPower]);

        QRowVector gains_dB = _results->data()[iPower].y_dB(2, 1);
        for (uint iSweptFreq = 0; iSweptFreq < uint(sweptFreq_Hz.size()); iSweptFreq++) {
            const double freq_Hz = sweptFreq_Hz[iSweptFreq];
            const double gain_dB = gains_dB[iSweptFreq];

            const uint iResult = _results->frequencies_Hz().indexOf(freq_Hz);

            // Check for Max Gain
            if (isGainExpansion) {
                if (_results->maxGain_dB()[iResult] < gain_dB) {
                    _results->powerInAtMaxGain_dBm()[iResult] = power_dBm;
                    _results->maxGain_dB()[iResult] = gain_dB;
                    _results->powerOutAtMaxGain_dBm()[iResult] = power_dBm + gain_dB;
                }
            }

            // Check for compression
            bool isCompression = false;
            const double compression_dB = _results->maxGain_dB()[iResult] - gain_dB;
            if (compression_dB > compressionLevel_dB) {
                isCompression = true;
                _results->powerInAtCompression_dBm()[iResult] = power_dBm;
                _results->gainAtCompression_dB()[iResult] = gain_dB;
                _results->powerOutAtCompression_dBm()[iResult] = power_dBm + gain_dB;
            }
            if (isCompression && isStopAtCompression) {
                sweptFreq_Hz.removeAt(iSweptFreq);
                sweep.deleteSegment(iSweptFreq+1);
                iSweptFreq--;
            }
        }
        emit plotMaxGain(_results->frequencies_Hz(), _results->maxGain_dB());
        emit plotPinAtCompression(_results->frequencies_Hz(), _results->powerInAtCompression_dBm());
    }

    emit progress(100);
    _vna->deleteChannel(c);
    unfreezeChannels();

    // Check if any compression points not found
    if (!sweptFreq_Hz.isEmpty()) {
        QString msg = "*Could not find compression point for %1";
        msg = msg.arg(formatValue(sweptFreq_Hz.first(), 3, Units::Hertz));
        setError(msg);
    }
    else {
        displayResultsOnInstrument();
    }
}

