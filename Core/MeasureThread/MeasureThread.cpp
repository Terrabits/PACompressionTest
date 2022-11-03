#include "MeasureThread.h"

// logging
#include "logging.hpp"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>
#include <NetworkTraceData.h>
using namespace RsaToolbox;

// Qt
#include <QApplication>
#include <QMessageBox>

// C++ std lib
#include <complex>


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
    LOG(info) << "starting measurement";
    clearError();
    _results.reset(new MeasurementData());
    _results->setAppInfo(_appName, _appVersion);
    _results->setTimeToNow();
    _results->setSettings(_settings);
    _results->createExportFileHeader(*_vna);
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

bool MeasureThread::prepareVna() {
    QString msg;
    if (!_settings.isValid(*_vna, msg)) {
        setError(msg);
        _vna->local();
        return false;
    }

    // Set RF Off post-measurement condition
    _vna->settings().powerReductionBetweenSweepsOn(_settings.isRfOffPostCondition());

    // Stop all channels
    _continuousChannels.clear();
    foreach(uint c, _vna->channels()) {
        if (_vna->channel(c).isContinuousSweep())
            _continuousChannels << c;
        _vna->channel(c).manualSweepOn();
    }

    // Copy reference channel
    _vna->channel(_settings.channel()).select();
    _measurementChannel = _vna->createChannel();

    // Clear errors
    _vna->isError();
    _vna->clearStatus();
    return true;
}
void MeasureThread::restoreVna() {
    _vna->deleteChannel(_measurementChannel);

    if (!_settings.isRfOffPostCondition()) {
        foreach(uint c, _continuousChannels)
            _vna->channel(c).continuousSweepOn();
    }
    _continuousChannels.clear();
    _vna->local();
}
