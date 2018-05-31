#include "qcustomplottest.h"

#include "qcustomplot.h"

#include <Test.h>
using namespace RsaToolbox;

#include <QVariant>
#include <QVariantList>

QCustomPlotTest::QCustomPlotTest(QObject *parent) : TestClass(parent)
{

}

void QCustomPlotTest::createPlot() {
    QCustomPlot plot;

    plot.show();
    pause("Is this correct?");
}
