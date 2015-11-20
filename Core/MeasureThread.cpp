#include <QDebug>

#include "MeasureThread.h"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>
#include <NetworkTraceData.h>

// C++ std lib
#include <complex>

// Qt
#include <QApplication>
#include <QMessageBox>

using namespace RsaToolbox;


MeasureThread::MeasureThread(QObject *parent)
    : QThread(parent)
{

}
MeasureThread::~MeasureThread()
{

}

void MeasureThread::setAppInfo(const QString &name, const QString &version) {
    _appName = name;
    _appVersion = version;
}
void MeasureThread::setVna(Vna *vna) {
    _vna = vna;
}
void MeasureThread::setSettings(const MeasurementSettings &settings) {
    _settings = settings;
}

bool MeasureThread::isError() const {
    return _isError;
}
QString MeasureThread::errorMessage() const {
    return _error;
}
void MeasureThread::clearError() {
    _isError = false;
    _error.clear();
}
void MeasureThread::setError(QString message) {
    _isError = true;
    _error = message;
}

void MeasureThread::start(Priority priority) {
    clearError();
    _results.reset(new MeasurementData());
    _results->setAppInfo(_appName, _appVersion);
    _results->setTimeToNow();
    _results->setVnaInfo(*_vna);
    _results->setSettings(_settings);
    QThread::start(priority);
}
MeasurementData *MeasureThread::takeResults() {
    return _results.take();
}

void MeasureThread::flipPorts() {
    flipPorts(_results->data());
}
void MeasureThread::flipPorts(NetworkData &data) {
    const uint points = data.points();
    for (uint i = 0; i < points; i++) {
        const ComplexMatrix2D s = data.y()[i];
        data.y()[i][0][0] = s[1][1];
        data.y()[i][0][1] = s[1][0];
        data.y()[i][1][0] = s[0][1];
        data.y()[i][1][1] = s[0][0];
    }
}
void MeasureThread::flipPorts(QVector<NetworkData> &data) {
    for (int i = 0; i < data.size(); i++) {
        flipPorts(data[i]);
    }
}

void MeasureThread::freezeChannels() {
    _channels = _vna->channels();
    _isContinuous.resize(_channels.size());
    for (int i = 0; i < _channels.size(); i++) {
        const uint c = _channels[i];
        _isContinuous[i] = _vna->channel(c).isContinuousSweep();
        _vna->channel(c).manualSweepOn();
    }
}
void MeasureThread::unfreezeChannels() {
    for (int i = 0; i < _channels.size(); i++) {
        const uint c = _channels[i];
        if (_isContinuous[i])
            _vna->channel(c).continuousSweepOn();
    }
}

void MeasureThread::displayResultsOnInstrument() {
    const uint iChannel = _settings.channel();
    uint iMaxGain = 0;
    uint iCompression = 0;

    const QString max_gain = "max_gain";
    if (_vna->isChannel(max_gain)) {
        iMaxGain = _vna->channelId(max_gain);
    }

    const QString compression = "compression";
    if (_vna->isChannel(compression)) {
        iCompression = _vna->channelId(compression);
    }

    if (!iMaxGain && !iCompression) {
        _vna->channel(iChannel).select();
        iMaxGain = _vna->createChannel();
        _vna->channel(iMaxGain).setName(max_gain);

        _vna->channel(iChannel).select();
        iCompression = _vna->createChannel();
        _vna->channel(iCompression).setName(compression);

        const QString max_gain = "max_gain";
        if (_vna->isTrace(max_gain))
            _vna->deleteTrace(max_gain);

        const QString compressed_gain = "compressed_gain";
        if (_vna->isTrace(compressed_gain))
            _vna->deleteTrace(compressed_gain);

        const QString pin_compression = "Pin_compression";
        if (_vna->isTrace(pin_compression))
            _vna->deleteTrace(compressed_gain);

        const QString pout_compression = "Pout_compression";
        if (_vna->isTrace(pout_compression))
            _vna->deleteTrace(pout_compression);

        _vna->settings().displayOn();
        uint diagram = _vna->createDiagram();
        createMaxGainTrace(diagram);
        createCompressedGainTrace(diagram);
        createPinCompressionTrace(diagram);
        createPoutCompressionTrace(diagram);
        _vna->settings().displayOff();
    }

    setupChannel(iMaxGain, _results->frequencies_Hz(), _results->powerInAtMaxGain_dBm());
    setupChannel(iCompression, _results->frequencies_Hz(), _results->powerInAtCompression_dBm());
}

