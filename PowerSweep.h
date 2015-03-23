#ifndef POWERSWEEP_H
#define POWERSWEEP_H


// Project
#include "PowerPoint.h"

// RsaToolbox
#include "Definitions.h"

// Qt
#include <QVector>


class PowerSweep
{
public:

    PowerSweep();
    PowerSweep(const PowerSweep &other);
    PowerSweep(double frequency_Hz, RsaToolbox::QRowVector powers_dBm, RsaToolbox::ComplexMatrix3D sParameters);

    double frequency_Hz() const;
    void setFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);

    uint points() const;
    RsaToolbox::QRowVector powers_dBm() const;
    RsaToolbox::ComplexRowVector sParameter(uint outputPort, uint inputPort) const;

    void addPoints(RsaToolbox::QRowVector powers_dBm, RsaToolbox::ComplexMatrix3D sParameters);


    PowerSweep &operator<<(const PowerPoint &point);
    PowerPoint &operator[](int index);
    void operator=(const PowerSweep &other);

    void reset();

private:
    double _frequency_Hz;
    QVector<PowerPoint> _points;

};

#endif // POWERSWEEP_H
