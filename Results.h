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

    bool isGainExpansionMode() const;
    void setGainExpansionMode(bool on);

    bool isCompressionValue() const;
    double compressionValue() const;
    void setCompressionValue(double value_dB);

    bool hasCompressionPoint(uint frequencyIndex) const;
    void compressionPoint(uint frequencyIndex, double &powerIn, double &powerOut);

    uint frequencyPoints() const;
    RsaToolbox::QRowVector frequencies_Hz() const;

    RsaToolbox::ComplexRowVector sParameter(uint frequencyIndex, uint outputPort, uint inputPort);

    PowerSweep &operator[](int index);

signals:
    void dataReset();

public slots:
    void reset();


private:
    bool _isGainExpansionMode;
    double _compressionValue;
    QVector<PowerSweep> _sweeps;

};

#endif // RESULTS_H
