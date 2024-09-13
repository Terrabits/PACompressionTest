#ifndef HELPERS_H
#define HELPERS_H


// RsaToolbox
#include "Definitions.h"


RsaToolbox::QRowVector add(
    double base,
    const RsaToolbox::QRowVector& exponent
);


double dcPower_mW(double voltage_V, double current_A);


RsaToolbox::QRowVector dcPower_mW(
    const RsaToolbox::QRowVector& voltage_V,
    const RsaToolbox::QRowVector& current_A
);


double dBm_mW(double power_dBm);


RsaToolbox::QRowVector dBm_mW(const RsaToolbox::QRowVector& power_dBm);


#endif  // HELPERS_H
