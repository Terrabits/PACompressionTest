#ifndef POWERPOINT_H
#define POWERPOINT_H


// RsaToolbox
#include "General.h"


class PowerPoint
{
public:
    PowerPoint();

    double power_dBm;
    RsaToolbox::ComplexMatrix2D sParameters;
    ComplexDouble sParameter(uint outputPort, uint inputPort);

    RsaToolbox::ComplexRowVector &operator[](int index);
};

#endif // POWERPOINT_H
