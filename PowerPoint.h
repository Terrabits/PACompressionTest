#ifndef POWERPOINT_H
#define POWERPOINT_H


// RsaToolbox
#include "Definitions.h"


class PowerPoint
{
public:
    PowerPoint();
    PowerPoint(const PowerPoint &other);
    PowerPoint(double power_dBm, RsaToolbox::ComplexMatrix2D sParameters);

    double power_dBm;
    RsaToolbox::ComplexMatrix2D sParameters;

    void reset();

    RsaToolbox::ComplexRowVector &operator[](int index);
    void operator=(const PowerPoint &other);
};

#endif // POWERPOINT_H
