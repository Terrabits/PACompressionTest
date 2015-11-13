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

    QCustomPlot plot;
    setupPlot(&plot, settings);
    sweep.setProgressPlot(&plot);

    QSignalSpy started(&sweep, SIGNAL(started()));
    QSignalSpy progress(&sweep, SIGNAL(progress(int)));
    QSignalSpy finished(&sweep, SIGNAL(finished()));
    sweep.start();
    sweep.wait();

    QVERIFY(!vna.isError());
    QCOMPARE(vna.channels().size(), 1);

    QVERIFY(!sweep.isError());
    QCOMPARE(sweep.errorMessage(), QString());

    QCOMPARE(started.count(), 1);
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
    QVERIFY(plot.savePng(_sourceDir.filePath("Plot.png")));

    // Delete log
    log->deleteLater();
    log.take();
    logThread.quit();
    logThread.wait();
}

void FrequencySweepTest::setupPlot(QCustomPlot *plot, MeasurementSettings &settings) {
    plot->clearGraphs();

    plot->addGraph(plot->xAxis, plot->yAxis);
    plot->graph(0)->setName("Max Gain");
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setVisible(true);

    plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->graph(1)->setName("Pin[Compression]");
    plot->graph(0)->setPen(QPen(Qt::red));
    plot->graph(1)->setVisible(true);

    plot->yAxis2->setVisible(true);
    plot->legend->setVisible(true);

    plot->xAxis->setRange(settings.startFrequency_Hz(), settings.stopFrequency_Hz());
    plot->xAxis->setLabel("Frequency (Hz)");

    plot->yAxis->setRange(-20, 0);
    plot->yAxis->setLabel("Gain (dB)");

    plot->yAxis2->setRange(settings.startPower_dBm(), settings.stopPower_dBm());
    plot->yAxis2->setLabel("Power (dBm)");
}
