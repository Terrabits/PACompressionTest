#include "FrequencySweepTest.h"


// Project
#include "MeasurementSettings.h"
#include "FrequencySweep.h"

// RsaToolbox
#include <Log.h>
#include <Vna.h>
using namespace RsaToolbox;

// Qt
#include <QTest>
#include <QSignalSpy>
#include <QScopedPointer>
#include <QDebug>
#include <QProgressBar>


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

    FrequencySweep sweep;
    sweep.setAppInfo("PA Compression Test", "0");
    sweep.setSettings(settings);
    sweep.setVna(&vna);

    QSignalSpy started(&sweep, SIGNAL(started()));
    QSignalSpy plotMaxGain(&sweep, SIGNAL(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    QSignalSpy plotPinAtCompression(&sweep, SIGNAL(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    QSignalSpy progress(&sweep, SIGNAL(progress(int)));
    QSignalSpy finished(&sweep, SIGNAL(finished()));
    sweep.start();
    sweep.wait();

    QVERIFY(!vna.isError());

    QVERIFY(vna.isChannel("compression"));
    QVERIFY(vna.isTrace("compression"));
    QVERIFY(vna.trace("compression").diagram());
    QCOMPARE(vna.channelId("compression"), vna.trace("compression").channel());

    QVERIFY(vna.isChannel("max_gain"));
    QVERIFY(vna.isTrace("max_gain"));
    QVERIFY(vna.trace("max_gain").diagram());
    QCOMPARE(vna.channelId("max_gain"), vna.trace("max_gain").channel());

    QCOMPARE(vna.trace("compression").diagram(), vna.trace("max_gain").diagram());

    QVERIFY(!sweep.isError());
    QCOMPARE(sweep.errorMessage(), QString());

    QCOMPARE(started.count(), 1);
    QCOMPARE(progress.count(), plotMaxGain.count()+1);
    QCOMPARE(progress.count(), plotPinAtCompression.count()+1);
    QVERIFY(progress.count() >= 2);
    QCOMPARE(progress.first().first().toInt(), 0);
    QCOMPARE(progress.last().last().toInt(), 100);
    QCOMPARE(finished.count(), 1);

    QScopedPointer<MeasurementData> results(sweep.takeResults());
    QCOMPARE(uint(results->powerInAtMaxGain_dBm().size()), settings.frequencyPoints());
    QCOMPARE(uint(results->maxGain_dB().size()), settings.frequencyPoints());
    QCOMPARE(uint(results->powerOutAtMaxGain_dBm().size()), settings.frequencyPoints());

    QCOMPARE(uint(results->powerInAtCompression_dBm().size()), settings.frequencyPoints());
    QCOMPARE(uint(results->gainAtCompression_dB().size()), settings.frequencyPoints());
    QCOMPARE(uint(results->powerInAtCompression_dBm().size()), settings.frequencyPoints());

    QVERIFY(results->exportToZip(_sourceDir.filePath("Results.zip")));
    QVERIFY(_sourceDir.exists("Results.zip"));
    QVERIFY(generatePlot(results.data(), _sourceDir.filePath("Plot.png")));
    QVERIFY(_sourceDir.exists("Plot.png"));

    // Delete log
    log->deleteLater();
    log.take();
    logThread.quit();
    logThread.wait();
}

bool FrequencySweepTest::generatePlot(MeasurementData *results, const QString &filename) {
    QCustomPlot plot;

    plot.clearGraphs();

    plot.addGraph(plot.xAxis, plot.yAxis);
    plot.graph(0)->setName("Max Gain");
    plot.graph(0)->setPen(QPen(Qt::blue));
    plot.graph(0)->setVisible(true);

    plot.addGraph(plot.xAxis, plot.yAxis2);
    plot.graph(1)->setName("Pin[Compression]");
    plot.graph(0)->setPen(QPen(Qt::red));
    plot.graph(1)->setVisible(true);

    plot.yAxis2->setVisible(true);
    plot.legend->setVisible(true);

    plot.xAxis->setRange(results->settings().startFrequency_Hz(), results->settings().stopFrequency_Hz());
    plot.xAxis->setLabel("Frequency (Hz)");

    plot.yAxis->setRange(-20, 0);
    plot.yAxis->setLabel("Gain (dB)");

    plot.yAxis2->setRange(results->settings().startPower_dBm(), results->settings().stopPower_dBm());
    plot.yAxis2->setLabel("Power (dBm)");

    plot.graph(0)->setData(results->frequencies_Hz(), results->maxGain_dB());
    plot.graph(1)->setData(results->frequencies_Hz(), results->powerInAtCompression_dBm());
    plot.replot();
    return plot.savePng(filename);
}
