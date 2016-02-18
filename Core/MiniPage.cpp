#include "MiniPage.h"
#include "ui_MiniPage.h"


// RsaToolbox
using namespace RsaToolbox;


MiniPage::MiniPage(QWidget *parent) :
    QWidget(parent),
    ui(new ::Ui::MiniPage)
{
    ui->setupUi(this);

    ui->timedProgressBar->hidePercentage();
    ui->progressBar->setVisible(false);
    ui->timedProgressBar->setVisible(false);

    connect(ui->exportButton, SIGNAL(clicked()),
            this, SIGNAL(exportClicked()));
    connect(ui->standardGuiButton, SIGNAL(clicked()),
            this, SIGNAL(standardGuiClicked()));
    connect(ui->closeButton, SIGNAL(clicked()),
            this, SIGNAL(closeClicked()));
    connect(ui->measureAndPlotButton, SIGNAL(clicked()),
            this, SIGNAL(measureAndPlotClicked()));
}

MiniPage::~MiniPage()
{
    delete ui;
}

ErrorLabel *MiniPage::errorLabel() {
    return ui->error;
}

void MiniPage::startMeasurement() {
    ui->standardGuiButton->setDisabled(true);

    ui->timedProgressBar->setVisible(true);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
}
void MiniPage::startSweep(const QString &caption, uint time_ms) {
    ui->timedProgressBar->start(caption, time_ms);
}
void MiniPage::stopSweep() {
    ui->timedProgressBar->stop();
}
void MiniPage::updateTotalProgress(int percent) {
    ui->progressBar->setValue(percent);
}
void MiniPage::finishMeasurement() {
    ui->progressBar->setValue(100);
    ui->progressBar->setVisible(false);
    ui->timedProgressBar->setVisible(false);
    ui->standardGuiButton->setEnabled(true);
}

void MiniPage::enableExport() {
    ui->exportButton->setEnabled(true);
}
void MiniPage::disableExport() {
    ui->exportButton->setDisabled(true);
}
