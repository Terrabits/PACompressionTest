#include "HeaderTest.h"


// RsaToolbox
#include <Vna.h>
#include <Log.h>
using namespace RsaToolbox;

// Qt
#include <QTest>
#include <QDebug>


HeaderTest::HeaderTest(QObject *parent) :
    QObject(parent),
    _sourceDir(SOURCE_DIR),
    _ipAddress("192.168.35.13")
{

}
HeaderTest::~HeaderTest() {

}

void HeaderTest::pulseSettings() {
    Vna vna(ConnectionType::TCPIP_CONNECTION, _ipAddress);
    QVERIFY(vna.isConnected());
    QVERIFY(!vna.idString().isEmpty());
    Log log(_sourceDir.filePath("HeaderTest_Log.txt"),
            "PA Compression Test", "0");
    QVERIFY(log.isOpen());
    log.printHeader();
    vna.useLog(&log);
    vna.printInfo();

    qDebug() << "ZVAX-TRM? " << vna.isExtensionUnit();
    qDebug() << "Pulse modulator on path 1? " << vna.channel().extensionUnit().isPulseModulatorOn(1);
    qDebug() << "Delay: " << vna.channel().pulseGenerator().delay_s() << " seconds";
    qDebug() << "Delay: " << vna.channel().pulseGenerator().pulseWidth_s() << " seconds";
    qDebug() << "Delay: " << vna.channel().pulseGenerator().period_s() << " seconds";

    QStringList errors;
    vna.errors(errors);
    qDebug() << "Errors: " << errors;
    vna.clearStatus();
}
