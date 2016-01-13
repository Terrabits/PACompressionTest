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

void TraceSettingsTest::isValid_data() {
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("yParameter");
    QTest::addColumn<QString>("xParameter");
    QTest::addColumn<QString>("atParameter");
    QTest::addColumn<double>("atValue");
    QTest::addColumn<bool>("isValid");

    // S11
    //            Row Name                   Trace Name               YParam   XParam         @Param                   isValid?
    QTest::newRow("S11_vs_Freq_at_Freq")  << "S11_vs_Freq_at_Freq" << "S11" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("S11_vs_Freq_at_Pin")   << "S11_vs_Freq_at_Pin"  << "S11" << "Frequency" << "Pin"          << 1.0 << true;
    QTest::newRow("S11_vs_Freq_at_Comp")  << "S11_vs_Freq_at_Comp" << "S11" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("S11_vs_Freq_at_MaxG")  << "S11_vs_Freq_at_MaxG" << "S11" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("S11_vs_Pin_at_Freq")   << "S11_vs_Pin_at_Freq"  << "S11" << "Pin"       << "Frequency"    << 1.0 << true;
    QTest::newRow("S11_vs_Pin_at_Pin")    << "S11_vs_Pin_at_Pin"   << "S11" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("S11_vs_Pin_at_Comp")   << "S11_vs_Pin_at_Comp"  << "S11" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("S11_vs_Pin_at_MaxG")   << "S11_vs_Pin_at_MaxG"  << "S11" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("S11_vs_Pout_at_Freq")  << "S11_vs_Pout_at_Freq" << "S11" << "Pout"      << "Frequency"    << 1.0 << true;
    QTest::newRow("S11_vs_Pout_at_Pin")   << "S11_vs_Pout_at_Pin"  << "S11" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("S11_vs_Pout_at_Comp")  << "S11_vs_Pout_at_Comp" << "S11" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("S11_vs_Pout_at_MaxG")  << "S11_vs_Pout_at_MaxG" << "S11" << "Pout"      << "Maximum Gain" << 1.0 << false;

    // S21
    //            Row Name                   Trace Name               YParam   XParam         @Param                   isValid?
    QTest::newRow("S21_vs_Freq_at_Freq")  << "S21_vs_Freq_at_Freq" << "S21" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("S21_vs_Freq_at_Pin")   << "S21_vs_Freq_at_Pin"  << "S21" << "Frequency" << "Pin"          << 1.0 << true;
    QTest::newRow("S21_vs_Freq_at_Comp")  << "S21_vs_Freq_at_Comp" << "S21" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("S21_vs_Freq_at_MaxG")  << "S21_vs_Freq_at_MaxG" << "S21" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("S21_vs_Pin_at_Freq")   << "S21_vs_Pin_at_Freq"  << "S21" << "Pin"       << "Frequency"    << 1.0 << true;
    QTest::newRow("S21_vs_Pin_at_Pin")    << "S21_vs_Pin_at_Pin"   << "S21" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("S21_vs_Pin_at_Comp")   << "S21_vs_Pin_at_Comp"  << "S21" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("S21_vs_Pin_at_MaxG")   << "S21_vs_Pin_at_MaxG"  << "S21" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("S21_vs_Pout_at_Freq")  << "S21_vs_Pout_at_Freq" << "S21" << "Pout"      << "Frequency"    << 1.0 << true;
    QTest::newRow("S21_vs_Pout_at_Pin")   << "S21_vs_Pout_at_Pin"  << "S21" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("S21_vs_Pout_at_Comp")  << "S21_vs_Pout_at_Comp" << "S21" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("S21_vs_Pout_at_MaxG")  << "S21_vs_Pout_at_MaxG" << "S21" << "Pout"      << "Maximum Gain" << 1.0 << false;

    // S12
    //            Row Name                   Trace Name               YParam   XParam         @Param                   isValid?
    QTest::newRow("S12_vs_Freq_at_Freq")  << "S12_vs_Freq_at_Freq" << "S12" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("S12_vs_Freq_at_Pin")   << "S12_vs_Freq_at_Pin"  << "S12" << "Frequency" << "Pin"          << 1.0 << true;
    QTest::newRow("S12_vs_Freq_at_Comp")  << "S12_vs_Freq_at_Comp" << "S12" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("S12_vs_Freq_at_MaxG")  << "S12_vs_Freq_at_MaxG" << "S12" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("S12_vs_Pin_at_Freq")   << "S12_vs_Pin_at_Freq"  << "S12" << "Pin"       << "Frequency"    << 1.0 << true;
    QTest::newRow("S12_vs_Pin_at_Pin")    << "S12_vs_Pin_at_Pin"   << "S12" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("S12_vs_Pin_at_Comp")   << "S12_vs_Pin_at_Comp"  << "S12" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("S12_vs_Pin_at_MaxG")   << "S12_vs_Pin_at_MaxG"  << "S12" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("S12_vs_Pout_at_Freq")  << "S12_vs_Pout_at_Freq" << "S12" << "Pout"      << "Frequency"    << 1.0 << true;
    QTest::newRow("S12_vs_Pout_at_Pin")   << "S12_vs_Pout_at_Pin"  << "S12" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("S12_vs_Pout_at_Comp")  << "S12_vs_Pout_at_Comp" << "S12" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("S12_vs_Pout_at_MaxG")  << "S12_vs_Pout_at_MaxG" << "S12" << "Pout"      << "Maximum Gain" << 1.0 << false;

    // S22
    //            Row Name                   Trace Name               YParam   XParam         @Param                   isValid?
    QTest::newRow("S22_vs_Freq_at_Freq")  << "S22_vs_Freq_at_Freq" << "S22" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("S22_vs_Freq_at_Pin")   << "S22_vs_Freq_at_Pin"  << "S22" << "Frequency" << "Pin"          << 1.0 << true;
    QTest::newRow("S22_vs_Freq_at_Comp")  << "S22_vs_Freq_at_Comp" << "S22" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("S22_vs_Freq_at_MaxG")  << "S22_vs_Freq_at_MaxG" << "S22" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("S22_vs_Pin_at_Freq")   << "S22_vs_Pin_at_Freq"  << "S22" << "Pin"       << "Frequency"    << 1.0 << true;
    QTest::newRow("S22_vs_Pin_at_Pin")    << "S22_vs_Pin_at_Pin"   << "S22" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("S22_vs_Pin_at_Comp")   << "S22_vs_Pin_at_Comp"  << "S22" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("S22_vs_Pin_at_MaxG")   << "S22_vs_Pin_at_MaxG"  << "S22" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("S22_vs_Pout_at_Freq")  << "S22_vs_Pout_at_Freq" << "S22" << "Pout"      << "Frequency"    << 1.0 << true;
    QTest::newRow("S22_vs_Pout_at_Pin")   << "S22_vs_Pout_at_Pin"  << "S22" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("S22_vs_Pout_at_Comp")  << "S22_vs_Pout_at_Comp" << "S22" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("S22_vs_Pout_at_MaxG")  << "S22_vs_Pout_at_MaxG" << "S22" << "Pout"      << "Maximum Gain" << 1.0 << false;

    // Pin
    //            Row Name                   Trace Name               YParam   XParam         @Param                   isValid?
    QTest::newRow("Pin_vs_Freq_at_Freq")  << "Pin_vs_Freq_at_Freq" << "Pin" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("Pin_vs_Freq_at_Pin")   << "Pin_vs_Freq_at_Pin"  << "Pin" << "Frequency" << "Pin"          << 1.0 << false;
    QTest::newRow("Pin_vs_Freq_at_Comp")  << "Pin_vs_Freq_at_Comp" << "Pin" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("Pin_vs_Freq_at_MaxG")  << "Pin_vs_Freq_at_MaxG" << "Pin" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("Pin_vs_Pin_at_Freq")   << "Pin_vs_Pin_at_Freq"  << "Pin" << "Pin"       << "Frequency"    << 1.0 << false;
    QTest::newRow("Pin_vs_Pin_at_Pin")    << "Pin_vs_Pin_at_Pin"   << "Pin" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("Pin_vs_Pin_at_Comp")   << "Pin_vs_Pin_at_Comp"  << "Pin" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("Pin_vs_Pin_at_MaxG")   << "Pin_vs_Pin_at_MaxG"  << "Pin" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("Pin_vs_Pout_at_Freq")  << "Pin_vs_Pout_at_Freq" << "Pin" << "Pout"      << "Frequency"    << 1.0 << false; // Possible, but not monotonic?
    QTest::newRow("Pin_vs_Pout_at_Pin")   << "Pin_vs_Pout_at_Pin"  << "Pin" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("Pin_vs_Pout_at_Comp")  << "Pin_vs_Pout_at_Comp" << "Pin" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("Pin_vs_Pout_at_MaxG")  << "Pin_vs_Pout_at_MaxG" << "Pin" << "Pout"      << "Maximum Gain" << 1.0 << false;

    // Pout
    //            Row Name                   Trace Name                YParam    XParam         @Param                   isValid?
    QTest::newRow("Pout_vs_Freq_at_Freq") << "Pout_vs_Freq_at_Freq" << "Pout" << "Frequency" << "Frequency"    << 1.0 << false;
    QTest::newRow("Pout_vs_Freq_at_Pin")  << "Pout_vs_Freq_at_Pin"  << "Pout" << "Frequency" << "Pin"          << 1.0 << true;
    QTest::newRow("Pout_vs_Freq_at_Comp") << "Pout_vs_Freq_at_Comp" << "Pout" << "Frequency" << "Compression"  << 1.0 << true;
    QTest::newRow("Pout_vs_Freq_at_MaxG") << "Pout_vs_Freq_at_MaxG" << "Pout" << "Frequency" << "Maximum Gain" << 1.0 << true;
    QTest::newRow("Pout_vs_Pin_at_Freq")  << "Pout_vs_Pin_at_Freq"  << "Pout" << "Pin"       << "Frequency"    << 1.0 << true;
    QTest::newRow("Pout_vs_Pin_at_Pin")   << "Pout_vs_Pin_at_Pin"   << "Pout" << "Pin"       << "Pin"          << 1.0 << false;
    QTest::newRow("Pout_vs_Pin_at_Comp")  << "Pout_vs_Pin_at_Comp"  << "Pout" << "Pin"       << "Compression"  << 1.0 << false;
    QTest::newRow("Pout_vs_Pin_at_MaxG")  << "Pout_vs_Pin_at_MaxG"  << "Pout" << "Pin"       << "Maximum Gain" << 1.0 << false;
    QTest::newRow("Pout_vs_Pout_at_Freq") << "Pout_vs_Pout_at_Freq" << "Pout" << "Pout"      << "Frequency"    << 1.0 << false;
    QTest::newRow("Pout_vs_Pout_at_Pin")  << "Pout_vs_Pout_at_Pin"  << "Pout" << "Pout"      << "Pin"          << 1.0 << false;
    QTest::newRow("Pout_vs_Pout_at_Comp") << "Pout_vs_Pout_at_Comp" << "Pout" << "Pout"      << "Compression"  << 1.0 << false;
    QTest::newRow("Pout_vs_Pout_at_MaxG") << "Pout_vs_Pout_at_MaxG" << "Pout" << "Pout"      << "Maximum Gain" << 1.0 << false;
}
void TraceSettingsTest::isValid() {
    QFETCH(QString, name);
    QFETCH(QString, yParameter);
    QFETCH(QString, xParameter);
    QFETCH(QString, atParameter);
    QFETCH(double,  atValue);
    QFETCH(bool,    isValid);

    TraceSettings trace;
    trace.name = name;
    trace.yParameter = yParameter;
    trace.xParameter = xParameter;
    trace.atParameter = atParameter;
    trace.atValue = atValue;
    QCOMPARE(trace.isValid(), isValid);
}

void TraceSettingsTest::setNameFromSettings_data() {
    QTest::addColumn<QString>("yParameter");
    QTest::addColumn<QString>("xParameter");
    QTest::addColumn<QString>("atParameter");
    QTest::addColumn<double>("atValue");
    QTest::addColumn<QString>("expectedName");

    QTest::newRow("S11_at_Comp")  << "S11"  << "Frequency" << "Compression"  << 0.0   << "S11_at_Compression";
    QTest::newRow("S22_at_MaxG")  << "S22"  << "Frequency" << "Maximum Gain" << 0.0   << "S22_at_Maximum_Gain";
    QTest::newRow("S21_at_Pin")   << "S21"  << "Frequency" << "Pin"          << -10.5 << "S21_at_Pin_neg10_5_dBm";
    QTest::newRow("S12_at_Freq")  << "S12"  << "Pin"       << "Frequency"    << 1.0E9 << "S12_at_Frequency_1_GHz";
    QTest::newRow("Pin_at_Comp")  << "Pin"  << "Frequency" << "Compression"  << 0.0   << "Pin_at_Compression";
    QTest::newRow("Pout_at_Comp") << "Pout" << "Frequency" << "Compression"  << 0.0   << "Pout_at_Compression";
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
