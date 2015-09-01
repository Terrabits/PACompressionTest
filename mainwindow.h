#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "RunSweeps.h"
#include "Settings.h"
#include "MeasurementData.h"
#include "TraceSettingsModel.h"

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

    // Run sweep
    bool isValidInput();
    void GetInput();
    void Progress(int percent);
    void Finished();

    // Export Data
    void Export();

    // Open, Save
    void Open();
    void Save();

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

    // Data
    MeasurementData _data;
    TraceSettingsModel _tracesModel;

};


#endif // MAINWINDOW_H

