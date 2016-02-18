#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "MeasurementSettings.h"
#include "MeasurementData.h"
#include "MeasureThread.h"

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

    enum /*class*/ GuiState {
        Configuration,
        Progress,
        Mini
    };

protected:
    bool askCancelMeasurement();
    virtual void closeEvent(QCloseEvent *event);
    
private slots:
    void startMeasurement();
    void cancelMeasurement();
    void measurementFinished();

    void exportData();
    void processTraces();

    void showMessage(const QString &message);
    void showMessage(const QString &message, Qt::GlobalColor color);
    void shake();

    void miniGuiMode();
    void standardGuiMode();
    void restoreSizePolicy();

private:
    Ui::MainWindow *ui;
    RsaToolbox::Vna &_vna;

    MeasurementSettings _settings;

    QScopedPointer<MeasureThread> _thread;
    MeasureThread *createThread();

    QScopedPointer<MeasurementData> _results;
    void showResults();
    void clearResults();

    // Keys
    RsaToolbox::Keys &_keys;
    RsaToolbox::LastPath _exportPath;
    void loadKeys();

    // Gui
    bool _isMeasuring;
    GuiState _guiState;

    QRect _configureGeometry;
    QRect _miniGeometry;

    void showConfiguration();

    void prepareProgressPage();
    void showProgressPage();
    void disconnectProgressPage();

    void showMiniGui();
    void prepareMiniGuiForMeasurement();
    void finishMiniGuiMeasurement();
};


#endif // MAINWINDOW_H
