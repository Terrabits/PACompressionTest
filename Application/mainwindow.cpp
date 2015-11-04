

// %ProjectName%
#include "Settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QRect>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QApplication>


MainWindow::MainWindow(Vna &vna, Keys &keys, QWidget *parent) : 
    QMainWindow(parent),
    ui(new ::Ui::MainWindow),
    vna(vna), keys(keys),
    _isMeasuring(false)
{
    ui->setupUi(this);
    QString title = "Compression Test " + APP_VERSION;
    setWindowTitle(title);

    // Show settings initially
    ui->pages->setCurrentWidget(ui->settingsPage);

    // Frequency
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

    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));

    // Power
    const double minimum_dBm = vna.properties().minimumPower_dBm();
    const double maximum_dBm = vna.properties().maximumPower_dBm();

    ui->startPower->setParameterName("Start power");
    ui->startPower->setMinimum(minimum_dBm);
    ui->startPower->setMaximum(maximum_dBm);

    ui->stopPower->setParameterName("Stop power");
    ui->stopPower->setMinimum(minimum_dBm);
    ui->stopPower->setMaximum(maximum_dBm);

    ui->powerPoints->setParameterName("Power points");
    ui->powerPoints->setMaximum(vna.properties().maximumPoints());

    ui->compressionLevel->setParameterName("Compression level");
    ui->compressionLevel->setMinimum(0.01);
    ui->compressionLevel->setMaximum(6.0);

    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->compressionLevel, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->compressionLevel, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));

    // Miscellaneous
    QStringList channels = toStringList(vna.channels());
    ui->channel->clear();
    ui->channel->addItems(channels);

    const uint testPorts = vna.testPorts();
    ui->inputPort->setParameterName("Input port");
    ui->inputPort->setMinimum(1);
    ui->inputPort->setMaximum(testPorts);

    ui->outputPort->setParameterName("Output port");
    ui->outputPort->setMinimum(1);
    ui->outputPort->setMaximum(testPorts);

    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
}

MainWindow::~MainWindow() {
    delete ui;
}

// Private slots:
void MainWindow::on_cancel_clicked() {
    close();
}
void MainWindow::on_measure_clicked() {
    qDebug() << "Meausure";
    if (!_isMeasuring) {
        // Transition into measurement
        _isMeasuring = true;
        showProgressPage();
    }
    else {
        // Transition back
        _isMeasuring = false;
        showSettingsPage();
    }
}
void MainWindow::on_exportData_clicked() {
    qDebug() << "Export Data";
    shake();
}

QRect MainWindow::progressGeometry() const {
    const int i = QApplication::desktop()->primaryScreen();
    QRect screen = QApplication::desktop()->screen(i)->geometry();

    QPoint center = screen.center();

    const int w = 600;
    const int h = 450;

    const int x = center.x() - w/2.0;
    const int y = center.y() - h/2.0;
    QRect rect = QRect(x, y, w, h);
    return rect;
}

void MainWindow::shake() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    QRect _geometry = geometry();
    _geometry.moveRight(_geometry.right() + 10);
    animation->setStartValue(_geometry);
    animation->setEndValue(geometry());
    animation->setDuration(500);
    QEasingCurve curve(QEasingCurve::OutElastic);
    curve.setAmplitude(2);
    curve.setPeriod(0.3);
    animation->setEasingCurve(curve);
    animation->start();
}

void MainWindow::showProgressPage() {
    _settingsGeometry = geometry();
    ui->pages->setCurrentWidget(ui->progressPage);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(_settingsGeometry);
    animation->setEndValue(progressGeometry());
    animation->start();
}
void MainWindow::showSettingsPage() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(geometry());
    animation->setEndValue(_settingsGeometry);
    animation->start();

    ui->pages->setCurrentWidget(ui->settingsPage);
}
