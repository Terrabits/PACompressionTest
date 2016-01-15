

// Project
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Settings.h"
#include "TraceSettings.h"
#include "ProcessTrace.h"

// RsaToolbox
using namespace RsaToolbox;


MainWindow::MainWindow(Vna &vna, Keys &keys, QWidget *parent) :
    QMainWindow(parent),
    ui(new ::Ui::MainWindow),
    _vna(vna),
    _keys(keys)
{
    ui->setupUi(this);

    connect(ui->tracesWidget, SIGNAL(error(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->button, SIGNAL(clicked()),
            this, SLOT(plot()));

    _data.open(QDir(SOURCE_DIR).filePath("measurementData.dat"));
    ui->tracesWidget->setFrequencies(_data.frequencies_Hz());
    ui->tracesWidget->setPowers(_data.pin_dBm());
    loadKeys();
    ui->tracesWidget->isTracesValid();
}

MainWindow::~MainWindow()
{
    saveKeys();
    delete ui;
}

void MainWindow::plot() {
    if (!ui->tracesWidget->isTracesValid())
        return;

    QVector<TraceSettings> traces = ui->tracesWidget->traces();
    uint diagram = max(_vna.diagrams()) + 1;
    for (int i = 0; i < traces.size(); i++) {
        if (_vna.isDiagram(diagram) && _vna.diagram(diagram).traces().size() >= 20) {
            diagram++;
        }
        ProcessTrace(&(traces[i]), &_data, &_vna, diagram);
    }
    _vna.local();
    saveKeys();
}

void MainWindow::loadKeys() {
    if (_keys.exists(TRACES_KEY)) {
        QVector<TraceSettings> traces;
        _keys.get(TRACES_KEY, traces);
        ui->tracesWidget->setTraces(traces);
    }
}
void MainWindow::saveKeys() {
    _keys.set(TRACES_KEY, ui->tracesWidget->traces());
}
