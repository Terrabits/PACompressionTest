#include "ProgressPage.h"
#include "ui_ProgressPage.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;


ProgressPage::ProgressPage(QWidget *parent) :
    QWidget(parent),
    ui(new ::Ui::ProgressPage)
{
    ui->setupUi(this);
}

ProgressPage::~ProgressPage()
{
    delete ui;
}

void ProgressPage::startMeasurement(double startFrequency_Hz, double stopFrequency_Hz, double startPower_dBm, double stopPower_dBm) {
    initializePlot(startFrequency_Hz, stopFrequency_Hz, startPower_dBm, stopPower_dBm);
    ui->timedProgressBar->showPercentage();
    ui->progressBar->setValue(0);
}
void ProgressPage::startSweep(const QString &caption, uint time_ms) {
    ui->timedProgressBar->start(caption, time_ms);
}
void ProgressPage::stopSweep() {
    ui->timedProgressBar->stop();
}
void ProgressPage::updateTotalProgress(int percent) {
    ui->progressBar->setValue(percent);
}

void ProgressPage::plotMaxGain(const RsaToolbox::QRowVector &frequencies_Hz, const RsaToolbox::QRowVector &gain_dB) {
    const double upper = ui->plot->yAxis->range().upper;
    const double lower = ui->plot->yAxis->range().lower;

    double newUpper = max(gain_dB);
    double newLower = min(gain_dB);

    if (newUpper > upper || newLower < lower) {
        newUpper = std::max(newUpper, upper);
        newLower = std::min(newLower, lower);
        roundAxis(newLower, newUpper, 5.0, newLower, newUpper);
        ui->plot->yAxis->setRange(newLower, newUpper);
    }

    ui->plot->graph(0)->setData(frequencies_Hz, gain_dB);
    ui->plot->replot();
}
void ProgressPage::plotPinAtCompression(const RsaToolbox::QRowVector &frequencies_Hz, const RsaToolbox::QRowVector &pin_dBm) {
    ui->plot->graph(1)->setData(frequencies_Hz, pin_dBm);
    ui->plot->replot();
}
void ProgressPage::finishMeasurement() {
    ui->progressBar->setValue(100);
}

void ProgressPage::initializePlot(double startFrequency_Hz, double stopFrequency_Hz, double startPower_dBm, double stopPower_dBm) {
    ui->plot->clearGraphs();

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
    ui->plot->graph(0)->setName("Max Gain");
    ui->plot->graph(0)->setPen(QPen(Qt::blue));
    ui->plot->graph(0)->setVisible(true);

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2);
    ui->plot->graph(1)->setName("Pin[Compression]");
    ui->plot->graph(0)->setPen(QPen(Qt::red));
    ui->plot->graph(1)->setVisible(true);

    ui->plot->yAxis2->setVisible(true);
    ui->plot->legend->setVisible(true);

    ui->plot->xAxis->setRange(startFrequency_Hz, stopFrequency_Hz);
    ui->plot->xAxis->setLabel("Frequency (Hz)");

    ui->plot->yAxis->setRange(0, 100.0);
    ui->plot->yAxis->setLabel("Gain (dB)");

    ui->plot->yAxis2->setRange(startPower_dBm, stopPower_dBm);
    ui->plot->yAxis2->setLabel("Power (dBm)");
}
