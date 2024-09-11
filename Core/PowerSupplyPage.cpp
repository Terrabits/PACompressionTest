#include "PowerSupplyPage.h"
#include "ui_PowerSupplyPage.h"


// project
#include "Settings.h"


// RsaToolbox
using namespace RsaToolbox;


// qt
#include <QFileInfo>
#include <QFileDialog>


PowerSupplyPage::PowerSupplyPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PowerSupplyPage)
{
    ui->setupUi(this);

    // driver widget
    ui->driverWidget->setFocusProxy(ui->getDriverButton);

    // init last path
    _lastPath = newLastPath(DRIVER_PATH);

    // is pae on signals and slots
    connect(ui->paeOn,             SIGNAL(toggled(bool)),
            ui->powerSupplyWidget, SLOT(setEnabled(bool)));
    connect(ui->getDriverButton, SIGNAL(clicked()),
            this,                SLOT(getDriverDialog()));

    // button click signals and slots
    connect(ui->exportButton, SIGNAL(clicked()),
            this, SIGNAL(exportClicked()));
    connect(ui->miniGuiButton, SIGNAL(clicked()),
            this, SIGNAL(miniGuiClicked()));
    connect(ui->closeButton, SIGNAL(clicked()),
            this, SIGNAL(closeClicked()));
    connect(ui->measureButton, SIGNAL(clicked()),
            this, SIGNAL(measureClicked()));
}


PowerSupplyPage::~PowerSupplyPage()
{
    delete ui;
}


void PowerSupplyPage::setKeys(Keys *keys) {
  _keys = keys;
}


void PowerSupplyPage::loadKeys() {
  if (!_keys) {
    return;
  }

  if (_keys->exists(PAE_ON_KEY)) {
    bool isOn;
    _keys->get(PAE_ON_KEY, isOn);
    setPAEOn(isOn);
  }

  if (_keys->exists(POWER_SUPPLY_RESOURCE_KEY)) {
    QString resource;
    _keys->get(POWER_SUPPLY_RESOURCE_KEY, resource);
    setVisaResource(resource);
  }

  if (_keys->exists(POWER_SUPPLY_DRIVER_KEY)) {
    QString filePath;
    _keys->get(POWER_SUPPLY_DRIVER_KEY, filePath);
    setDriver(filePath);
  }
}


void PowerSupplyPage::saveKeys() {
  if (!_keys) {
    return;
  }

  _keys->set(PAE_ON_KEY, isPAEOn());
  _keys->set(POWER_SUPPLY_RESOURCE_KEY, visaResource());
  _keys->set(POWER_SUPPLY_DRIVER_KEY, driverFilePath());
}


bool PowerSupplyPage::isPAEOn() const {
  return ui->paeOn->isChecked();
}


void PowerSupplyPage::setPAEOn(bool isOn) {
  ui->paeOn->setChecked(isOn);
}


QString PowerSupplyPage::visaResource() const {
  return ui->visaResource->text().trimmed();
}


void PowerSupplyPage::setVisaResource(const QString& resource) {
  ui->visaResource->setText(resource);
}


QString PowerSupplyPage::driverFilePath() const {
  return _driverFilePath;
}


bool PowerSupplyPage::hasAcceptableInput() {
  if (!isPAEOn()) {
    // nothing to check
    return true;
  }

  // visa resource
  if (visaResource().trimmed().isEmpty()) {
    const QString message = "*Enter power supply VISA resource";
    ui->error->showMessage(message);
    ui->visaResource->selectAll();
    ui->visaResource->setFocus();
    emit inputError(message);
    return false;
  }

  // driver file
  if (driverFilePath().trimmed().isEmpty()) {
    const QString message = "*Enter power supply driver file";
    ui->error->showMessage(message);
    ui->driverWidget->setFocus();
    emit inputError(message);
    return false;
  }

  QFileInfo driverInfo(driverFilePath());
  if (!driverInfo.exists() || !driverInfo.isFile()) {
    const QString message ="*Power supply driver file not found";
    ui->error->showMessage(message);
    ui->driverWidget->setFocus();
    emit inputError(message);
    return false;
  }

  // has acceptable input
  return true;
}


ErrorLabel* PowerSupplyPage::errorLabel() {
  return ui->error;
}


void PowerSupplyPage::getDriverDialog() {
  // initial path
  QString dir;
  if (!_driverFilePath.isEmpty()) {
    dir = driverPath();
  }
  else {
    dir = _lastPath->toString();
  }

  // request file from user (dialog)
  QString result = QFileDialog::getOpenFileName(
      this,
      "Choose driver file...",
      dir,
      "Power Supply Driver file (*.json)"
  );

  // user file?
  if (!result.isEmpty()) {
    setDriver(result);
    saveKeys();
  }
}


void PowerSupplyPage::enableExport() {
  ui->exportButton->setEnabled(true);
}


void PowerSupplyPage::disableExport() {
  ui->exportButton->setDisabled(true);
}


QString PowerSupplyPage::driverFileName() const {
  return QFileInfo(_driverFilePath).fileName();
}


QString PowerSupplyPage::driverPath() const {
  return QFileInfo(_driverFilePath).path();
}


void PowerSupplyPage::setDriver(const QString& filePath) {
  _driverFilePath = filePath;
  _lastPath->setFromFilePath(filePath);
  ui->driverFilename->setText(driverFileName());
}
