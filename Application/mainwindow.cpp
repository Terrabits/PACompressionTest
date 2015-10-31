

// %ProjectName%
#include "Settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QMessageBox>


MainWindow::MainWindow(Vna &vna, Keys &keys, QWidget *parent) : 
    vna(vna), keys(keys),
    QMainWindow(parent), ui(new ::Ui::MainWindow)
{
    ui->setupUi(this);
    QString title = APP_NAME + " " + APP_VERSION;
    setWindowTitle(title);

    const double minimum_Hz = vna.properties().minimumFrequency_Hz();
    const double maximum_Hz = vna.properties().maximumFrequency_Hz();

    ui->startFrequency->setParameterName("Start frequency");
    ui->startFrequency->setMinimum(minimum_Hz);
    ui->startFrequency->setMaximum(maximum_Hz);

    ui->stopFrequency->setParameterName("Stop frequency");
    ui->stopFrequency->setMinimum(minimum_Hz);
    ui->stopFrequency->setMaximum(maximum_Hz);

    ui->frequencyPoints->setParameterName("Frequency points");
    ui->frequencyPoints->setMaximum(vna.properties().maximumPoints());

    ui->ifBw->setParameterName("IF BW");
    ui->ifBw->setAcceptedValues(vna.properties().ifBandwidthValues_Hz());
    
    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    const double minimum_dBm = vna.properties().minimumPower_dBm();
    const double maximum_dBm = vna.properties().maximumPower_dBm();

    ui->startPower->setParameterName("Start power");
    ui->startPower->setMinimum(minimum_dBm);
    ui->startPower->setMaximum(maximum_dBm);

    ui->stopPower->setParameterName("Stop power");
    ui->stopPower->setMinimum(minimum_dBm);
    ui->stopPower->setMaximum(maximum_dBm);

    ui->powerStepSize->setParameterName("Power step size");
    ui->powerStepSize->setMinimum(0.01);
    ui->powerStepSize->setMaximum(maximum_dBm - minimum_dBm);

    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->powerStepSize, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    ui->compressionLevel->setParameterName("Compression level");
    ui->compressionLevel->setMinimum(0.01);
    ui->compressionLevel->setMaximum(6.0);

    connect(ui->compressionLevel, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
