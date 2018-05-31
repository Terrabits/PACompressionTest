#ifndef QCustomPlotTest_H
#define QCustomPlotTest_H

#include <QCustomPlot.h>

#include <TestClass.h>
#include <QObject>

class QCustomPlotTest : public RsaToolbox::TestClass
{
    Q_OBJECT
public:
    explicit QCustomPlotTest(QObject *parent = nullptr);

private slots:
    void createPlot();
};

#endif // QCustomPlotTest_H
