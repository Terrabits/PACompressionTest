

// Project
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Settings.h"
#include "TraceSettings.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plot() {

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
