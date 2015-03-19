#ifndef RESULTS_H
#define RESULTS_H


// Project
#include "PowerSweep.h"

// RsaToolbox
#include "General.h"
#include "NetworkData.h"

// Qt
#include <QVector>
#include <QObject>


class Results : public QObject
{
    Q_OBJECT

public:
    explicit Results(QObject *parent = 0);
    ~Results();

    double compressionValue() const;
    void setCompressionValue(double value_dB);

    bool hasCompressionPoint(uint frequencyIndex) const;
    void compressionPoint(uint frequencyIndex, double &powerIn, double &powerOut);

    uint frequencyPoints() const;
    uint frequencyIndex(double frequency_Hz) const;
    RsaToolbox::QRowVector frequencies_Hz() const;

    RsaToolbox::ComplexRowVector sParameter(uint frequencyIndex, uint outputPort, uint inputPort);

    PowerSweep &operator[](int index);

signals:

    void reset(); // ?

public slots:
    void setFrequencies(RsaToolbox::QRowVector frequencies_Hz);


private:
    double _compressionValue;
    QVector<PowerSweep> _sweeps;

};

#endif // RESULTS_H