void MeasureThread::setupChannel(uint channelIndex, const QRowVector &frequencies_Hz, const QRowVector &powers_dBm) {
    const uint points = frequencies_Hz.size();
    resizeChannel(channelIndex, points);
    _vna->channel(channelIndex).setSweepType(VnaChannel::SweepType::Segmented);

    VnaSegmentedSweep sweep = _vna->channel(channelIndex).segmentedSweep();
    sweep.setIfbandwidth(_settings.ifBw_Hz());
    for (uint i = 1; i <= points; i++) {
        VnaSweepSegment s = sweep.segment(i);
        s.setPoints(1);
        s.setStart(frequencies_Hz[i-1]);
        s.setPower(powers_dBm[i-1]);
    }
}
void MeasureThread::resizeChannel(uint channelIndex, uint points) {
    VnaSegmentedSweep sweep = _vna->channel(channelIndex).segmentedSweep();
    if (points == 0) {
        sweep.deleteSegments();
        return;
    }

    uint segments = sweep.segments();
    while (segments < points) {
        const uint i = sweep.addSegment();
        sweep.segment(i).setPoints(1);
        segments++;
    }
    while (segments > points) {
        sweep.deleteSegment(segments);
        segments--;
    }
}

void MeasureThread::createMaxGainTrace(uint diagram) {
    const QString max_gain = "max_gain";
    const uint index = _vna->channelId(max_gain);

    _vna->createTrace(max_gain, index);
    _vna->trace(max_gain).setNetworkParameter(NetworkParameter::S, _settings.outputPort(), _settings.inputPort());
    _vna->trace(max_gain).setDiagram(diagram);
}
void MeasureThread::createCompressedGainTrace(uint diagram) {
    const QString compression = "compression";
    const uint index = _vna->channelId(compression);

    const QString compressed_gain = "compressed_gain";
    _vna->createTrace(compressed_gain, index);
    _vna->trace(compressed_gain).setNetworkParameter(NetworkParameter::S, _settings.outputPort(), _settings.inputPort());
    _vna->trace(compressed_gain).setDiagram(diagram);
}
void MeasureThread::createPinCompressionTrace(uint diagram) {
    const QString compression = "compression";
    const uint index = _vna->channelId(compression);

    const QString pin_compression = "Pin_compression";
    _vna->createTrace(pin_compression, index);
    _vna->trace(pin_compression).setWaveQuantity(WaveQuantity::a, 1);
    _vna->trace(pin_compression).setDiagram(diagram);
}
void MeasureThread::createPoutCompressionTrace(uint diagram) {
    const QString compression = "compression";
    const uint index = _vna->channelId(compression);

    const QString pout_compression = "Pout_compression";
    _vna->createTrace(pout_compression, index);
    _vna->trace(pout_compression).setWaveQuantity(WaveQuantity::b, 2);
    _vna->trace(pout_compression).setDiagram(diagram);

    const QString pin_compression = "Pin_compression";
    const QString compressed_gain = "compressed_gain";
    QString expression = "%1 + %2";
    expression = expression.arg(pin_compression);
    expression = expression.arg(compressed_gain);
    _vna->trace(pout_compression).math().setExpression(expression);
    _vna->trace(pout_compression).math().on();
}

void MeasureThread::removeEmptyDiagrams() {
    QVector<uint> diagrams = _vna->diagrams();
    foreach(const uint d, diagrams) {
        if (_vna->diagram(d).traces().isEmpty())
            _vna->deleteDiagram(d);
    }
}
