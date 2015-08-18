#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H


// Project
#include "TraceSettingsModel.h"
#include "MeasurementData.h"

// QCustomPlot
#include <qcustomplot.h>

// Qt
#include <QWidget>


namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

    void setModel(TraceSettingsModel *model);
    void setData(MeasurementData *data);

public slots:
    void replot();
    bool saveToFile(QString filename) const;

private:
    Ui::PlotWidget *ui;
    QCPPlotTitle *_title;

    TraceSettingsModel *_model;
    MeasurementData *_data;
    void clearPlot();

};

#endif // PLOTWIDGET_H
