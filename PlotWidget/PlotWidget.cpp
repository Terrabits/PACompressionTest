#include "PlotWidget.h"
#include "ui_PlotWidget.h"


// Project
#include "ProcessTrace.h"

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
    QStringList legend;
    foreach (TraceSettings t, _model->traces()) {
        // Generate plot(s)
        ProcessTrace p(t, *_data);
        qDebug() << "  " << p.name();
        legend << p.name();
    }
}
bool PlotWidget::saveToFile(QString filename) const {
    return false;
}
void PlotWidget::clearPlot() {

}
