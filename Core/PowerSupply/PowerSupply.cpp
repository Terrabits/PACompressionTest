#include "PowerSupply.h"


// RsaToolbox
#include "VisaBus.h"


PowerSupply::PowerSupply(QString resource, QString driverFilename)
  : driver(driverFilename)
{
  resetBus(new RsaToolbox::VisaBus(resource));
}


void PowerSupply::setup() {
  // preset?
  if (driver.preset) {
    preset();
    pause();
  }

  // setup scpi
  foreach (const QString &scpi, driver.setupScpi) {
    write(scpi);
  }
  pause();
}


double PowerSupply::voltage_V() {
  return query(driver.queryVoltageScpi).trimmed().toDouble();
}


double PowerSupply::current_A() {
  return query(driver.queryCurrentScpi).trimmed().toDouble();
}
