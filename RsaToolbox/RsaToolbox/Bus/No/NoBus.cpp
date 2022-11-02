#include "NoBus.h"
using namespace RsaToolbox;

// logging
#include "logging.hpp"

// Qt
#include <QTextStream>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>

// std lib
#include <cstdio>



NoBus::NoBus(QObject *parent)
    : GenericBus(parent)
{
}
NoBus::NoBus(ConnectionType type, QString address,
               uint bufferSize_B, uint timeout_ms,
               QObject *parent)
    : GenericBus(type, address,
                 bufferSize_B, timeout_ms,
                 parent)
{
}
NoBus::~NoBus() {
}

bool NoBus::isOpen() const {
    return false;
}

bool NoBus::lock() {
    LOG(warning) << "Cannot lock instrument via NoBus";
    return false;
}
bool NoBus::unlock() {
    LOG(warning) << "Cannot unlock instrument via NoBus";
    return false;
}
bool NoBus::local() {
    LOG(warning) << "Cannot put instrument into local mode via NoBus.";
    return false;
}
bool NoBus::remote() {
    LOG(warning) << "Cannot put instrument into remote mode via NoBus.";
    return false;
}

QString NoBus::status() const {
    QString text;
    QTextStream stream(&text);
    stream << "Status:   Not connected!" << endl;
    stream.flush();
    return text;
}

bool NoBus::read(char *buffer, uint bufferSize_B) {
    nullTerminate(buffer, bufferSize_B, 0);
    printRead(buffer, 0);
    return false;
}
bool NoBus::write(QString scpi) {
    printWrite(scpi);
    return false;
}
bool NoBus::binaryRead(char *buffer, uint bufferSize_B, uint &bytesRead) {
    Q_UNUSED(bytesRead);
    nullTerminate(buffer, bufferSize_B, 0);
    printRead(buffer, 0);
    return false;
}
bool NoBus::binaryWrite(QByteArray data) {
    printWrite(data);
    return false;
}
