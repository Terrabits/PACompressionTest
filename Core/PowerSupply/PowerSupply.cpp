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
    write(scpi.trimmed());
  }
  pause();
}


double PowerSupply::power_W() {
  return query(driver.queryPowerScpi).trimmed().toDouble();
}
