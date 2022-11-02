#include "HeaderTest.h"
#include "TraceSettingsTest.h"
#include "SpeedTrackerTest.h"
#include "SafeFrequencySweepTest.h"
#include "MeasureDataTest.h"
#include "ProcessTraceTest.h"

// logging
#include "logging.hpp"

// RsaToolbox
#include <General.h>
#include <TestRunner.h>
using namespace RsaToolbox;

// Core
#include "Settings.h"

// Qt
#include <QtTest>
#include <QDebug>


int main() {
    init_logging(boost::log::trivial::warning, MAIN_LOG_FILENAME_QBA.constData());
    TestRunner testRunner;
//    testRunner.addTest(new HeaderTest);
//    testRunner.addTest(new TraceSettingsTest);
//    testRunner.addTest(new SpeedTrackerTest);
//    testRunner.addTest(new SafeFrequencySweepTest);
    testRunner.addTest(new MeasureDataTest);
//    testRunner.addTest(new ProcessTraceTest);

    qDebug() << "Global result: " << (testRunner.runTests() ? "PASS" : "FAIL");
    return 0;
}
