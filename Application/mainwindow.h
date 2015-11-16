#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "MeasurementSettings.h"
#include "MeasurementData.h"

// RsaToolbox
#include "Keys.h"
#include "Vna.h"

// Qt
#include <QMainWindow>
#include <QScopedPointer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(RsaToolbox::Vna &vna, RsaToolbox::Keys &keys, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    // Buttons
    void on_cancel_clicked();
    void on_measure_clicked();
    void on_exportData_clicked();

    void plotMaxGain(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &pin_dBm);

    void shake();

private:
    Ui::MainWindow *ui;
    RsaToolbox::Vna &vna;
    RsaToolbox::Keys &keys;

    MeasurementSettings _settings;
    QScopedPointer<MeasurementData> _results;

    bool _isMeasuring;
    QRect _settingsGeometry;
    QRect progressGeometry() const;
    void showProgressPage();
    void showSettingsPage();

    void setupPlot();
};


#endif // MAINWINDOW_H
