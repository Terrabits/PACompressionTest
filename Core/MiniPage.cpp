#include "MiniPage.h"
#include "ui_MiniPage.h"


// Project
#include "Settings.h"

// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QMessageBox>
#include <QPropertyAnimation>


MiniPage::MiniPage(QWidget *parent) :
    QWidget(parent),
    ui(new ::Ui::MiniPage)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

    ui->progressLabel->clear();

    connect(ui->dragWidget, SIGNAL(dragged(int,int)),
            this, SLOT(drag(int,int)));

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

void MiniPage::drag(int x, int y) {
    move(x, y);
}
void MiniPage::animateMove(int x, int y) {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    connect(animation, SIGNAL(finished()),
            this, SIGNAL(animationFinished()));

    const QRect currentGeometry = geometry();
    QRect newGeometry = currentGeometry;
    newGeometry.moveTo(x, y);

    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(currentGeometry);
    animation->setEndValue(newGeometry);
    animation->start();
}

void MiniPage::showError(const QString &message) {
    QMessageBox::critical(this, APP_NAME, message);
}

void MiniPage::startMeasurement() {
    ui->standardGuiButton->setDisabled(true);
    ui->exportButton->setDisabled(true);

    ui->progressLabel->setText("0%");
}
void MiniPage::startSweep(const QString &caption, uint time_ms) {
    Q_UNUSED(caption);
    Q_UNUSED(time_ms);
}
void MiniPage::stopSweep() {

}
void MiniPage::updateTotalProgress(int percent) {
    ui->progressLabel->setText(QString("%1%").arg(percent));
}
void MiniPage::finishMeasurement() {
    ui->progressLabel->clear();
    ui->standardGuiButton->setEnabled(true);
}

void MiniPage::enableExport() {
    ui->exportButton->setEnabled(true);
}
void MiniPage::disableExport() {
    ui->exportButton->setDisabled(true);
}
