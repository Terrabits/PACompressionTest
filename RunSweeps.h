#ifndef RUNSWEEPS_H
#define RUNSWEEPS_H

// Rsa
#include "Definitions.h"
#include "Vna.h"

// Qt
#include <QThread>

class RunSweeps : public QThread {
    Q_OBJECT

public:
    RunSweeps(RsaToolbox::Vna &vna,
              QVector<uint> ports,
              RsaToolbox::QRowVector &frequency_points_Hz,
              RsaToolbox::QRowVector &power_points_dBm,
              RsaToolbox::QMatrix2D &power_sweeps_dBm,
              QVector<RsaToolbox::NetworkData> &s_parameter_data,
              QObject *parent = 0);
    //~RunSweeps();

signals:
    void Progress(int percent);

private slots:
    void run();
    void FlipPorts();

private:
    RsaToolbox::Vna &vna;
    QVector<uint> ports;
    RsaToolbox::QRowVector &frequency_points_Hz;
    RsaToolbox::QRowVector &power_points_dBm;
    RsaToolbox::QMatrix2D &power_sweeps_dBm;
    QVector<RsaToolbox::NetworkData> &s_parameter_data;
};


#endif // RUNSWEEPS_H
