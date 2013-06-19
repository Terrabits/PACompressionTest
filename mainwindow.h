#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "RunSweeps.h"

// Rsa
#include "Definitions.h"
#include "General.h"
#include "Key.h"
#include "Vna.h"

// QCustomPlots
#include "qcustomplot.h"

// Qt
#include <QMainWindow>
#include <QScopedPointer>
#include <QString>
#include <QStringList>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(RsaToolbox::Key &key, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void UpdateStatus();
    void UpdateInstrumentInfo();
    void UpdateValidators();
    void UpdateFreqValidators();
    void UpdateIfBwValues();
    void ToggleInputs(bool enabled);
    bool isValidInput();
    void Progress(int percent);
    void Finished();
    void TogglePlots(bool enabled);
    void ToggleConnect(bool enabled);
    void CalculateGain();
    void FindNominalGain();
    void FindCompressionPoints();
    double LinearInterpolate(double x1, double y1, double x2, double y2, double y_desired);
    void PlotPinVsPout();
    void PlotGainVsFreq();
    void PlotGainVsPin();
    void CompressionPointVsFreq();

    void on_vna_connect_push_button_clicked();
    void on_start_freq_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_stop_freq_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_if_units_combo_box_currentIndexChanged(const QString &arg1);
    void on_measure_push_button_clicked();

    void on_plot_type_combo_box_currentIndexChanged(const QString &arg1);

    void on_frequency_slider_valueChanged(int value);

    void on_print_plot_push_button_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<RunSweeps> run_sweeps;
    QScopedPointer<RsaToolbox::Vna> vna;
    RsaToolbox::Key *key;

    // Vna properties:
    int ports;
    double max_freq_Hz, min_freq_Hz;
    int max_points;
    double min_power_dBm, max_power_dBm;
    QStringList if_mantissa_Hz;
    QStringList if_mantissa_KHz;
    QStringList if_mantissa_MHz;
    QStringList receiver_attenuations;
    QStringList source_attenuations;

    // Measured data
    double compression_point_dB;
    int number_frequency_points;
    int number_power_points;
    RsaToolbox::QRowVector frequency_points_Hz;
    RsaToolbox::QRowVector power_points_dBm;
    RsaToolbox::QMatrix2D sweep_data_dBm;
    RsaToolbox::QMatrix2D gain_data_dB;
    RsaToolbox::QRowVector nominal_gain_dB;
    RsaToolbox::QRowVector compression_points_dBm;
};


#endif // MAINWINDOW_H

