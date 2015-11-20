#include "SafeFrequencySweepTest.h"


// Project
#include "MeasurementSettings.h"
#include "SafeFrequencySweep.h"

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


SafeFrequencySweepTest::SafeFrequencySweepTest() :
    _sourceDir(SOURCE_DIR)
{
}
SafeFrequencySweepTest::~SafeFrequencySweepTest() {
}

void SafeFrequencySweepTest::initTestCase() {
}
void SafeFrequencySweepTest::cleanupTestCase() {
}

void SafeFrequencySweepTest::init() {
}
void SafeFrequencySweepTest::cleanup() {
}

void SafeFrequencySweepTest::sweep() {
    Vna vna(ConnectionType::TCPIP_CONNECTION, "127.0.0.1");
    QVERIFY(vna.isConnected());
    QVERIFY(!vna.idString().isEmpty());

    QScopedPointer<Log> log(new Log(_sourceDir.filePath("SafeFrequencySweepTest_Log.txt"),
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

    SafeFrequencySweep sweep;
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

    const QString max_gain = "max_gain";
    QVERIFY(vna.isChannel(max_gain));
    const uint iMaxGain = vna.channelId(max_gain);

    QVERIFY(vna.isTrace(max_gain));
    QVERIFY(vna.trace(max_gain).diagram());
    QCOMPARE(iMaxGain, vna.trace(max_gain).channel());

    const QString compression = "compression";
    QVERIFY(vna.isChannel(compression));
    const uint iCompression = vna.channelId(compression);

    const QString compressed_gain = "compressed_gain";
    const QString pin_compression = "Pin_compression";
    const QString pout_compression = "Pout_compression";
    QVERIFY(vna.isTrace(compressed_gain));
    QVERIFY(vna.isTrace(pin_compression));
    QVERIFY(vna.isTrace(pout_compression));
    QCOMPARE(iCompression, vna.trace(compressed_gain).channel());
    QCOMPARE(iCompression, vna.trace(pin_compression).channel());
    QCOMPARE(iCompression, vna.trace(pout_compression).channel());

    const uint diagram = vna.trace(max_gain).diagram();
    QVERIFY(diagram);
    QCOMPARE(diagram, vna.trace(compressed_gain).diagram());
    QCOMPARE(diagram, vna.trace(pin_compression).diagram());
    QCOMPARE(diagram, vna.trace(pout_compression).diagram());

    QVERIFY(!sweep.isError());
    QVERIFY(sweep.errorMessage().isEmpty());

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

bool SafeFrequencySweepTest::generatePlot(MeasurementData *results, const QString &filename) {
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
