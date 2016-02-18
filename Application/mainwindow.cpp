

// Project
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Settings.h"
#include "FrequencySweep.h"
#include "SafeFrequencySweep.h"

#include "ProcessTrace.h"

// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QRect>
#include <QPropertyAnimation>


MainWindow::MainWindow(Vna &vna, Keys &keys, QWidget *parent) : 
    QMainWindow(parent),
    ui(new ::Ui::MainWindow),
    _vna(vna), _keys(keys),
    _isMeasuring(false),
    _guiState(GuiState::Configuration)
{
    ui->setupUi(this);
    QString title = "Compression Test " + APP_VERSION;
    setWindowTitle(title);

    qRegisterMetaType<RsaToolbox::QRowVector>("RsaToolbox::QRowVector");

    ui->settings->setVna(&vna);
    ui->settings->setKeys(&keys);

    ui->traces->setKeys(&keys);

    // Show settings initially
    ui->pages->setCurrentWidget(ui->configurePage);
    ui->configureTabs->setCurrentWidget(ui->settingsTab);

    // Settings
    connect(ui->settings, SIGNAL(exportClicked()),
            this, SLOT(exportData()));
    connect(ui->settings, SIGNAL(miniGuiClicked()),
            this, SLOT(miniGuiMode()));
    connect(ui->settings, SIGNAL(closeClicked()),
            this, SLOT(close()));
    connect(ui->settings, SIGNAL(measureClicked()),
            this, SLOT(startMeasurement()));


    // Traces
    connect(ui->traces, SIGNAL(exportClicked()),
            this, SLOT(exportData()));
    connect(ui->traces, SIGNAL(miniGuiClicked()),
            this, SLOT(miniGuiMode()));
    connect(ui->traces, SIGNAL(plotClicked()),
            this, SLOT(processTraces()));
    connect(ui->traces, SIGNAL(closeClicked()),
            this, SLOT(close()));

    // Progress
    connect(ui->progress, SIGNAL(cancelClicked()),
            this, SLOT(cancelMeasurement()));

    // Mini Gui
    connect(ui->mini, SIGNAL(standardGuiClicked()),
            this, SLOT(standardGuiMode()));
    connect(ui->mini, SIGNAL(closeClicked()),
            this, SLOT(close()));
    connect(ui->mini, SIGNAL(measureAndPlotClicked()),
            this, SLOT(startMeasurement()));

    _exportPath.setKey(&_keys, EXPORT_PATH_KEY);
    if (_exportPath.isEmpty())
        _exportPath.setPath(QDir::homePath());

    loadKeys();
}

MainWindow::~MainWindow() {
    _vna.isError();
    _vna.clearStatus();

    if (_guiState == GuiState::Mini) {
        _keys.set(MINI_GEOMETRY_KEY, geometry());
    }
    else if (!_miniGeometry.isNull()) {
        _keys.set(MINI_GEOMETRY_KEY, _miniGeometry);
    }

    if (_guiState == GuiState::Configuration) {
        _keys.set(STANDARD_GEOMETRY_KEY, geometry());
    }
    else if (!_configureGeometry.isNull()) {
        _keys.set(STANDARD_GEOMETRY_KEY, _configureGeometry);
    }

    delete ui;
}

bool MainWindow::askCancelMeasurement() {
    QMessageBox::StandardButton button
            = QMessageBox::question(this,
                                    "Abort Measurement?",
                                    "Measurements are running.\nAbort?");
    return button == QMessageBox::Yes;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!_isMeasuring) {
        QMainWindow::closeEvent(event);
        return;
    }

    // Measuring
    event->ignore();
    if (askCancelMeasurement()) {
        _thread->requestInterruption();
    }
}

// Private slots:
//void MainWindow::on_cancel_clicked() {
//    if (!_isMeasuring) {
//        close();
//    }
//    else {

