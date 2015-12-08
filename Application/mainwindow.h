#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "MeasurementSettings.h"
#include "MeasurementData.h"
#include "MeasureThread.h"
#include "TraceSettingsModel.h"

// RsaToolbox
#include <Keys.h>
#include <Vna.h>
#include <LastPath.h>

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

protected:
    virtual void closeEvent(QCloseEvent *event);
    
private slots:
    // Buttons
    void on_measure_clicked();
    void on_exportData_clicked();

    void updatePowerSpacing();

    void plotMaxGain(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &pin_dBm);
    void measurementFinished();

    void shake();

private:
    Ui::MainWindow *ui;
    RsaToolbox::Vna &_vna;

    MeasurementSettings _settings;
    QScopedPointer<MeasurementData> _results;
    QScopedPointer<MeasureThread> _thread;
    MeasureThread *createThread();

    RsaToolbox::Keys &_keys;
    RsaToolbox::LastPath _exportPath;
    void loadKeys();
    void saveKeys();
    bool processSettings();

    bool _isMeasuring;
    QRect _settingsGeometry;
    QRect progressGeometry() const;
    QRect tracesGeometry() const;
    void showProgressPage();
    void showSettingsPage();
    void showTracesPage();

    void setupPlot();

    TraceSettingsModel _traceSettingsModel;
};


#endif // MAINWINDOW_H
