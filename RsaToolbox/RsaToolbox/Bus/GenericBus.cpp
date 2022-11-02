#include "GenericBus.h"

// logging
#include "logging.hpp"


// RsaToolbox
using namespace RsaToolbox;


QString RsaToolbox::toString(ConnectionType connectionType) {
    switch(connectionType) {
    case ConnectionType::VisaTcpConnection:
    case ConnectionType::VisaTcpSocketConnection:
    case ConnectionType::VisaHiSlipConnection:
    case ConnectionType::TcpSocketConnection:
        return "TCPIP";
    case ConnectionType::VisaGpibConnection:
        return "GPIB";
    case ConnectionType::VisaUsbConnection:
    case ConnectionType::UsbConnection:
        return "USB";
    default:
        return "No Connection";
    }
}

GenericBus::GenericBus(QObject *parent) :
    QObject(parent)
{
    _connectionType = ConnectionType::NoConnection;
    _address.clear();
    _bufferSize_B = 0;
    _timeout_ms = 0;
}

GenericBus::GenericBus(ConnectionType connectionType,
                       QString address,
                       uint bufferSize_B, uint timeout_ms,
                       QObject *parent) :
    QObject(parent)
{
    _connectionType = connectionType;
    _address = address;
    _timeout_ms = ((timeout_ms   > 0) ? timeout_ms   : 1000);
    (bufferSize_B > 0) ? setBufferSize(bufferSize_B) : setBufferSize(500);
}

bool GenericBus::isClosed() const {
    return(!isOpen());
}


ConnectionType GenericBus::connectionType() const {
    return(_connectionType);
}


QString GenericBus::address() const {
    return(_address);
}


uint GenericBus::bufferSize_B() const {
    return(_bufferSize_B);
}


void GenericBus::setBufferSize(uint size_B) {
    if (size_B > 0) {
        _bufferSize_B = size_B;
        _buffer.reset(new char[size_B]);
    }
}


uint GenericBus::timeout_ms() const {
    return(_timeout_ms);
}


void GenericBus::setTimeout(uint time_ms) {
    if (time_ms > 0)
        _timeout_ms = time_ms;
}

QString GenericBus::read() {
    if (read(_buffer.data(), _bufferSize_B))
        return(QString(_buffer.data()));
    else
        return(QString());
}

QString GenericBus::query(QString scpi) {
    if (write(scpi))
        return(read());
    else
        return(QString());
}

QByteArray GenericBus::binaryRead() {
    uint bytesRead;
    if (binaryRead(_buffer.data(), _bufferSize_B, bytesRead))
        return(QByteArray(_buffer.data(), bytesRead));
    else
        return(QByteArray());
}


QByteArray GenericBus::binaryQuery(QByteArray scpi) {
    if (binaryWrite(scpi))
        return(binaryRead());
    else
        return(QByteArray());
}


void GenericBus::printRead(char *buffer, uint bytesRead) const {
  // truncate?
  bool truncate = bytesRead > MAX_PRINT;
  int  count    = truncate? MAX_PRINT : bytesRead;

  // message
  QString text = QString(QByteArray(buffer, count));
  text         = text.trimmed();
  if (truncate) {
    text += "...";
  }

  // log
  QByteArray data = text.toUtf8();
  LOG(debug) << data.constData();
}


void GenericBus::printWrite(QString scpi) const {
  // truncate?
  const bool truncate = scpi.size() > MAX_PRINT;
  const int  count    = truncate? MAX_PRINT : scpi.size();

  // message
  QString text        = scpi;
  if (truncate) {
    text.truncate(count);
    text += "...";
  }

  // log
  QByteArray data = text.toUtf8();
  LOG(debug) << data.constData();
}

void GenericBus::setConnectionType(ConnectionType type) {
    _connectionType = type;
}

void GenericBus::setAddress(const QString &address) {
    _address = address;
}

void GenericBus::nullTerminate(char *buffer, uint bufferSize_B, uint bytesUsed) {
    // Do not null terminate if it means
    // data will be overwritten
    if (bytesUsed < bufferSize_B)
        buffer[bytesUsed] = '\0';
}