//    }
//}
void MainWindow::startMeasurement() {
    if (!ui->settings->hasAcceptableInput()) {
        if (_guiState != GuiState::Configuration) {
            ui->configureTabs->setCurrentWidget(ui->settingsTab);
            showConfiguration();
        }
        else {
            shake();
        }
        return;
    }

    _settings = ui->settings->settings();
    ui->settings->saveKeys();

    _isMeasuring = true;
    clearResults();
    _results.reset();

    _thread.reset(createThread());
    _thread->setAppInfo(APP_NAME, APP_VERSION);
    _thread->setVna(&_vna);
    _thread->setSettings(_settings);

    if (_guiState == GuiState::Configuration) {
        // Maximized
        prepareProgressPage();
        showProgressPage();
    }
    else {
        prepareMiniGuiForMeasurement();
    }

    connect(_thread.data(), SIGNAL(finished()),
            this, SLOT(measurementFinished()));

    _thread->start();
}
void MainWindow::cancelMeasurement() {
    if (askCancelMeasurement())
        _thread->requestInterruption();
}
void MainWindow::measurementFinished() {
    _isMeasuring = false;

    if (_guiState == GuiState::Progress) {
        // Maximized
        disconnectProgressPage();
        showConfiguration();
    }
    else {
        // Minimized
        finishMiniGuiMeasurement();
        processTraces();
    }

    if (!_thread->isError()) {
        _results.reset(_thread->takeResults());
        showResults();
    }
    else {
        showMessage(_thread->errorMessage());
        shake();
    }

    _thread.reset();
    _vna.local();
}

void MainWindow::exportData() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export to...",
                                                    _exportPath.toString(),
                                                    "Zip (*.zip)");
    if (filename.isEmpty())
        return;

//    _results->save(QDir(SOURCE_DIR).filePath("measurementData.dat")); // Save data for testing purposes
    if (_results->exportToZip(filename)) {
        _exportPath.setFromFilePath(filename);
        showMessage("Export Sucessful!", Qt::darkGreen);
    }
    else {
        showMessage("*Error saving export file.");
    }
}
void MainWindow::processTraces() {
    if (!ui->traces->isTracesValid()) {
        if (_guiState != GuiState::Configuration) {
            ui->configureTabs->setCurrentWidget(ui->tracesTab);
            showConfiguration();
        }
        else {
            shake();
        }
        return;
    }

    // Apply traces
    QVector<TraceSettings> traces = ui->traces->traces();
    uint diagram = max(_vna.diagrams()) + 1;
    for (int i = 0; i < traces.size(); i++) {
        if (_vna.isDiagram(diagram) && _vna.diagram(diagram).traces().size() >= 20) {
            diagram++;
        }
        ProcessTrace(&(traces[i]), _results.data(), &_vna, diagram);
    }
    _vna.local();

    ui->traces->saveKeys();
}

void MainWindow::showMessage(const QString &message) {
    ui->settings->errorLabel()->showMessage(message);
    ui->traces->errorLabel()->showMessage(message);
    ui->mini->errorLabel()->showMessage(message);
}
void MainWindow::showMessage(const QString &message, Qt::GlobalColor color) {
    ui->settings->errorLabel()->showMessage(message, color);
    ui->traces->errorLabel()->showMessage(message, color);
    ui->mini->errorLabel()->showMessage(message, color);
}

void MainWindow::shake() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    QRect _geometry = geometry();
    _geometry.moveRight(_geometry.right() + 10);
    animation->setStartValue(_geometry);
    animation->setEndValue(geometry());
    animation->setDuration(500);
    QEasingCurve curve(QEasingCurve::OutElastic);
    curve.setAmplitude(2);
    curve.setPeriod(0.3);
    animation->setEasingCurve(curve);
    animation->start();
}

