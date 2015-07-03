#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "RunSweeps.h"
#include "Settings.h"

// Rsa
#include "Definitions.h"
#include "Keys.h"
#include "Vna.h"

// QCustomPlots
#include "qcustomplot.h"

// Qt
#include <QMainWindow>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QDataStream>

class RunSweeps;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(RsaToolbox::Keys *_keys, RsaToolbox::Log *log, QWidget *parent = 0);
    ~MainWindow();

    friend class RunSweeps;
    
private slots:

    // Initialize
    void ConnectMenuSignals();
    void LoadSettings();
    void SaveSettings();

    // Update GUI
    void UpdateStatus();
    void UpdateInstrumentInfo();
    void UpdateValidators();
    void UpdateFreqValidators();
    void UpdateIfBwValues();

    // Toggle inputs
    void ToggleConnect(bool enabled);
    void ToggleInputs(bool enabled);
    void TogglePlots(bool enabled);
    void ToggleSlider(bool enabled);

    // Run sweep
    bool isValidInput();
    void GetInput();
    void Progress(int percent);
    void Finished();

    // Calculate
    void CalculateReflectionMags();
    void CalculatePower();
    void FindNominalGain();

    // Plot
    void PlotReflection();
    void PlotPinVsPout();
    void PlotGainVsFreq();
    void PlotGainVsPin();
    void PlotPinCompression();
    void PlotPoutCompression();

    // Export Data
    void Export();
    void ExportTouchstone(QString path);
    void ExportPowerSweeps(QString path);
    void ExportPowerSweep(QString filename, int freq_index);
    void ExportGainSweeps(QString path);
    void ExportGainSweep(QString filename, int freq_index);
    void ExportNominalGain(QString filename);
    void ExportCompressionPoints(QString filename);

    // Open, Save
    void Open();
    bool Open(QDataStream &input);
    void Save();
    bool Save(QDataStream &output);

    void About();


    //////////// GUI CONTROLS ///////////////////

    // Connection controls
    void on_vna_connect_push_button_clicked();

    // Settings, Measure
    void on_start_freq_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_stop_freq_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_if_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_calibration_push_button_clicked();
    void on_retrieve_settings_push_button_clicked();
    void on_measure_push_button_clicked();

    // Plot, data
    void on_plot_type_combo_box_currentIndexChanged(const QString &arg1);
    void on_axis_push_button_clicked();
    void on_frequency_slider_valueChanged(int value);
    void on_print_plot_push_button_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<RunSweeps> run_sweeps;
    QScopedPointer<RsaToolbox::Vna> vna;
    RsaToolbox::Log *_log;
    RsaToolbox::Keys *_keys;

    // Current Vna properties:
    int ports;
    double max_freq_Hz, min_freq_Hz;
    int max_points;
    double min_power_dBm, max_power_dBm;
    QVector<QStringList> if_mantissa_values;
    QStringList if_units;
//    QStringList if_mantissa_Hz;
//    QStringList if_mantissa_KHz;
//    QStringList if_mantissa_MHz;
    QStringList receiver_attenuations;
    QStringList source_attenuations;

    // Vna Properties (last measurement)
    QString make;
    QString model;
    QString serial_no;
    QString firmware_version;

    // Paths
    QString open_path;
    QString save_path;
    QString export_path;
    QString print_path;

    // Settings (last measurement)
    QString time_stamp;
    int input_port;
    int output_port;
    double start_power_dBm;
    double stop_power_dBm;
    int power_points;
    double start_freq_Hz;
    double stop_freq_Hz;
    int frequency_points;
    double if_bw_Hz;
    double compression_level_dB;
    int source_attenuation;
    int receiver_attenuation;
    RsaToolbox::QRowVector frequencies_Hz;
    RsaToolbox::QRowVector power_in_dBm;

    // Measured, calculated data
    RsaToolbox::QRowVector s11_dB;
    RsaToolbox::QRowVector s22_dB;
    RsaToolbox::QMatrix2D gain_dB;
    RsaToolbox::QMatrix2D power_out_dBm;
    RsaToolbox::QRowVector nominal_gain_dB;
    RsaToolbox::QRowVector compression_points_in_dBm;
    RsaToolbox::QRowVector compression_points_out_dBm;
    RsaToolbox::QRowVector compression_frequencies_Hz;
    QVector<RsaToolbox::NetworkData> s_parameter_data;

    QCPPlotTitle *_plotTitle;
};


#endif // MAINWINDOW_H

