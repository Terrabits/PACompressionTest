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

    double frequency_Hz() const;
    void setFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);

    uint points() const;
    RsaToolbox::QRowVector powers_dBm() const;
    RsaToolbox::ComplexRowVector sParameter(uint outputPort, uint inputPort) const;

    void addPoint(const PowerPoint &point);

    PowerSweep &operator<<(const PowerPoint &point);
    PowerPoint &operator[](int index);

private:
    double _frequency_Hz;
    QVector<PowerPoint> _points;

};

#endif // POWERSWEEP_H
