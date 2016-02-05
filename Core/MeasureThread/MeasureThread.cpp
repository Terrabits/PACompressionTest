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
