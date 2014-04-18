#include <QDebug>

#include "RunSweeps.h"
#include "ui_mainwindow.h"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>

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

    if (sweep_mode == "Frequency")
        RunFrequencySweeps();
    else
        RunPowerSweeps();

    vna->Channel().DeleteSParameterGroup();
    DisplayCompressionPoints();
    vna->Channel().EnableContinuousSweep();
    vna->Channel(2).EnableContinuousSweep();
    vna->moveToThread(QApplication::instance()->thread());
}

// Power Sweep
void RunSweeps::InitializePowerSweep() {
    vna->Channel().SetSweepType(POWER_SWEEP);
    vna->Trace().SetParameters(S_PARAMETER,
                               output_port,
                               input_port);
    vna->Trace().SetFormat(DB_MAGNITUDE_TRACE);
    vna->Channel().SetStartPower(start_power_dBm);
    vna->Channel().SetStopPower(stop_power_dBm);
    vna->Channel().SetPoints(power_points);
    vna->Channel().SetCompressionLevel(compression_level_dB);
    vna->Channel().EnableCompressionCalc();
    vna->Channel().GetStimulusValues(power_in_dBm);
    vna->EnableRfOutputPower();
    vna->Channel().DisableContinuousSweep();

    LinearSpacing(frequencies_Hz,
                  start_freq_Hz,
                  stop_freq_Hz,
                  frequency_points);
}
void RunSweeps::GetCompressionPoint(int index) {
    vna->ClearStatus();
    double in, out;
    vna->Channel().GetCompressionPoints(in, out);
    if(vna->isError() == false) {
        compression_points_in_dBm << in;
        compression_points_out_dBm << out;
        compression_frequencies_Hz << frequencies_Hz[index];
    }
}
void RunSweeps::ProcessPowerSweeps(QVector<NetworkData> &power_sweeps) {
    for (int i = 0; i < power_points; i++) {
        s_parameter_data[i].sweep_type = LINEAR_FREQUENCY_SWEEP;
        s_parameter_data[i].network_parameter = S_PARAMETER;
        s_parameter_data[i].ports = 2;
        s_parameter_data[i].points = frequency_points;
        s_parameter_data[i].format = REAL_IMAGINARY_COMPLEX;
        s_parameter_data[i].impedance = 50;
        s_parameter_data[i].isBalanced = false;
        s_parameter_data[i].date_time = QDateTime::currentDateTime();
        s_parameter_data[i].stimulus_prefix = NO_PREFIX;
        s_parameter_data[i].stimulus = frequencies_Hz.toStdVector();
        s_parameter_data[i].data.resize(frequency_points);
        for (int j = 0; j < frequency_points; j++) {
            s_parameter_data[i][j] = power_sweeps[j][i];
        }
    }
}
void RunSweeps::RunPowerSweeps() {
    InitializePowerSweep();
    vna->DisableErrorDisplay();

    QVector<NetworkData> power_sweeps;
    power_sweeps.resize(frequency_points);
    emit Progress(0);
    for (int i = 0; i < frequency_points; i++) {
        TraceData trace;
        vna->Channel().SetCwFrequency(frequencies_Hz[i]);
        frequencies_Hz[i] = vna->Channel().GetCwFrequency_Hz();
        vna->Channel().MeasureNetwork(power_sweeps[i], ports);
        vna->AutoscaleDiagrams();
        vna->Trace().MeasureTrace(trace);
        gain_dB[i] = QRowVector::fromStdVector(trace.data);
//        GetCompressionPoint(i);
        emit Progress((i+1.0)/frequency_points*100);
    }
    vna->ClearStatus();
    vna->EnableErrorDisplay();

    if (output_port < input_port)
        FlipPorts(power_sweeps);

    ProcessPowerSweeps(power_sweeps);
    FindCompressionPoints();
}

