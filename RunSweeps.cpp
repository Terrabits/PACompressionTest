#include <QDebug>


#include "RunSweeps.h"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>

// C++ std lib
#include <complex>

// Qt
#include <QApplication>

using namespace RsaToolbox;


RunSweeps::RunSweeps(Vna &vna,
                     uint power_channel,
                     uint freq_channel,
                     QVector<uint> ports,
                     QRowVector &frequency_points_Hz,
                     QRowVector &power_points_dBm,
                     QMatrix2D &power_sweeps_dBm,
                     QRowVector &compression_points_dBm,
                     QRowVector &compression_points_out_dBm,
                     QVector<NetworkData> &s_parameter_data,
                     QObject *parent)
    : vna(vna),
      power_channel(power_channel),
      freq_channel(freq_channel),
      ports(ports),
      frequency_points_Hz(frequency_points_Hz),
      power_points_dBm(power_points_dBm),
      power_sweeps_dBm(power_sweeps_dBm),
      compression_points_in_dBm(compression_points_dBm),
      compression_points_out_dBm(compression_points_out_dBm),
      s_parameter_data(s_parameter_data),
      QThread(parent)
{
    // Not much else to do...
}

void RunSweeps::run() {
    freq_points = frequency_points_Hz.size();
    power_points = power_points_dBm.size();

    power_sweeps_dBm.resize(freq_points);
    compression_points_in_dBm.resize(freq_points);
    compression_points_out_dBm.resize(freq_points);
    s_parameter_data.resize(power_points);

    int total_sweeps = qMax(freq_points, power_points);
    for (int i = 0; i < total_sweeps; i++) {
        SetupFreqSweep(i);
        SetupPowerSweep(i);
        vna.InitiateSweeps();
        vna.PauseUntilCommandQueueIsFinished();
        RetrieveFreqSweep(i);
        RetrievePowerSweep(i);
        emit Progress(((i+1.0)/total_sweeps)*100);
    }

    // Vna requires ports in ascending order. I will reorder the
    //   returned data so that s21 is always the gain of the PA.
    if (ports[1] < ports[0])
        FlipPorts();

    vna.moveToThread(QApplication::instance()->thread());
}

void RunSweeps::SetupFreqSweep(int index) {
    if (index < power_points)
        vna.Channel(freq_channel).SetChannelPower_dBm(power_points_dBm[index]);
}
void RunSweeps::SetupPowerSweep(int index) {
    if (index < freq_points)
        vna.Channel(power_channel).SetCwFrequency(frequency_points_Hz[index]);
}
void RunSweeps::RetrieveFreqSweep(int index) {
    if (index < power_points)
        vna.Channel(freq_channel).GetSParameterGroupData(s_parameter_data[index]);
}
void RunSweeps::RetrievePowerSweep(int index) {
    if (index < freq_points) {
        TraceData trace;
        vna.Trace().MeasureTrace(trace);
        power_sweeps_dBm[index] = QRowVector::fromStdVector(trace.data);
        vna.Channel(power_channel).GetCompressionPoints(compression_points_in_dBm[index],
                                                        compression_points_out_dBm[index]);
        if (vna.isError()) {
            compression_points_in_dBm[index] = power_points_dBm[0];
            compression_points_out_dBm[index] = power_points_dBm[0];
        }
    }
}
void RunSweeps::FlipPorts() {
    for (int iPower = 0; iPower < power_points; iPower++) {
        for (int iFreq = 0; iFreq < freq_points; iFreq++) {
            s_parameter_data[iPower][iFreq][0][1] = s_parameter_data[iPower][iFreq][1][0];
        }
    }
}
