#include "TraceSettingsTest.h"


// Project
#include "TraceSettings.h"

// Qt
#include <QTest>
#include <QString>


TraceSettingsTest::TraceSettingsTest(QObject *parent) :
    QObject(parent)
{

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
    QTest::newRow("GainAtPinValue")                << "Reverse Gain"        << "Pin"       << "Frequency"    << 1.0E9 << "Reverse_Gain_at_Frequency_1_GHz";
    QTest::newRow("GainAtPinValue")                << "Pin"                 << "Frequency" << "Compression"  << 0.0   << "Pin_at_Compression";
    QTest::newRow("GainAtPinValue")                << "Pout"                << "Frequency" << "Compression"  << 0.0   << "Pout_at_Compression";
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
