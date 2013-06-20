

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
                     QVector<uint> ports,
                     QRowVector &frequency_points_Hz,
                     QRowVector &power_points_dBm,
                     QMatrix2D &power_sweeps_dBm,
                     QVector<NetworkData> &s_parameter_data,
                     QObject *parent)
    : vna(vna),
      ports(ports),
      frequency_points_Hz(frequency_points_Hz),
      power_points_dBm(power_points_dBm),
      power_sweeps_dBm(power_sweeps_dBm),
      s_parameter_data(s_parameter_data),
      QThread(parent)
{
    // Not much else to do...
}

void RunSweeps::run() {
    // Measure each power level
    const int freq_points = frequency_points_Hz.size();
    const int power_points = power_points_dBm.size();
    s_parameter_data.resize(power_points);
    for (int iPower = 0; iPower < power_points; iPower++) {
        vna.Channel().SetChannelPower_dBm(power_points_dBm[iPower]);
        power_points_dBm[iPower] = vna.Channel().GetChannelPower_dBm();
        vna.Channel().MeasureNetwork(s_parameter_data[iPower], ports);
        emit Progress(((iPower+1.0)/power_points)*100);
    }

    // Vna requires ports in ascending order. I will reorder the
    //   returned data so that s21 is always the gain of the PA.
    if (ports[1] < ports[0])
        FlipPorts();

    // Reorder data by frequency point
    power_sweeps_dBm.resize(freq_points);
    for (int iFreq = 0; iFreq < freq_points; iFreq++) {
        power_sweeps_dBm[iFreq].resize(power_points);
        for (int iPower = 0; iPower < power_points; iPower++) {
            std::complex<double> s21_complex = s_parameter_data[iPower][iFreq][1][0];
            double s21_dB = ToDb(s21_complex);
            power_sweeps_dBm[iFreq][iPower] = power_points_dBm[iPower] + s21_dB;
        }
    }
    vna.moveToThread(QApplication::instance()->thread());
}

void RunSweeps::FlipPorts() {
    const int power_points = s_parameter_data.size();
    const int freq_points = frequency_points_Hz.size();
    for (int iPower = 0; iPower < power_points; iPower++) {
        for (int iFreq = 0; iFreq < freq_points; iFreq++) {
            s_parameter_data[iPower][iFreq][0][1] = s_parameter_data[iPower][iFreq][1][0];
        }
    }
}
