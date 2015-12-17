#include "TraceSettingsTest.h"


// Project
#include "TraceSettings.h"

// RsaToolbox
#include <Keys.h>
using namespace RsaToolbox;

// Qt
#include <QTest>
#include <QString>


TraceSettingsTest::TraceSettingsTest(QObject *parent) :
    QObject(parent)
{
    _dir.setPath(SOURCE_DIR);
    _dir.mkpath("TraceSettingsTest");
    _dir.cd("TraceSettingsTest");
}
TraceSettingsTest::~TraceSettingsTest() {

}

void TraceSettingsTest::setNameFromSettings_data() {
    QTest::addColumn<QString>("yParameter");
    QTest::addColumn<QString>("xParameter");
    QTest::addColumn<QString>("atParameter");
    QTest::addColumn<double>("atValue");
    QTest::addColumn<QString>("expectedName");

    QTest::newRow("InputReflectionAtCompression")  << "Input Reflection"    << "Frequency" << "Compression"  << 0.0   << "Input_Reflection_at_Compression";
    QTest::newRow("OutputReflectionAtMaximumGain") << "Output Reflection"   << "Frequency" << "Maximum Gain" << 0.0   << "Output_Reflection_at_Maximum_Gain";
    QTest::newRow("GainAtPinValue")                << "Gain"                << "Frequency" << "Pin"          << -10.5 << "Gain_at_Pin_neg10_5_dBm";
    QTest::newRow("ReverseGainAtFrequencyValue")   << "Reverse Gain"        << "Pin"       << "Frequency"    << 1.0E9 << "Reverse_Gain_at_Frequency_1_GHz";
    QTest::newRow("PinAtCompression")              << "Pin"                 << "Frequency" << "Compression"  << 0.0   << "Pin_at_Compression";
    QTest::newRow("PoutAtCompression")             << "Pout"                << "Frequency" << "Compression"  << 0.0   << "Pout_at_Compression";
}
void TraceSettingsTest::setNameFromSettings() {
    QFETCH(QString, yParameter);
    QFETCH(QString, xParameter);
    QFETCH(QString, atParameter);
    QFETCH(double,  atValue);
    QFETCH(QString, expectedName);

    TraceSettings settings;
    settings.yParameter = yParameter;
    QVERIFY(settings.isValidYParameter());

    settings.xParameter = xParameter;
    QVERIFY(settings.isValidXParameter());

    settings.atParameter = atParameter;
    QVERIFY(settings.isValidAtParameter());

    settings.atValue = atValue;
    QVERIFY(settings.isValidAtValue());

    settings.generateNameFromSettings();
    QCOMPARE(settings.name, expectedName);
}

void TraceSettingsTest::save() {
    QVector<TraceSettings> traces;

    TraceSettings settings1;
    settings1.name = "Trace1";
    settings1.yParameter = "yParameter1";
    settings1.xParameter = "xParameter1";
    settings1.atParameter = "atParameter1";
    settings1.atValue = 1.0;
    traces << settings1;

    TraceSettings settings2;
    settings2.name = "Trace2";
    settings2.yParameter = "yParameter2";
    settings2.xParameter = "xParameter2";
    settings2.atParameter = "atParameter2";
    settings2.atValue = 2.0;
    traces << settings2;

    Keys keys(_dir.path());

    QString TRACES_KEY = "TRACES";
    keys.set(TRACES_KEY, traces);

    QVector<TraceSettings> _traces;
    keys.get(TRACES_KEY, _traces);

    QCOMPARE(_traces[0].name, settings1.name);
    QCOMPARE(_traces[0].yParameter, settings1.yParameter);
    QCOMPARE(_traces[0].xParameter, settings1.xParameter);
    QCOMPARE(_traces[0].atParameter, settings1.atParameter);
    QCOMPARE(_traces[0].atValue, settings1.atValue);

    QCOMPARE(_traces[1].name, settings2.name);
    QCOMPARE(_traces[1].yParameter, settings2.yParameter);
    QCOMPARE(_traces[1].xParameter, settings2.xParameter);
    QCOMPARE(_traces[1].atParameter, settings2.atParameter);
    QCOMPARE(_traces[1].atValue, settings2.atValue);

    QCOMPARE(traces, _traces);
}
