#include <QDebug>

#include "RunSweeps.h"
#include "ui_mainwindow.h"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>
#include <NetworkTraceData.h>

// C++ std lib
#include <complex>

// Qt
#include <QApplication>
#include <QMessageBox>

using namespace RsaToolbox;


RunSweeps::RunSweeps(MainWindow *main_window, QObject *parent)
    : QThread(parent),
      frequencies_Hz(main_window->frequencies_Hz),
      power_in_dBm(main_window->power_in_dBm),
      compression_points_in_dBm(main_window->compression_points_in_dBm),
      compression_points_out_dBm(main_window->compression_points_out_dBm),
      compression_frequencies_Hz(main_window->compression_frequencies_Hz),
      gain_dB(main_window->gain_dB),
      s_parameter_data(main_window->s_parameter_data)
{
    vna = main_window->vna.data();
    sweep_mode = main_window->ui->sweep_mode_combo_box->currentText();
    input_port = main_window->input_port;
    output_port = main_window->output_port;
    start_power_dBm = main_window->start_power_dBm;
    stop_power_dBm = main_window->stop_power_dBm;
    power_points = main_window->power_points;
    start_freq_Hz = main_window->start_freq_Hz;
    stop_freq_Hz = main_window->stop_freq_Hz;
    frequency_points = main_window->frequency_points;
    compression_level_dB = main_window->compression_level_dB;
}

void RunSweeps::run() {
    compression_points_in_dBm.clear();
    compression_points_out_dBm.clear();
    compression_frequencies_Hz.clear();
    frequencies_Hz.resize(frequency_points);
    power_in_dBm.resize(power_points);
    gain_dB.resize(frequency_points);
    s_parameter_data.resize(power_points);
    ports.clear();
    ports << input_port
          << output_port;

//    if (sweep_mode == "Frequency")
    RunFrequencySweeps();
//    else
//        RunPowerSweeps();

    vna->channel().linearSweep().clearSParameterGroup();
    DisplayCompressionPoints();
    vna->channel().continuousSweepOn();
    vna->channel(2).continuousSweepOn();
    vna->moveToThread(QApplication::instance()->thread());
}

// Power Sweep
void RunSweeps::InitializePowerSweep() {
    vna->channel().setSweepType(VnaChannel::SweepType::Power);
    vna->trace().setNetworkParameter(NetworkParameter::S,
                                     output_port,
                                     input_port);
    vna->trace().setFormat(TraceFormat::DecibelMagnitude);
    vna->channel().powerSweep().setStart(start_power_dBm);
    vna->channel().powerSweep().setStop(stop_power_dBm);
    vna->channel().powerSweep().setPoints(power_points);

    // Fix this!
//    vna->channel().SetCompressionLevel(compression_level_dB);
//    vna->channel().EnableCompressionCalc();

    power_in_dBm = vna->channel().powerSweep().powers_dBm();
    vna->settings().rfOutputPowerOn();
    vna->channel().manualSweepOn();

    frequencies_Hz = linearSpacing(start_freq_Hz, stop_freq_Hz, frequency_points);
}
void RunSweeps::GetCompressionPoint(int index) {
    vna->clearStatus();
    double in = 0.0,
           out = 0.0;

    // Fix this
//    vna->channel().GetCompressionPoints(in, out);

    if (!vna->isError()) {
        compression_points_in_dBm << in;
        compression_points_out_dBm << out;
        compression_frequencies_Hz << frequencies_Hz[index];
    }
}
void RunSweeps::ProcessPowerSweeps(QVector<NetworkData> &power_sweeps) {
    for (int i = 0; i < power_points; i++) {
        s_parameter_data[i].setParameter(NetworkParameter::S);
        s_parameter_data[i].setReferenceImpedance(50);
        s_parameter_data[i].setTimestamp();
        s_parameter_data[i].setXUnits(Units::Hertz);
        s_parameter_data[i].resize(2, frequency_points);
        s_parameter_data[i].x() = frequencies_Hz;
    }
}
void RunSweeps::RunPowerSweeps() {
    InitializePowerSweep();
    vna->settings().errorDisplayOff();

    QVector<NetworkData> power_sweeps;
    power_sweeps.resize(frequency_points);
    emit Progress(0);
    for (int i = 0; i < frequency_points; i++) {
        NetworkTraceData trace;
        vna->channel().powerSweep().setFrequency(frequencies_Hz[i]);
        frequencies_Hz[i] = vna->channel().powerSweep().frequency_Hz();

        // Fix???
        power_sweeps[i] = vna->channel().linearSweep().measure(ports);
//        vna->Channel().MeasureNetwork(power_sweeps[i], ports);

//        vna->autoscaleDiagrams();
        vna->trace().measure(trace);
        gain_dB[i] = trace.y_dB();
//        GetCompressionPoint(i); // This was like this already...
        emit Progress((i+1.0)/frequency_points*100);
    }
    vna->clearStatus();
    vna->settings().errorDisplayOn();

    if (output_port < input_port)
        FlipPorts(power_sweeps);

    ProcessPowerSweeps(power_sweeps);
    FindCompressionPoints();
}

