#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

// Project
#include "PowerSupplyDriver.h"


// RsaToolbox
#include <Definitions.h>
#include <GenericInstrument.h>


// Qt
#include <QScopedPointer>
#include <QString>


class PowerSupply : public RsaToolbox::GenericInstrument
{

public:

    PowerSupply(
      RsaToolbox::ConnectionType type,
      QString address,
      QString driverFilename
    );


    PowerSupplyDriver driver;


    // functionality
    void setup();
    double power_W();


    // errors
    QStringList errors();

};


#endif // POWERSUPPLY_H
