#include "FrequencySweepTest.h"


// Project
#include "MeasurementSettings.h"
#include "FrequencySweep.h"

// RsaToolbox
#include <Log.h>
#include <Vna.h>
#include <qcustomplot.h>
using namespace RsaToolbox;

// Qt
#include <QTest>
#include <QSignalSpy>
#include <QScopedPointer>
#include <QDebug>


FrequencySweepTest::FrequencySweepTest() :
    _sourceDir(SOURCE_DIR)
{
}
FrequencySweepTest::~FrequencySweepTest() {
}

void FrequencySweepTest::initTestCase() {
}
void FrequencySweepTest::cleanupTestCase() {
}

void FrequencySweepTest::init() {
}
void FrequencySweepTest::cleanup() {
}

void FrequencySweepTest::sweep() {
    Vna vna(ConnectionType::TCPIP_CONNECTION, "127.0.0.1");
    QVERIFY(vna.isConnected());
    QVERIFY(!vna.idString().isEmpty());

    QScopedPointer<Log> log(new Log(_sourceDir.filePath("FrequencySweepTest_Log.txt"),
            "PA Compression Test", "0"));
    QVERIFY(log->isOpen());
    log->printHeader();

    QThread logThread;
    log->moveToThread(&logThread);
    logThread.start();

    vna.useLog(log.data());
    vna.printInfo();

    vna.preset();

    MeasurementSettings settings;
    settings.setStartFrequency(1.0E9);
    settings.setStopFrequency(2.0E9);
    settings.setFrequencyPoints(101); // step: 10 MHz
    settings.setIfBw(1.0E3);

    settings.setStartPower(-20);
    settings.setStopPower(10);
    settings.setPowerPoints(301); // step: 0.1 dBm

    settings.setCompressionLevel(1.0);
    settings.setGainExpansion(true);
    settings.setStopAtCompression(true);
    settings.setRfOffPostCondition(false);

    settings.setChannel(1);
    settings.setInputPort(1);
    settings.setOutputPort(2);

    QCustomPlot plot;

    FrequencySweep sweep;
    sweep.setAppInfo("PA Compression Test", "0");
    sweep.setSettings(settings);

    sweep.setVna(&vna);
    sweep.setProgressPlot(&plot);

    QSignalSpy started(&sweep, SIGNAL(started()));
    QSignalSpy progress(&sweep, SIGNAL(progress(int)));
    QSignalSpy finished(&sweep, SIGNAL(finished()));
    sweep.start();
    sweep.wait();

    QVERIFY(!vna.isError());
    QCOMPARE(vna.channels().size(), 1);
    QVERIFY(!sweep.isError());
    QCOMPARE(started.count(), 1);
    QVERIFY(progress.count() > 0);
    QCOMPARE(finished.count(), 1);

    QCOMPARE(sweep.errorMessage(), QString());

    QScopedPointer<MeasurementData> results(sweep.takeResults());

    // Delete log
    log->deleteLater();
    log.take();
    logThread.quit();
    logThread.wait();
}
