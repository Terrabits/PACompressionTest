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

    retrieveData();
    _vna->settings().displayOn();
    if (isPreexistingTrace()) {
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

void ProcessTrace::retrieveData() {
    qDebug() << "ProcessTrace::retrieveData";
    if (_settings->isYPower()) {
        qDebug() << "  YPower";
        _x = _data->frequencies_Hz();
        if (_settings->isYPin()) {
            qDebug() << "  YPin";
            if (_settings->isAtCompression()) {
                qDebug() << "  @Compression";
                _y_dBm = _data->powerInAtCompression_dBm();
            }
            else {
                qDebug() << "  @MaxGain";
                _y_dBm = _data->powerInAtMaxGain_dBm();
            }
        }
        else {
            // Pout
            qDebug() << "  YPout";
            if (_settings->isAtCompression()) {
                qDebug() << "  @Compression";
                _y_dBm = _data->powerOutAtCompression_dBm();
            }
            else {
                qDebug() << "  @MaxGain";
                _y_dBm = _data->powerOutAtMaxGain_dBm();
            }
        }
    }
    else {
        // SParameter
        qDebug() << "  YSParam";
        uint outputPort, inputPort;
        if (_settings->isYInputReflectionTrace()) {
            qDebug() << "  YS11";
            outputPort = inputPort = 1;
        }
        else if (_settings->isYGainTrace()) {
            qDebug() << "  YS21";
            outputPort = 2;
            inputPort = 1;
        }
        else if (_settings->isYReverseGainTrace()) {
            qDebug() << "  YS12";
            outputPort = 1;
            inputPort = 2;
        }
        else {
            // S22
            qDebug() << "  YS22";
            outputPort = inputPort = 2;
        }

        if (_settings->isAtCompression()) {
            qDebug() << "  @Compression";
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtCompression(outputPort, inputPort);
        }
        else if (_settings->isAtMaximumGain()) {
            qDebug() << "  @MaxGain";
            _x = _data->frequencies_Hz();
            _y = _data->sParameterAtMaxGain(outputPort, inputPort);
        }
        else if (_settings->isXFrequency()) {
            qDebug() << "  @Power " << _settings->atValue;
            _data->sParameterVsFrequency(_settings->atValue, outputPort, inputPort, _x, _y);
        }
        else {
            qDebug() << "  @Freq " << _settings->atValue;
            _data->sParameterVsPower(_settings->atValue, outputPort, inputPort, _x, _y);
        }
    }
}
void ProcessTrace::createChannel() {
    _channel = _vna->createChannel();
    VnaChannel channel = _vna->channel(_channel);

    channel.manualSweepOn();
    channel.setName(_channelName);
    if (_settings->isXPin()) {
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
    qDebug() << "ProcessTrace::createTrace";
    const uint outputPort = _data->settings().outputPort();
    const uint inputPort = _data->settings().inputPort();
    _vna->createTrace(_dataTraceName, _channel);
    if (_settings->isYInputReflectionTrace()) {
        qDebug() << "  S11 Trace";
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, inputPort);
    }
    else if (_settings->isYOutputReflectionTrace()) {
        qDebug() << "  S22 Trace";
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, outputPort);
    }
    else if (_settings->isYGainTrace()) {
        qDebug() << "  S21 Trace";
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, outputPort, inputPort);
    }
    else if (_settings->isYReverseGainTrace()) {
        qDebug() << "  S12 Trace";
        _vna->trace(_dataTraceName).setNetworkParameter(NetworkParameter::S, inputPort, outputPort);
    }
    else if (_settings->isYPin()) {
        qDebug() << "  Pin Trace";
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::a, inputPort);
    }
    else /*if (_settings->isYPout())*/ {
        qDebug() << "  Pout Trace";
        _vna->trace(_dataTraceName).setWaveQuantity(WaveQuantity::b, outputPort);
    }
    _vna->trace(_dataTraceName).setDiagram(_diagram);
    _vna->trace(_dataTraceName).toMemory(_memoryTraceName);
    _vna->trace(_memoryTraceName).setDiagram(_diagram);
    updateTrace();
    _vna->trace(_dataTraceName).hide();
}
void ProcessTrace::updateTrace() {
    qDebug() << "ProcessTrace::updateTrace";
    VnaTrace trace = _vna->trace(_memoryTraceName);
    if (_settings->isXFrequency()) {
        if (_settings->isYPower()) {
            qDebug() << " Power vs Frequency (_y_dBm).";
            debugPrint(_memoryTraceName);
            trace.write(toComplex_dBm(_y_dBm));
        }
        else {
            qDebug() << " Smn vs Frequency (_y).";
            debugPrint(_memoryTraceName);
            trace.write(_y);
        }
    }
    else {
        if (_settings->isYPower()) {
            qDebug() << " Power vs Power (_y_dBm).";
            debugPrint(_memoryTraceName);
            trace.write(toComplex_dBm(_y_dBm));
        }
        else {
            qDebug() << " Smn vs Power (_y).";
            debugPrint(_memoryTraceName);
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
