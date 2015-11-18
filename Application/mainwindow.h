#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "MeasurementSettings.h"
#include "MeasurementData.h"
#include "MeasureThread.h"

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
    explicit MainWindow(RsaToolbox::Vna &_vna, RsaToolbox::Keys &_keys, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    // Buttons
    void on_cancel_clicked();
    void on_measure_clicked();
    void on_exportData_clicked();

    void plotMaxGain(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &pin_dBm);
    void measurementFinished();

    void shake();

private:
    Ui::MainWindow *ui;
    RsaToolbox::Vna &_vna;
    RsaToolbox::Keys &_keys;

    MeasurementSettings _settings;
    QScopedPointer<MeasurementData> _results;
    QScopedPointer<MeasureThread> _thread;

    void loadKeys();
    void saveKeys();
    bool processSettings();

    bool _isMeasuring;
    QRect _settingsGeometry;
    QRect progressGeometry() const;
    void showProgressPage();
    void showSettingsPage();

    void setupPlot();
};


#endif // MAINWINDOW_H
