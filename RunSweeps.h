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
              uint power_channel,
              uint freq_channel,
              QVector<uint> ports,
              RsaToolbox::QRowVector &frequency_points_Hz,
              RsaToolbox::QRowVector &power_points_dBm,
              RsaToolbox::QMatrix2D &power_sweeps_dBm,
              RsaToolbox::QRowVector &compression_points_in_dBm,
              RsaToolbox::QRowVector &compression_points_out_dBm,
              QVector<RsaToolbox::NetworkData> &s_parameter_data,
              QObject *parent = 0);
    //~RunSweeps();

signals:
    void Progress(int percent);

private slots:
    void run();
    void SetupPowerSweep(int index);
    void SetupFreqSweep(int index);
    void RetrievePowerSweep(int index);
    void RetrieveFreqSweep(int index);
    void FlipPorts();

private:
    RsaToolbox::Vna &vna;
    uint power_channel;
    uint freq_channel;
    QVector<uint> ports;
    int freq_points;
    int power_points;
    RsaToolbox::QRowVector &frequency_points_Hz;
    RsaToolbox::QRowVector &power_points_dBm;

    RsaToolbox::QMatrix2D &power_sweeps_dBm;
    RsaToolbox::QRowVector &compression_points_in_dBm;
    RsaToolbox::QRowVector &compression_points_out_dBm;
    QVector<RsaToolbox::NetworkData> &s_parameter_data;
};


#endif // RUNSWEEPS_H
