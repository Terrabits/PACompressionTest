

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

    QDir sourceDir(SOURCE_DIR);
    _data.open(sourceDir.filePath("measurementData.dat"));
    ui->tracesWidget->setFrequencies(_data.frequencies_Hz());
    ui->tracesWidget->setPowers(_data.pin_dBm());
    ui->tracesWidget->isTracesValid();

    ui->tracesWidget->setKeys(&keys);
    ui->tracesWidget->loadKeys();

    connect(ui->tracesWidget, SIGNAL(closeClicked()),
            this, SLOT(close()));
    connect(ui->tracesWidget, SIGNAL(plotClicked()),
            this, SLOT(plot()));
}

MainWindow::~MainWindow()
{   
    delete ui;
}

void MainWindow::plot() {
    if (!ui->tracesWidget->isTracesValid())
        return;

    // Apply traces
    QVector<TraceSettings> traces = ui->tracesWidget->traces();
    uint diagram = max(_vna.diagrams()) + 1;
    for (int i = 0; i < traces.size(); i++) {
        if (_vna.isDiagram(diagram) && _vna.diagram(diagram).traces().size() >= 20) {
            diagram++;
        }
        ProcessTrace(&(traces[i]), &_data, &_vna, diagram);
    }
    _vna.local();
    ui->tracesWidget->saveKeys();
}
