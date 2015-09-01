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

    // Pretty Plot Helpers
    bool isTitle() const;
    QString generateTitle() const;
    bool isHideLegend() const;

    void generateXAxis(double &min, double &max, RsaToolbox::SiPrefix &prefix) const;
    QString generateXLabel(RsaToolbox::SiPrefix prefix) const;

    bool isLeftYAxisDegrees() const;
    void generateLeftYAxis(double &min, double &max) const;
    QString generateLeftYLabel() const;

    bool isRightYAxisDegrees() const;
    void generateRightYAxis(double &min, double &max) const;
    QString generateRightYLabel() const;



};

#endif // PLOTWIDGET_H
