

// RsaToolbox
#include <Vna.h>
#include <Log.h>
using namespace RsaToolbox;


// Qt
#include <QTest>
#include <QDebug>
#include <QScopedPointer>
#include <QString>
#include <QDir>


class Test1 : public QObject
{
    Q_OBJECT

public:
    Test1();
    ~Test1();

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void firstTest();

private:
    Vna _vna;
    QScopedPointer<Log> _log;

    int _cycle;
    QString _appName;
    QString _appVersion;
    QString _filename;
    QDir _dir;
};

Test1::Test1() :
    _cycle(0),
    _appName("Test1"),
    _appVersion("0.0"),
    _filename("%1 %2 Log.txt"),
    _dir(SOURCE_DIR)
{
    _dir.mkpath("Test1 Logs");
    _dir.cd("Test1 Logs");
}
Test1::~Test1()
{
    //
}

void Test1::initTestCase() {
    _vna.disconnectLog();

    QString f = _filename.arg(_cycle++).arg("initTestCase()");
    f = _dir.filePath(f);
    _log.reset(new Log(f, _appName, _appVersion));
    _log->printHeader();
    _vna.resetBus(TCPIP_CONNECTION, "127.0.0.1");
    _vna.useLog(_log.data());
    _vna.printInfo();
    _vna.preset();
    _vna.clearStatus();

    // One-time setup

    QVERIFY(!_vna.isError());
    _vna.clearStatus();
    _vna.disconnectLog();
    _vna.resetBus();
}
void Test1::init() {
    _vna.disconnectLog();

    QString f = _filename.arg(_cycle++).arg("init()");
    f = _dir.filePath(f);
    _log.reset(new Log(f, _appName, _appVersion));
    _log->printHeader();
    _vna.resetBus(TCPIP_CONNECTION, "127.0.0.1");
    _vna.useLog(_log.data());
    _vna.printInfo();
    _vna.preset();
    _vna.clearStatus();

    QVERIFY(_vna.isConnected());
    QVERIFY(_vna.properties().isKnownModel());
    QVERIFY(!_vna.idString().isEmpty());

    // Per test case setup

    _vna.disconnectLog();
}
void Test1::cleanup() {
    _vna.disconnectLog();

    QString f = _filename.arg(_cycle++).arg("cleanup()");
    f = _dir.filePath(f);
    _log.reset(new Log(f, _appName, _appVersion));
    _log->printHeader();
    _vna.useLog(_log.data());
    _vna.printInfo();

    // Per test case cleanup
    _vna.isError();

    _vna.clearStatus();
    _vna.resetBus();
    _vna.disconnectLog();
}
void Test1::cleanupTestCase() {
    _vna.disconnectLog();

    QString f = _filename.arg(_cycle++).arg("cleanupTestCase()");
    f = _dir.filePath(f);
    _log.reset(new Log(f, _appName, _appVersion));
    _log->printHeader();
    _vna.useLog(_log.data());
    _vna.printInfo();

    // Testing finished cleanup

    QVERIFY(!_vna.isError());
    QVERIFY(_vna.calUnits().isEmpty());

    _vna.clearStatus();
    _vna.disconnectLog();
    _vna.resetBus();
}

void Test1::firstTest() {
    QString f = _filename.arg(_cycle++).arg("firstTest()");
    f = _dir.filePath(f);
    _log.reset(new Log(f, _appName, _appVersion));
    _vna.useLog(_log.data());
    _vna.printInfo();

    // Test goes here...
    //

    QVERIFY(!_vna.isError());
}


QTEST_MAIN(Test1)
#include "Test1.moc"
