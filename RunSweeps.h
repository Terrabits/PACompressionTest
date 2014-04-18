#ifndef RUNSWEEPS_H
#define RUNSWEEPS_H

// PA Compression Test
#include "mainwindow.h"
class MainWindow;

// Rsa
#include "Definitions.h"
#include "Vna.h"

// Qt
#include <QThread>


class RunSweeps : public QThread {
    Q_OBJECT

public:
    RunSweeps(MainWindow *window, QObject *parent = 0);
    //~RunSweeps();

signals:
    void Progress(int percent);

private slots:
    void run();

    void InitializePowerSweep();
    void GetCompressionPoint(int index);
    void ProcessPowerSweeps(QVector<RsaToolbox::NetworkData> &power_sweeps);
    void RunPowerSweeps();

    void InitializeFrequencySweep();
    void ProcessFrequencySweep();
    void FindCompressionPoints();
    void RunFrequencySweeps();

    void DisplayCompressionPoints();
    static void FlipPorts(QVector<RsaToolbox::NetworkData> &sweeps);

private:
    // From MainWindow:
    RsaToolbox::Vna *vna;
    QString sweep_mode;
    int input_port;
    int output_port;
    double start_power_dBm;
    double stop_power_dBm;
    int power_points;
    double start_freq_Hz;
    double stop_freq_Hz;
    int frequency_points;
    double compression_level_dB;
    RsaToolbox::QRowVector &frequencies_Hz;
    RsaToolbox::QRowVector &power_in_dBm;
    RsaToolbox::QRowVector &compression_points_in_dBm;
    RsaToolbox::QRowVector &compression_points_out_dBm;
    RsaToolbox::QRowVector &compression_frequencies_Hz;
    RsaToolbox::QMatrix2D &gain_dB;
    QVector<RsaToolbox::NetworkData> &s_parameter_data;

    QVector<uint> ports;
};


#endif // RUNSWEEPS_H
