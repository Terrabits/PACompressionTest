#include "PowerSupply.h"


PowerSupply::PowerSupply(
  RsaToolbox::ConnectionType type,
  QString address,
  QString driverFilename
) :
  GenericInstrument(type, address),
  driver(driverFilename)
{
  // noop
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