// Freq Sweep
void RunSweeps::InitializeFrequencySweep() {
    vna->Channel().SetSweepType(LINEAR_FREQUENCY_SWEEP);
    vna->Trace().SetParameters(S_PARAMETER,
                               output_port,
                               input_port);
    vna->Trace().SetFormat(DB_MAGNITUDE_TRACE);
    vna->Channel().SetStartFrequency(start_freq_Hz);
    vna->Channel().SetStopFrequency(stop_freq_Hz);
    vna->Channel().SetPoints(frequency_points);
    vna->Channel().GetStimulusValues(frequencies_Hz);
    vna->Channel().DisableContinuousSweep();

    LinearSpacing(power_in_dBm,
                  start_power_dBm,
                  stop_power_dBm,
                  power_points);
}
void RunSweeps::ProcessFrequencySweep() {
    for (int i = 0; i < frequency_points; i++) {
        gain_dB[i].resize(power_points);
        for (int j = 0; j < power_points; j++) {
            gain_dB[i][j] = ToDb(s_parameter_data[j][i][1][0]);
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
            compression_points_in_dBm <<
                                         LinearInterpolateX(power_in_dBm[power_index-1],
                    gain_dB[i][power_index-1],
                    power_in_dBm[power_index],
                    gain_dB[i][power_index],
                    nominal_gain_dB_i - compression_level_dB);
            compression_points_out_dBm << LinearInterpolateY(power_in_dBm[power_index-1],
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
        vna->Channel().SetChannelPower_dBm(power_in_dBm[i]);
        power_in_dBm[i] = vna->Channel().GetChannelPower_dBm();
        vna->Channel().MeasureNetwork(s_parameter_data[i], ports);
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
        vna->EnableDisplay();
        return;
    }

    // Return to sweep mode
    if (sweep_mode == "Power") {
        vna->Channel().DisableCompressionCalc();
        vna->Channel().SetSweepType(LINEAR_FREQUENCY_SWEEP);
        vna->Channel().SetStartFrequency(start_freq_Hz);
        vna->Channel().SetStopFrequency(stop_freq_Hz);
        vna->Channel().SetPoints(frequency_points);
    }

    // Create new channel, diagram
    uint new_diagram = vna->GetDiagrams().last() + 1;
    uint new_channel = vna->GetChannels().last() + 1;
    vna->CreateDiagram(new_diagram);
    vna->CreateChannel(new_channel);

    // Set segmented sweep
    vna->Channel(new_channel).SetCustomFrequencySweep(compression_frequencies_Hz);

    // Create traces
    vna->CreateTrace("Trc2", new_channel, S_PARAMETER, 2, 1);
    vna->Trace("Trc2").SetDiagram(new_diagram);
    vna->Diagram(new_diagram).SetTitle("Compression Point");
    vna->Trace("Trc2").CopyToMemory("Pin");
    vna->Trace("Pin").SetDiagram(new_diagram);
    vna->Trace("Pin").WriteData(ToMagnitude(compression_points_in_dBm));
    vna->Trace("Trc2").CopyToMemory("Pout");
    vna->Trace("Pout").SetDiagram(new_diagram);
    vna->Trace("Pout").WriteData(ToMagnitude(compression_points_out_dBm));
    vna->Channel(2).InitiateSweep();
    vna->FinishPreviousCommandsFirst();
    vna->EnableDisplay();
    vna->Diagram(2).Autoscale();
    vna->Trace("Trc2").Hide();
}
void RunSweeps::FlipPorts(QVector<NetworkData> &sweeps) {
    int iMax = sweeps.size();
    int jMax = sweeps[0].data.size();
    for (int i = 0; i < iMax; i++) {
        for (int j = 0; j < jMax; j++) {
            ComplexMatrix2D current_sweep = sweeps[i][j];
            sweeps[i][j][0][0] = current_sweep[1][1];
            sweeps[i][j][1][1] = current_sweep[0][0];
            sweeps[i][j][0][1] = current_sweep[1][0];
            sweeps[i][j][1][0] = current_sweep[0][1];
        }
    }
}
