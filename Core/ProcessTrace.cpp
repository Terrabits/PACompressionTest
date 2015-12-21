#include "ProcessTrace.h"


// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QDebug>

ProcessTrace::ProcessTrace(TraceSettings *settings, MeasurementData *data, Vna *vna, uint defaultDiagram) :
    _settings(settings),
    _data(data),
    _vna(vna),
    _diagram(defaultDiagram),
    _memoryTraceName(settings->name)
{
    _channelName = "_" + _memoryTraceName;
    _dataTraceName = _channelName;

    if (!retrieveData()) {
        qDebug() << "Could not retrieve data";
        return;
    }

    _vna->settings().updateDisplay();
    _vna->settings().displayOn();
    if (isPreexistingTrace()) {
        updateChannel();
        updateTrace();
    }
    else {
        createChannel();
        createTrace();
    }
}
ProcessTrace::~ProcessTrace() {

}

bool ProcessTrace::isPreexistingTrace() {
    return _vna->isTrace(_memoryTraceName);
}

ComplexRowVector ProcessTrace::toComplex_dBm(QRowVector powers_dBm) {
    ComplexRowVector _result(powers_dBm.size());
    for (int i = 0; i < powers_dBm.size(); i++) {
        const double magnitude = toMagnitude(powers_dBm[i] - 13.0);
        _result[i] = ComplexDouble(magnitude, 0);
    }
    return _result;
}

bool ProcessTrace::retrieveData() {
    _x.clear();
    _y.clear();
    _y_dBm.clear();

    if (_settings->isYPower()) {
        if (_settings->isXFrequency()) {
            _x = _data->frequencies_Hz();
            if (_settings->isYPin()) {
                if (_settings->isAtCompression()) {
                    // y: Pin
                    // x: Frequency
                    // @: Compression
                    _y_dBm = _data->powerInAtCompression_dBm();
                }
                else {
                    // y: Pin
                    // x: Frequency
                    // @: MaxGain
                    _y_dBm = _data->powerInAtMaxGain_dBm();
                }
            }
            else {
                // Pout
                if (_settings->isAtCompression()) {
                    // y: Pout
                    // x: Frequency
                    // @: Compression
                    _y_dBm = _data->powerOutAtCompression_dBm();
                }
                else if (_settings->isAtMaximumGain()) {
                    // y: Pout
                    // x: Frequency
                    // @: MaxGain
                    _y_dBm = _data->powerOutAtMaxGain_dBm();
                }
                else {
                    // y: Pout
                    // x: Frequency
                    // @: Pin
                    if (!_data->poutVsFrequency(_settings->atValue, _x, _y_dBm))
                        return false;
                }
            }
        }
        else {
            // y: Pout
            // x: Pin
            // @: Frequency
            if (!_data->poutVsPin(_settings->atValue, _x, _y_dBm))
                return false;
        }
    }
    else { // SParameter
        // Ports
        uint outputPort, inputPort;
        if (_settings->isYInputReflectionTrace()) {
            outputPort = inputPort = 1;
        }
        else if (_settings->isYGainTrace()) {
            outputPort = 2;
            inputPort = 1;
        }
        else if (_settings->isYReverseGainTrace()) {
            outputPort = 1;
            inputPort = 2;
        }
        else {
            // S22
            outputPort = inputPort = 2;
        }

        if (_settings->isAtCompression()) {
            // y: SParameter
            // x: Frequency
            // @: Compression
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtCompression(outputPort, inputPort);
        }
        else if (_settings->isAtMaximumGain()) {
            // y: SParameter
            // x: Frequency
            // @: MaxGain
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtMaxGain(outputPort, inputPort);
        }
        else if (_settings->isXFrequency()) {
            // y: SParameter
            // x: Frequency
            // @: Pin
            if (!_data->sParameterVsFrequencyAtPin(_settings->atValue, outputPort, inputPort, _x, _y))
                return false;
        }
        else if (_settings->isXPin()) {
            // y: SParameter
            // x: Pin
            // @: Frequency
            if (!_data->sParameterVsPin(_settings->atValue, outputPort, inputPort, _x, _y))
                return false;
        }
        else {
            // y: SParameter
            // x: Pout
            // @: Frequency
            if (!_data->sParameterVsPout(_settings->atValue, outputPort, inputPort, _x, _y))
                return false;
        }
    }

    return true;
}

void ProcessTrace::updateChannel() {
    _channel = _vna->trace(_memoryTraceName).channel();
    if (_settings->isXFrequency()) {
        _vna->channel(_channel).setFrequencies(_data->frequencies_Hz());
    }
    else {
        // X Axis: Power
        _vna->channel(_channel).setSweepType(VnaChannel::SweepType::Power);
        _vna->channel(_channel).powerSweep().setStart(_x.first());
        _vna->channel(_channel).powerSweep().setStop(_x.last());
        _vna->channel(_channel).powerSweep().setPoints(_x.size());
    }
    _vna->settings().updateDisplay();
    _vna->settings().displayOn();
}

void ProcessTrace::createChannel() {
    _channel = _vna->createChannel();
    VnaChannel channel = _vna->channel(_channel);

    channel.manualSweepOn();
    channel.setName(_channelName);
    if (_settings->isXPower()) {
        channel.setSweepType(VnaChannel::SweepType::Power);
        channel.powerSweep().setStart(_x.first());
        channel.powerSweep().setStop(_x.last());
        channel.powerSweep().setPoints(_x.size());
    }
    else {
        channel.setFrequencies(_x);
    }
}

void ProcessTrace::createTrace() {
    const uint outputPort = _data->settings().outputPort();
    const uint inputPort = _data->settings().inputPort();
    _vna->createTrace(_dataTraceName, _channel);
    if (_settings->isYInputReflectionTrace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, inputPort);
    }
    else if (_settings->isYOutputReflectionTrace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, outputPort);
    }
    else if (_settings->isYGainTrace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
    }
    else if (_settings->isYReverseGainTrace()) {
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, outputPort);
    }
    else if (_settings->isYPin()) {
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::a, inputPort);
    }
    else /*if (_settings->isYPout())*/ {
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::b, outputPort);
    }

    _vna->createDiagram(_diagram);
//    _vna->trace(_dataTraceName).setDiagram(_diagram);
    _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
    _vna->trace(_memoryTraceName).setDiagram(_diagram);
    updateTrace();
    _vna->trace(_dataTraceName).hide();
}
void ProcessTrace::updateTrace() {
    VnaTrace trace = _vna->trace(_memoryTraceName);
    if (_settings->isXFrequency()) {
        if (_settings->isYPower()) {
            trace.write(_y_dBm);
        }
        else {
            trace.write(_y);
        }
    }
    else {
        if (_settings->isYPower()) {
            trace.write(_y_dBm);
        }
        else {
            trace.write(_y);
        }
    }
}

void ProcessTrace::debugPrint(const QString &name) {
    qDebug() << "  at \'" << name << "\':";
    qDebug() << "    x.size:     " << _x.size();
    qDebug() << "    y.size:     " << _y.size();
    qDebug() << "    y_dBm.size: " << _y_dBm.size();
}
