#include "quaziptest.h"

#include <TestRunner.h>
using namespace RsaToolbox;

#include <QtTest>

int main() {
    TestRunner testRunner;
    testRunner.addTest(new QuaZipTest);

    qDebug() << "Global result: " << (testRunner.runTests() ? "PASS" : "FAIL");
    return 0;
}
