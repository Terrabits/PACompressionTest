#include "PowerSupplyDriver.h"


// Qt
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantList>


// std lib
#include <cassert>


PowerSupplyDriver::PowerSupplyDriver() {
    init();
}

PowerSupplyDriver::PowerSupplyDriver(const PowerSupplyDriver &other) {
    copy(other);
}


PowerSupplyDriver::PowerSupplyDriver(const QString &filename) {
    open(filename);
}


bool PowerSupplyDriver::isOpen() const {
    return _isOpen;
}


bool PowerSupplyDriver::isValid() const {
    QString msg;
    return isValid(msg);
}
bool PowerSupplyDriver::isValid(QString &errorMessage) const {
    errorMessage.clear();

    // Open?
//    if (!isOpen()) {
//        errorMessage = "file not open";
//        return false;
//    }
    if (isOpenError()) {
        errorMessage = _openError;
        return false;
    }


    // Valid values?
    if (setupScpi.isEmpty()) {
        errorMessage = "setup scpi not found";
        return false;
    }

    if (query.trimmed().isEmpty()) {
      errorMessage = "query power scpi not found";
      return false;
    }

    // validated
    return true;
}


QString PowerSupplyDriver::filename() const {
    return _filename;
}


void PowerSupplyDriver::operator=(const PowerSupplyDriver &other) {
    copy(other);
}


bool PowerSupplyDriver::isOpenError() const {
    return !_openError.trimmed().isEmpty();
}


void PowerSupplyDriver::init() {
    _filename.clear();
    _isOpen = false;
    _openError.clear();
    clearKeys();
}


void PowerSupplyDriver::clearKeys() {
    preset = false;
    setupScpi.clear();
    query.clear();
}


bool PowerSupplyDriver::open(const QString &filename) {
    init();

    // open file
    _filename = filename;
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        _openError = "could not open file";
        return false;
    }


    // read
    QByteArray text = file.readAll();
    file.close();


    // is file empty?
    if (text.isEmpty()) {
        _openError = "file is empty";
        return false;
    }

    // parse json
    QJsonParseError e;
    QJsonDocument document = QJsonDocument::fromJson(text, &e);

    if (e.error != QJsonParseError::ParseError::NoError) {
        _openError = e.errorString();
        return false;
    }

    if (!document.isObject()) {
        _openError = "could not parse file";
        return false;
    }


    QJsonObject json = document.object();


    // Check for keys
    QStringList keys;
    keys << "preset"
         << "setup scpi"
         << "query power scpi";

    foreach (const QString &key, keys) {
        if (!json.contains(key)) {
            _openError = "%1 is missing";
            _openError = _openError.arg(key);
            return false;
        }
    }


    // Check expected data types
    if (!json.value("preset").isBool()) {
        _openError = "invalid preset value";
        return false;
    }
    if (!json.value("setup scpi").isArray()) {
        _openError = "invalid setup scpi array";
        return false;
    }
    if (!json.value("query power scpi").isString()) {
        _openError = "invalid set points scpi";
        return false;
    }


    // Get values
    preset            = json.value("preset").toBool();
    setupScpi         = json.value("setup scpi").toVariant().toStringList();
    queryPowerScpi    = json["set points scpi"].toString();


    // success
    _isOpen = true;
    return true;
}


void PowerSupplyDriver::copy(const PowerSupplyDriver &other) {
    _filename      = other._filename;
    _isOpen        = other._isOpen;
    _openError     = other._openError;
    preset         = other.preset;
    setupScpi      = other.setupScpi;
    queryPowerScpi = other.queryPowerScpi;
}
