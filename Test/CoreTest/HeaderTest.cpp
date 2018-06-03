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
    _ipAddress("192.168.35.13::5025")
{

}
HeaderTest::~HeaderTest() {

}

void HeaderTest::pulseSettings() {
    Vna vna(ConnectionType::VisaTcpSocketConnection, _ipAddress);
    QVERIFY(vna.isOpen      ());
    QVERIFY(vna.isResponding());
    vna.startLog(_sourceDir.filePath("HeaderTest_Log.txt"),
                 "PA Compression Test Test", "0.0");
    QVERIFY(vna.isLogging());

    vna.isExtensionUnit();
    vna.channel().extensionUnit ().isPulseModulatorOn(1);
    vna.channel().pulseGenerator().pulseWidth_s();
    vna.channel().pulseGenerator().period_s    ();

    vna.isErrors();
    vna.clearStatus();
}