// Freq Sweep
void RunSweeps::InitializeFrequencySweep() {
    vna->channel().setSweepType(VnaChannel::SweepType::Linear);
    vna->trace().setNetworkParameter(NetworkParameter::S, output_port, input_port);
    vna->trace().setFormat(TraceFormat::DecibelMagnitude);
    vna->channel().linearSweep().setStart(start_freq_Hz);
    vna->channel().linearSweep().setStop(stop_freq_Hz);
    vna->channel().linearSweep().setPoints(frequency_points);
    frequencies_Hz = vna->channel().linearSweep().frequencies_Hz();
    vna->channel().manualSweepOn();

    power_in_dBm = linearSpacing(start_power_dBm, stop_power_dBm, power_points);
}
void RunSweeps::ProcessFrequencySweep() {
    for (int i = 0; i < frequency_points; i++) {
        gain_dB[i].resize(power_points);
        for (int j = 0; j < power_points; j++) {
            gain_dB[i][j] = toDb(s_parameter_data[j].y()[i][1][0]);
        }
    }
}
void RunSweeps::FindCompressionPoints() {
    compression_points_in_dBm.clear();
    compression_points_out_dBm.clear();
    compression_frequencies_Hz.clear();
    for (int i = 0; i < frequency_points; i++) {
        int power_index = 0;
        double nominal_gain_dB_i = gain_dB[i][0];
        while (power_index < power_points-1 && gain_dB[i][power_index] > nominal_gain_dB_i - compression_level_dB) {
            power_index++;
        }
        if (gain_dB[i][power_index] == nominal_gain_dB_i - compression_level_dB) {
            // Exact match
            compression_points_in_dBm << power_in_dBm[power_index];
            compression_points_out_dBm << power_in_dBm[power_index] + gain_dB[i][power_index];
            compression_frequencies_Hz << frequencies_Hz[i];
        }
        else if (gain_dB[i][power_index] < nominal_gain_dB_i - compression_level_dB){
            // compression point found inbetween data points
            compression_points_in_dBm << linearInterpolateX(power_in_dBm[power_index-1],
                                                            gain_dB[i][power_index-1],
                                                            power_in_dBm[power_index],
                                                            gain_dB[i][power_index],
                                                            nominal_gain_dB_i - compression_level_dB);
            compression_points_out_dBm << linearInterpolateY(power_in_dBm[power_index-1],
                    power_in_dBm[power_index-1] + gain_dB[i][power_index-1], //Pout[index-1]
                    power_in_dBm[power_index],
                    power_in_dBm[power_index] + gain_dB[i][power_index], //Pout[index]
                    compression_points_in_dBm.last());
            compression_frequencies_Hz << frequencies_Hz[i];
        }
        // else no match
    }
}
void RunSweeps::RunFrequencySweeps() {
    InitializeFrequencySweep();

    emit Progress(0);
    for (int i = 0; i < power_points; i++) {
        vna->channel().linearSweep().setPower(power_in_dBm[i]);
        power_in_dBm[i] = vna->channel().linearSweep().power_dBm();
        s_parameter_data[i] = vna->channel().linearSweep().measure(ports);
        emit Progress((i+1.0)/power_points*100);
    }

    if (output_port < input_port)
        FlipPorts(s_parameter_data);

    ProcessFrequencySweep();
    FindCompressionPoints();
}

// Misc.
void RunSweeps::DisplayCompressionPoints() {
    if (compression_frequencies_Hz.size() == 0) {
        vna->settings().displayOn();
        return;
    }

    // Return to sweep mode
    if (sweep_mode == "Power") {
        // FIX
//        vna->Channel().DisableCompressionCalc();
        vna->channel().setSweepType(VnaChannel::SweepType::Linear);
        vna->channel().linearSweep().setStart(start_freq_Hz);
        vna->channel().linearSweep().setStop(stop_freq_Hz);
        vna->channel().linearSweep().setPoints(frequency_points);
    }

    // Create new channel, diagram
    uint new_diagram = vna->createDiagram();
    uint new_channel = vna->createChannel();

    // Set segmented sweep
    vna->channel(new_channel).setFrequencies(compression_frequencies_Hz);

    // Y-Axis min/max
    double min = std::min(RsaToolbox::min(compression_points_in_dBm), RsaToolbox::min(compression_points_out_dBm));
    double max = std::max(RsaToolbox::max(compression_points_in_dBm), RsaToolbox::max(compression_points_out_dBm));
    roundAxis(min, max, 5, min, max);

    // Create traces
    vna->createTrace("Trc2", new_channel);
    vna->trace("Trc2").setNetworkParameter(NetworkParameter::S, 2, 1);
    vna->trace("Trc2").setDiagram(new_diagram);
    vna->trace("Trc2").setYAxis(min, max);
    vna->diagram(new_diagram).setTitle("Compression Point");
    vna->trace("Trc2").toMemory("Pin");
    vna->trace("Pin").setDiagram(new_diagram);
    vna->trace("Pin").write(compression_points_in_dBm);
    vna->trace("Pin").setYAxis(min, max);
    vna->trace("Trc2").toMemory("Pout");
    vna->trace("Pout").setDiagram(new_diagram);
    vna->trace("Pout").write(compression_points_out_dBm);
    vna->trace("Pout").setYAxis(min, max);
    vna->channel(2).startSweep();
    vna->wait();
    vna->settings().displayOn();
    vna->channel(2).manualSweepOn();
    vna->trace("Trc2").hide();
}
void RunSweeps::FlipPorts(QVector<NetworkData> &sweeps) {
    int iMax = sweeps.size();
    int jMax = int(sweeps[0].points());
    for (int i = 0; i < iMax; i++) {
        for (int j = 0; j < jMax; j++) {
            ComplexMatrix2D current_sweep = sweeps[i].y()[j];
            sweeps[i].y()[j][0][0] = current_sweep[1][1];
            sweeps[i].y()[j][1][1] = current_sweep[0][0];
            sweeps[i].y()[j][0][1] = current_sweep[1][0];
            sweeps[i].y()[j][1][0] = current_sweep[0][1];
        }
    }
}
