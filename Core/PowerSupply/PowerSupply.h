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
      QString resource,
      QString driverFilename
    );


    PowerSupplyDriver driver;


    // functionality
    void setup();
    double voltage_V();
    double current_A();

};


#endif // POWERSUPPLY_H
