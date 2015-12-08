#include "ProcessTraceTest.h"


// Project
#include "MeasurementData.h"
#include "ProcessTrace.h"

// Qt
#include <QTest>


ProcessTraceTest::ProcessTraceTest(QObject *parent) :
    QObject(parent)
{
    QDir sourceDir(SOURCE_DIR);
    _filename1 = sourceDir.filePath("./SampleMeasurementData/MeasurementData.bin");
    _filename2 = sourceDir.filePath("./SampleMeasurementData/SafeMeasurementData.bin");
    _filename3 = sourceDir.filePath("./SampleMeasurementData/GainExpansion/MeasurementData.bin");
    _filename4 = sourceDir.filePath("./SampleMeasurementData/GainExpansion/SafeMeasurementData.bin");
}
ProcessTraceTest::~ProcessTraceTest() {

}

void ProcessTraceTest::pinAtCompression_data() {
    QTest::addColumn<QString>("filename");
    QTest::addColumn<
}
void ProcessTraceTest::pinAtCompression() {

}
