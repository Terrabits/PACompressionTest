

// Tests
#include "SafeFrequencySweepTest.h"

// RsaToolbox
#include "TestRunner.h"
using namespace RsaToolbox;

// Qt
#include <QtTest>
#include <QDebug>


int main() {
    TestRunner testRunner;
    testRunner.addTest(new SafeFrequencySweepTest);

    qDebug() << "Global result: " << (testRunner.runTests() ? "PASS" : "FAIL");
    return 0;
}
