#include "PowerSupply.h"


// RsaToolbox
#include "VisaBus.h"


PowerSupply::PowerSupply(QString resource, QString driverFilename)
  : driver(driverFilename)
{
  // mock edit
  // resetBus(new RsaToolbox::VisaBus(resource));
}


// mock edit
QString PowerSupply::idString() {
  return "Rohde-Schwarz,NGX10XX,1234567890,1.0";
}


void PowerSupply::setup() {
  // mock edit
  // // preset?
  // if (driver.preset) {
  //   preset();
  //   pause();
  // }
  //
  // // setup scpi
  // foreach (const QString &scpi, driver.setupScpi) {
  //   write(scpi);
  // }
  // pause();
}


double PowerSupply::voltage_V() {
  // mock edit
  return 5.0;

  // return query(driver.queryVoltageScpi).trimmed().toDouble();
}


double PowerSupply::current_A() {
  // mock edit
  return 1.0;

  // return query(driver.queryCurrentScpi).trimmed().toDouble();
}
