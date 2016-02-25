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
    _undo.setVna(_vna);
//    _undo.setRecallOnDestruction(true);
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

bool MeasureThread::prepareVna() {
    QString msg;
    if (!_settings.isValid(*_vna, msg)) {
        setError(msg);
        _vna->settings().displayOn();
        return false;
    }

    _undo.save();
    QVector<uint> channels = _vna->channels();
    channels.removeAt(channels.indexOf(_settings.channel()));
    _vna->deleteChannels(channels);

    _vna->isError();
    _vna->clearStatus();
    _vna->settings().displayOff();
    return true;
}
void MeasureThread::restoreVna() {
    _undo.recall();
    if (_settings.isRfOffPostCondition()) {
        foreach(uint c, _vna->channels()) {
            _vna->channel(c).manualSweepOn();
        }
        _vna->settings().powerReductionBetweenSweepsOn();
    }
    _vna->local();
}
