

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
    _miniPage(NULL),
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

    _exportPath.setKey(&_keys, EXPORT_PATH_KEY);
    if (_exportPath.isEmpty())
        _exportPath.setPath(QDir::homePath());

    loadKeys();
    _vna.local();
}
MainWindow::~MainWindow() {
    _vna.isError();
    _vna.clearStatus();

    if (_guiState == GuiState::Mini) {
        _keys.set(MINI_GEOMETRY_KEY, _miniPage->geometry());
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

void MainWindow::setMiniPage(MiniPage *page) {
    _miniPage = page;
    connect(_miniPage, SIGNAL(animationFinished()),
            this, SLOT(miniAnimatedMoveFinished()));
    connect(_miniPage, SIGNAL(exportClicked()),
            this, SLOT(exportData()));
    connect(_miniPage, SIGNAL(standardGuiClicked()),
            this, SLOT(standardGuiMode()));
    connect(_miniPage, SIGNAL(closeClicked()),
            this, SLOT(close()));
    connect(_miniPage, SIGNAL(measureAndPlotClicked()),
            this, SLOT(startMeasurement()));
}

bool MainWindow::askCancelMeasurement() {
    QWidget *parent;
    if (_guiState == GuiState::Mini)
        parent = _miniPage;
    else
        parent = this;
    QMessageBox::StandardButton button
            = QMessageBox::question(parent,
                                    "Abort Measurement?",
                                    "Measurements are running.\nAbort?");
    return button == QMessageBox::Yes;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!_isMeasuring) {
        _miniPage->close();
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
    _vna.local();

    if (_guiState == GuiState::Progress) {
        // Maximized
        disconnectProgressPage();
        showConfiguration();
    }
    else {
        finishMiniGuiMeasurement();
    }

    if (_thread->isError()) {
        showMessage(_thread->errorMessage());
        _thread.reset();
        if (_guiState == GuiState::Configuration) {
            ui->configureTabs->setCurrentWidget(ui->settingsTab);
            shake();
        }
        return;
    }

    _results.reset(_thread->takeResults());
    _thread.reset();
    if (_guiState == GuiState::Mini) {
        processTraces();
    }
    else {
        ui->configureTabs->setCurrentWidget(ui->tracesTab);
    }
    showResults();
}

void MainWindow::exportData() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export to...",
                                                    _exportPath.toString(),
                                                    "Zip (*.zip)");
    if (filename.isEmpty())
        return;
    if (_results->exportToZip(filename)) {
        _exportPath.setFromFilePath(filename);
        showMessage("Export Sucessful!", Qt::darkGreen);
    }
    else {
        showMessage("*Error exporting file.");
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
    if (_guiState == GuiState::Mini)
        _miniPage->showError(message);
}
void MainWindow::showMessage(const QString &message, Qt::GlobalColor color) {
    ui->settings->errorLabel()->showMessage(message, color);
    ui->traces->errorLabel()->showMessage(message, color);
    if (_guiState == GuiState::Mini)
        _miniPage->showError(message);
}

void MainWindow::shake() {
    QRect _geometry = geometry();
    _geometry.moveRight(_geometry.right() + 10);

    QEasingCurve curve(QEasingCurve::OutElastic);
    curve.setAmplitude(2);
    curve.setPeriod(0.3);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setStartValue(_geometry);
    animation->setEndValue(geometry());
    animation->setDuration(500);
    animation->setEasingCurve(curve);
    animation->start();
}

void MainWindow::miniGuiMode() {
    showMiniGui();
}
void MainWindow::standardGuiMode() {
    showConfiguration();
}
void MainWindow::miniAnimatedMoveFinished() {
    if (_guiState == GuiState::Mini)
        return;

    // Show main window
    _miniPage->hide();
    this->show();
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
    const GuiState previousState = _guiState;

    _guiState = GuiState::Configuration;
    ui->pages->setCurrentWidget(ui->configurePage);
    if (previousState != GuiState::Mini)
        return;

    // Transition from Mini
    _miniGeometry = _miniPage->geometry();
    _keys.set(MINI_GEOMETRY_KEY, _miniGeometry);

    QPoint center = geometry().center();

    const int newWidth = 54;
    const int newHeight = 231;

    // Calculate mini centered on MainWindow
    QRect centerMini;
    centerMini.setTop(center.y() - newHeight/2);
    centerMini.setBottom(center.y() + int(newHeight/2) + 1);
    centerMini.setLeft(center.x() - newWidth/2);
    centerMini.setRight(center.x() + newWidth/2);

    _miniPage->animateMove(centerMini.x(), centerMini.y());
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

    QPoint center = geometry().center();

    const int newWidth = 54;
    const int newHeight = 231;

    // Calculate mini centered on MainWindow
    QRect centerMini;
    centerMini.setTop(center.y() - newHeight/2);
    centerMini.setBottom(center.y() + int(newHeight/2) + 1);
    centerMini.setLeft(center.x() - newWidth/2);
    centerMini.setRight(center.x() + newWidth/2);

    // Mini destination
    if (_miniGeometry.isNull()) {
        _miniGeometry = centerMini;
    }

    _guiState = GuiState::Mini;
    this->hide();
    _miniPage->setGeometry(centerMini);
    _miniPage->show();
    if (centerMini != _miniGeometry) {
        _miniPage->animateMove(_miniGeometry.x(), _miniGeometry.y());
    }
}
void MainWindow::prepareMiniGuiForMeasurement() {
    _miniPage->startMeasurement();
    connect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            _miniPage, SLOT(startSweep(QString,uint)));
    connect(_thread.data(), SIGNAL(finishedSweep()),
            _miniPage, SLOT(stopSweep()));
    connect(_thread.data(), SIGNAL(progress(int)),
            _miniPage, SLOT(updateTotalProgress(int)));
}
void MainWindow::finishMiniGuiMeasurement() {
    disconnect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            _miniPage, SLOT(startSweep(QString,uint)));
    disconnect(_thread.data(), SIGNAL(finishedSweep()),
            _miniPage, SLOT(stopSweep()));
    disconnect(_thread.data(), SIGNAL(progress(int)),
            _miniPage, SLOT(updateTotalProgress(int)));
    _miniPage->finishMeasurement();
}

void MainWindow::showResults() {
    ui->settings->enableExport();

    ui->traces->setFrequencies(_results->frequencies_Hz());
    ui->traces->setPowers(_results->pin_dBm());
    ui->traces->enableExportAndPlot();

    _miniPage->enableExport();
}
void MainWindow::clearResults() {
    ui->settings->disableExport();
    ui->traces->disableExportAndPlot();
    _miniPage->disableExport();

    _results.reset();
}
