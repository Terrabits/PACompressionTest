#include "PlotWidget.h"
#include "ui_PlotWidget.h"


// Project
#include "ProcessTrace.h"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>


PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    _model(0)
{
    ui->setupUi(this);
}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::setModel(TraceSettingsModel *model) {
    if (_model == model)
        return;

    if (_model) {
        disconnect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(replot()));
        disconnect(_model, SIGNAL(modelReset()),
                this, SLOT(replot()));
    }

    _model = model;
    if (_model) {
        _model->insertRows(0, 2);
        connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(replot()));
        connect(_model, SIGNAL(modelReset()),
                this, SLOT(replot()));
    }
}
void PlotWidget::setData(MeasurementData *data) {
    _data = data;
}

void PlotWidget::replot() {
    qDebug() << "PlotWidget::replot";
    clearPlot();

    // Legend
    ui->plot->legend->setVisible(true);
    ui->plot->legend->setFont(QFont("Helvetica", 9));
    ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignBottom);

    double y1Min = DBL_INF,
           y1Max = DBL_NEG_INF;
    double y2Min = DBL_INF,
           y2Max = DBL_NEG_INF;
    double xMin  = DBL_INF,
           xMax  = DBL_NEG_INF;
    foreach (TraceSettings t, _model->traces()) {
        ProcessTrace p(t, *_data);
        QCPGraph *graph;
        if (t.isLeftYAxis()) {
            graph = ui->plot->addGraph();
            ui->plot->yAxis->setVisible(true);
            const double newYMin = min(p.y());
            if (newYMin < y1Min)
                y1Min = newYMin;
            const double newYMax = max(p.y());
            if (newYMax > y1Max)
                y1Max = newYMax;
            const double newXMin = min(p.x());
            if (newXMin < xMin)
                xMin = newXMin;
            const double newXMax = max(p.x());
            if (newXMax > xMax)
                xMax = newXMax;
        }
        else {
            graph = ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2);
            ui->plot->yAxis2->setVisible(true);
            const double newYMin = min(p.y());
            if (newYMin < y2Min)
                y2Min = newYMin;
            const double newYMax = max(p.y());
            if (newYMax > y2Max)
                y2Max = newYMax;
            const double newXMin = min(p.x());
            if (newXMin < xMin)
                xMin = newXMin;
            const double newXMax = max(p.x());
            if (newXMax > xMax)
                xMax = newXMax;
        }
        graph->setName(p.name());
        graph->setData(p.x(), p.y());
    }

    if (y1Min < y1Max) {
        roundAxis(y1Min, y1Max, 5, y1Min, y1Max);
        ui->plot->yAxis->setRange(y1Min, y1Max);
        qDebug() << "y1 tick step: " << ui->plot->yAxis->tickStep();
        const double tickStep = ceiling(ui->plot->yAxis->tickStep(), 1);
        qDebug() << "new y1 tick step: " << tickStep;
        ui->plot->yAxis->setAutoTickStep(false);
        ui->plot->yAxis->setTickStep(tickStep);
        ui->plot->yAxis->setSubTickCount(5);
    }
    else {
        ui->plot->yAxis->rescale();
    }
    if (y2Min < y2Max) {
        roundAxis(y2Min, y2Max, 5, y2Min, y2Max);
        ui->plot->yAxis2->setRange(y2Min, y2Max);
    }
    else {
        ui->plot->yAxis2->rescale();
    }
    roundAxis(xMin, xMax, 5, xMin, xMax);
    ui->plot->xAxis->setRange(xMin, xMax);

    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->plot->setInteractions(QCP::iSelectPlottables);
    ui->plot->replot();
}
bool PlotWidget::saveToFile(QString filename) const {
    return false;
}
void PlotWidget::clearPlot() {
    ui->plot->clearGraphs();
    ui->plot->clearItems();
    ui->plot->legend->clearItems();

    ui->plot->yAxis->setAutoTickStep(true);
    ui->plot->yAxis->setAutoSubTicks(true);
    ui->plot->yAxis->setVisible(false);

    ui->plot->yAxis2->setAutoTickStep(true);
    ui->plot->yAxis2->setAutoSubTicks(true);
    ui->plot->yAxis2->setVisible(false);
}
