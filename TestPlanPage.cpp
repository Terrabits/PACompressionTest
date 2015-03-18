#include "TestPlanPage.h"
#include "ui_TestPlanPage.h"


// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QDebug>


TestPlanPage::TestPlanPage(QWidget *parent) :
    WizardPage(parent),
    ui(new ::Ui::TestPlanPage),
    _vna(ConnectionType::TCPIP_CONNECTION, "127.0.0.1")
{
    ui->setupUi(this);

    _plan.setVna(&_vna);

    connect(&_plan, SIGNAL(startFrequencyChanged(double)),
            ui->startFrequency, SLOT(setFrequency(double)));
    connect(&_plan, SIGNAL(stopFrequencyChanged(double)),
            ui->stopFrequency, SLOT(setFrequency(double)));
    connect(&_plan, SIGNAL(frequencyPointsChanged(uint)),
            ui->points, SLOT(setPoints(uint)));
    connect(&_plan, SIGNAL(startPowerChanged(double)),
            ui->startPower, SLOT(setValue(double)));
    connect(&_plan, SIGNAL(powerStepSizeChanged(double)),
            ui->stepSize, SLOT(setValue(double)));
    connect(&_plan, SIGNAL(absoluteStopPowerChanged(double)),
            ui->absoluteStopPower, SLOT(setValue(double)));
    connect(&_plan, SIGNAL(gainExpansionChanged(bool)),
            ui->gainExpansion, SLOT(setChecked(bool)));
    connect(&_plan, SIGNAL(compressionValueChanged(double)),
            ui->compression, SLOT(setValue(double)));

    connect(&_plan, SIGNAL(error(QString)),
            ui->error, SLOT(showMessage(QString)));
}

TestPlanPage::~TestPlanPage()
{
    delete ui;
}

bool TestPlanPage::isReadyForNext() {
    ui->error->clearMessage();
    if (!ui->startFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter a start frequency.");
        ui->startFrequency->selectAll();
        ui->startFrequency->setFocus();
        return false;
    }
    const double startFreq = ui->startFrequency->frequency_Hz();
    if (!_plan.setStartFrequency(startFreq)) {
        ui->startFrequency->selectAll();
        ui->startFrequency->setFocus();
        return false;
    }

    if (!ui->stopFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter a stop frequency.");
        ui->stopFrequency->selectAll();
        ui->stopFrequency->setFocus();
        return false;
    }
    const double stopFreq = ui->stopFrequency->frequency_Hz();
    if (!_plan.setStopFrequency(stopFreq)) {
        ui->stopFrequency->selectAll();
        ui->stopFrequency->setFocus();
        return false;
    }

    if (!ui->points->hasAcceptableInput()) {
        ui->error->showMessage("*Enter number of frequency points.");
        ui->points->selectAll();
        ui->points->setFocus();
        return false;
    }
    const uint points = ui->points->points();
    if (!_plan.setFrequencyPoints(points)) {
        ui->points->selectAll();
        ui->points->setFocus();
        return false;
    }

    if (!ui->startPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter a start power.");
        ui->startPower->selectAll();
        ui->startPower->setFocus();
        return false;
    }
    const double startPower = ui->startPower->value();
    if (!_plan.setStartPower(startPower)) {
        ui->startPower->selectAll();
        ui->startPower->setFocus();
        return false;
    }

    if (!ui->stepSize->hasAcceptableInput()) {
        ui->error->showMessage("*Enter a power step size.");
        ui->stepSize->selectAll();
        ui->stepSize->setFocus();
        return false;
    }
    const double stepSize = ui->stepSize->value();
    if (!_plan.setPowerStepSize(stepSize)) {
        ui->stepSize->selectAll();
        ui->stepSize->setFocus();
        return false;
    }

    if (!ui->absoluteStopPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter an absolute stop power.");
        ui->absoluteStopPower->selectAll();
        ui->absoluteStopPower->setFocus();
        return false;
    }
    const double stopPower = ui->absoluteStopPower->value();
    if (!_plan.setAbsoluteStopPower(stopPower)) {
        ui->absoluteStopPower->selectAll();
        ui->absoluteStopPower->setFocus();
        return false;
    }

    _plan.setGainExpansion(ui->gainExpansion->isChecked());

    if (!ui->compression->hasAcceptableInput()) {
        ui->error->showMessage("*Enter a compression value");
        ui->compression->selectAll();
        ui->compression->setFocus();
        return false;
    }
    const double compression = ui->compression->value();
    if (!_plan.setCompressionValue(compression)) {
        ui->compression->selectAll();
        ui->compression->setFocus();
        return false;
    }

    return _plan.isValid();
}

void TestPlanPage::on_testButton_clicked()
{
    qDebug() << "Is ready? " << isReadyForNext();
}
