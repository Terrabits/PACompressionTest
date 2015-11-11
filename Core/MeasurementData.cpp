#include "MeasurementData.h"


//RsaToolbox
#include <General.h>
#include <Touchstone.h>
#include <Log.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

// QuaZip
#include <quazip.h>
#include <JlCompress.h>


MeasurementData::MeasurementData() :
    _timeStamp(QDateTime::currentDateTime())
{

}
MeasurementData::~MeasurementData()
{

}

void MeasurementData::setAppInfo(const QString &name, const QString &version) {
    _appName = name;
    _appVersion = version;
}
void MeasurementData::setTimeToNow() {
    _timeStamp = QDateTime::currentDateTime();
}
void MeasurementData::setVnaInfo(Vna &vna) {
    vna.printInfo(_vnaInfo);
}

MeasurementSettings MeasurementData::settings() const {
    return _settings;
}
void MeasurementData::setSettings(const MeasurementSettings &settings) {
    _settings = settings;
}

QRowVector &MeasurementData::frequencies_Hz() {
    return _frequencies_Hz;
}
QRowVector &MeasurementData::powers_dBm() {
    return _powers_dBm;
}

QRowVector &MeasurementData::powerInAtMaxGain_dBm() {
    return _powerInAtMaxGain_dBm;
}
QRowVector &MeasurementData::maxGain_dB() {
    return _maxGain_dB;
}
QRowVector &MeasurementData::powerOutAtMaxGain_dBm() {
    return _powerOutAtMaxGain_dBm;
}
QRowVector &MeasurementData::powerInAtCompression_dBm() {
    return _powerInAtCompression_dBm;
}
QRowVector &MeasurementData::gainAtCompression_dB() {
    return _gainAtCompression_dB;
}
QRowVector &MeasurementData::powerOutAtCompression_dBm() {
    return _powerOutAtCompression_dBm;
}

QVector<NetworkData> &MeasurementData::data() {
    return _data;
}

void MeasurementData::clearAllData() {
    _frequencies_Hz.clear();
    _powers_dBm.clear();

    _powerInAtMaxGain_dBm.clear();
    _maxGain_dB.clear();
    _powerOutAtMaxGain_dBm.clear();

    _powerInAtCompression_dBm.clear();
    _gainAtCompression_dB.clear();
    _powerOutAtCompression_dBm.clear();

    _data.clear();
}

bool MeasurementData::exportToZip(QString filename) {
    if (filename.endsWith(".zip", Qt::CaseInsensitive))
        filename.chop(4);

    QString s2pFolder = "Touchstone Files";
    QString csvFile = "Compression.csv";
    QString infoFile = "Settings.txt";
    QFileInfo fileInfo(filename);
    QDir dir(fileInfo.absolutePath());

    if (!dir.mkdir(fileInfo.fileName())) {
        return false;
    }
    dir.cd(fileInfo.fileName());

    infoFile = dir.filePath(infoFile);
    if (!exportInfo(infoFile)) {
        dir.removeRecursively();
        return false;
    }

    csvFile = dir.filePath(csvFile);
    if (!exportCsv(csvFile)) {
        dir.removeRecursively();
        return false;
    }

    if (!dir.mkdir(s2pFolder)) {
        dir.removeRecursively();
        return false;
    }
    s2pFolder = dir.filePath(s2pFolder);
    if (!exportTouchstone(s2pFolder)) {
        dir.removeRecursively();
        return false;
    }



    filename += ".zip";
    bool success = JlCompress::compressDir(filename, dir.path(), true);
    dir.removeRecursively();
    return success;
}
bool MeasurementData::exportInfo(QString path) {
    Log log(path, _appName, _appVersion);
    log.printHeader();
    log.printLine("SETTING:\n");
    log.print(_settings.printInfo());
    log.print(_vnaInfo);
    return true;
}
bool MeasurementData::exportCsv(QString path) {
    if (!path.endsWith(".csv", Qt::CaseInsensitive))
        path += ".csv";

    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return false;

    const int FIELD_WIDTH = 16;
    QTextStream s(&file);
    s.setFieldAlignment(QTextStream::AlignLeft);
    s.setFieldWidth(FIELD_WIDTH);

    s << "Frequency Hz,"; // 13
    s << "Pin[Max Gain] dBm,"; // 18
    s << "Max Gain dB,"; // 12
    s << "Pout[Max] dBm,"; // 14
    s << "Pin[Comp] dBm,"; // 14
    s << "Gain[Comp] dB,"; // 14
    s << "Pout[Comp] dBm"; // 15

    s.setFieldWidth(0);
    s << "\n";
    s.setFieldWidth(FIELD_WIDTH);

    for (int i = 0; i < _frequencies_Hz.size(); i++) {
        s << _frequencies_Hz[i];
        s << _powerInAtMaxGain_dBm[i];
        s << _maxGain_dB[i];
        s << _powerOutAtMaxGain_dBm[i];
        s << _powerInAtCompression_dBm[i];
        s << _gainAtCompression_dB[i];
        s << _powerOutAtCompression_dBm[i];

        s.setFieldWidth(0);
        s << "\n";
        s.setFieldWidth(FIELD_WIDTH);
    }

    s.flush();
    file.close();
    return true;
}
bool MeasurementData::exportTouchstone(QString path) {
    QDir _path(path);
    for (uint i = 0; i < _settings.powerPoints(); i++) {
        QString filename = "Pin %1 dBm.s2p";
        filename = filename.arg(formatDouble(_powers_dBm[i], 2));
        filename = _path.filePath(filename);
        if (!Touchstone::write(_data[i], filename)) {
            return false;
        }
    }
    return true;
}