void MainWindow::miniGuiMode() {
    showMiniGui();
}
void MainWindow::standardGuiMode() {
    showConfiguration();
}
void MainWindow::restoreSizePolicy() {
    setMinimumSize(0, 0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

MeasureThread *MainWindow::createThread() {
    if (_settings.isFrequencySweep()) {
        if (_settings.isStopAtCompression())
            return new SafeFrequencySweep;
        else
            return new FrequencySweep;
    }

    // Default
    return new SafeFrequencySweep;
}

void MainWindow::loadKeys() {
    ui->settings->loadKeys();
    ui->traces->loadKeys();

    // MainWindow geometry
    if (_keys.exists(STANDARD_GEOMETRY_KEY)) {
        _keys.get(STANDARD_GEOMETRY_KEY, _configureGeometry);
        setGeometry(_configureGeometry);
    }
    if (_keys.exists(MINI_GEOMETRY_KEY)) {
        _keys.get(MINI_GEOMETRY_KEY, _miniGeometry);
    }
}

//QRect MainWindow::tracesGeometry() const {
//    const int i = QApplication::desktop()->primaryScreen();
//    QRect screen = QApplication::desktop()->screen(i)->geometry();

//    QPoint center = screen.center();

//    const int w = 600;
//    const int h = 450;

//    const int x = center.x() - w/2.0;
//    const int y = center.y() - h/2.0;
//    QRect rect = QRect(x, y, w, h);
//    return rect;
//}
void MainWindow::showConfiguration() {
    if (_guiState == GuiState::Mini) {
        _miniGeometry = geometry();
        _keys.set(MINI_GEOMETRY_KEY, _miniGeometry);
    }

    _guiState = GuiState::Configuration;
    ui->pages->setCurrentWidget(ui->configurePage);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(geometry());
    animation->setEndValue(_configureGeometry);
    connect(animation, SIGNAL(finished()),
            this, SLOT(restoreSizePolicy()));
    animation->start();
}

void MainWindow::prepareProgressPage() {
    ui->progress->startMeasurement(_settings.startFrequency_Hz(), _settings.stopFrequency_Hz(),
                                   _settings.startPower_dBm(), _settings.stopPower_dBm());
    connect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->progress, SLOT(startSweep(QString,uint)));
    connect(_thread.data(), SIGNAL(finishedSweep()),
            ui->progress, SLOT(stopSweep()));
    connect(_thread.data(), SIGNAL(progress(int)),
            ui->progress, SLOT(updateTotalProgress(int)));
    connect(_thread.data(), SIGNAL(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            ui->progress, SLOT(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    connect(_thread.data(), SIGNAL(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            ui->progress, SLOT(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
}
void MainWindow::showProgressPage() {
    _guiState = GuiState::Progress;
    ui->pages->setCurrentWidget(ui->progressPage);
}
void MainWindow::disconnectProgressPage() {
    disconnect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->progress, SLOT(startSweep(QString,uint)));
    disconnect(_thread.data(), SIGNAL(finishedSweep()),
            ui->progress, SLOT(stopSweep()));
    disconnect(_thread.data(), SIGNAL(progress(int)),
            ui->progress, SLOT(updateTotalProgress(int)));
    disconnect(_thread.data(), SIGNAL(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            ui->progress, SLOT(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    disconnect(_thread.data(), SIGNAL(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            ui->progress, SLOT(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
}

void MainWindow::showMiniGui() {
    _configureGeometry = geometry();
    _keys.set(STANDARD_GEOMETRY_KEY, _configureGeometry);

    const int newWidth = 300;
    const int newHeight = 150;

    if (_miniGeometry.isNull()) {
        QPoint center = geometry().center();
        _miniGeometry.setTop(center.y() - newHeight/2);
        _miniGeometry.setBottom(center.y() + newHeight/2);
        _miniGeometry.setLeft(center.x() - newWidth/2);
        _miniGeometry.setRight(center.x() + newWidth/2);
    }

    _guiState = GuiState::Mini;
    ui->pages->setCurrentWidget(ui->miniPage);

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMinimumSize(newWidth, newHeight);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(geometry());
    animation->setEndValue(_miniGeometry);
    animation->start();
}
void MainWindow::prepareMiniGuiForMeasurement() {
    ui->mini->startMeasurement();

    connect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->mini, SLOT(startSweep(QString,uint)));
    connect(_thread.data(), SIGNAL(finishedSweep()),
            ui->mini, SLOT(stopSweep()));
    connect(_thread.data(), SIGNAL(progress(int)),
            ui->mini, SLOT(updateTotalProgress(int)));
}
void MainWindow::finishMiniGuiMeasurement() {
    disconnect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->mini, SLOT(startSweep(QString,uint)));
    disconnect(_thread.data(), SIGNAL(finishedSweep()),
            ui->mini, SLOT(stopSweep()));
    disconnect(_thread.data(), SIGNAL(progress(int)),
            ui->mini, SLOT(updateTotalProgress(int)));

    ui->mini->finishMeasurement();
}

void MainWindow::showResults() {
    ui->settings->enableExport();

    ui->traces->setFrequencies(_results->frequencies_Hz());
    ui->traces->setPowers(_results->pin_dBm());
    ui->traces->enableExportAndPlot();

    ui->mini->enableExport();
}
void MainWindow::clearResults() {
    ui->settings->disableExport();
    ui->traces->disableExportAndPlot();
    ui->mini->disableExport();

    _results.reset();
}
