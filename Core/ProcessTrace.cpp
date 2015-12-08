#include "ProcessTrace.h"

// RsaToolbox
using namespace RsaToolbox;

ProcessTrace::ProcessTrace(TraceSettings *settings, MeasurementData *data, Vna *vna, uint defaultDiagram) :
    _settings(settings),
    _data(data),
    _vna(vna),
    _diagram(defaultDiagram),
    _memoryTraceName(settings->name)
{
    retrieveData();
    if (isPreexistingTrace()) {
        updateTrace();
    }
    else {
        createTrace();
    }
}

bool ProcessTrace::isPreexistingTrace() {
    return _vna->isTrace(_memoryTraceName);
}
void ProcessTrace::retrieveData() {
    if (_settings->isYPower()) {
        _x = _data->frequencies_Hz();
        if (_settings->isYPin()) {
            if (_settings->isAtCompression()) {
                _y = _data->powerInAtCompression_dBm();
            }
            else {
                _y = _data->powerInAtMaxGain_dBm();
            }
        }
        else {
            // Pout
            if (_settings->isAtCompression()) {
                _y = _data->powerOutAtCompression_dBm();
            }
            else {
                _y = _data->powerOutAtMaxGain_dBm();
            }
        }
    }
    else {
        // SParameter
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
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtCompression(outputPort, inputPort);
        }
        else if (_settings->isAtMaximumGain()) {
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtMaxGain(outputPort, inputPort);
        }
        else if (_settings->isXFrequency()) {
            // @ Power
            _data->sParameterVsFrequency(_settings->atValue, outputPort, inputPort, _x, _y);
        }
        else {
            // XPin @ Frequency
            _data->sParameterVsPower(_settings->atValue, outputPort, inputPort, _x, _y);
        }
    }
}
void ProcessTrace::createTrace() {
    _channel = _vna->createChannel();
    _vna->channel(_channel).manualSweepOn();
    _vna->channel(_channel).setName(_channelName);
    if (_settings->isXFrequency()) {
        _vna->channel(_channel).setSweepType(VnaChannel::SweepType::Segmented);
    }
    else {
        _vna->channel(_channel).setSweepType(VnaChannel::SweepType::Power);
    }
    const uint outputPort = _data->settings.outputPort();
    const uint inputPort = _data->settings.inputPort();
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
    _vna->trace(-dataTraceName).setDiagram(_diagram);
    _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
    updateTrace();
    _vna->trace(_dataTraceName).hide();
}
void ProcessTrace::updateTrace() {
    VnaChannel channel = _vna->channel(_channel);
    channel.manualSweepOn();

    VnaTrace trace = _vna->trace(_memoryTraceName);
    if (_settings->isXFrequency()) {
        trace.write(_x, _y);
    }
    else {
        channel.setSweepType(VnaChannel::SweepType::Power);
        channel.powerSweep().setStart(_x.first());
        channel.powerSweep().setStop(_x.last());
        channel.powerSweep().setPoints(_x.size());
        trace.write(_y);
    }
}
