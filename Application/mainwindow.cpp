

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
    _isMeasuring(false)
{
    ui->setupUi(this);
    QString title = "Compression Test " + APP_VERSION;
    setWindowTitle(title);

    // Show settings initially
    ui->pages->setCurrentWidget(ui->settingsPage);

    // Frequency
    const double minimum_Hz = vna.properties().minimumFrequency_Hz();
    const double maximum_Hz = vna.properties().maximumFrequency_Hz();

    ui->startFrequency->setParameterName("Start frequency");
    ui->startFrequency->setMinimum(minimum_Hz);
    ui->startFrequency->setMaximum(maximum_Hz);

    ui->stopFrequency->setParameterName("Stop frequency");
    ui->stopFrequency->setMinimum(minimum_Hz);
    ui->stopFrequency->setMaximum(maximum_Hz);

    ui->frequencyPoints->setParameterName("Frequency points");
    ui->frequencyPoints->setMinimum(2);
    ui->frequencyPoints->setMaximum(vna.properties().maximumPoints());

    ui->ifBw->setParameterName("IF BW");
    ui->ifBw->setAcceptedValues(vna.properties().ifBandwidthValues_Hz());
    
    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));    
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));

    // Power
    const double minimum_dBm = vna.properties().minimumPower_dBm();
    const double maximum_dBm = vna.properties().maximumPower_dBm();

    ui->startPower->setParameterName("Start power");
    ui->startPower->setMinimum(minimum_dBm);
    ui->startPower->setMaximum(maximum_dBm);

    ui->stopPower->setParameterName("Stop power");
    ui->stopPower->setMinimum(minimum_dBm);
    ui->stopPower->setMaximum(maximum_dBm);

    ui->powerPoints->setParameterName("Power points");
    ui->powerPoints->setMinimum(2);
    ui->powerPoints->setMaximum(vna.properties().maximumPoints());

    ui->compressionLevel->setParameterName("Compression level");
    ui->compressionLevel->setMinimum(0.01);
//    ui->compressionLevel->setMaximum(6.0); // ?

    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->compressionLevel, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->compressionLevel, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));

    connect(ui->startPower, SIGNAL(powerChanged(double)),
            this, SLOT(updatePowerSpacing()));
    connect(ui->stopPower, SIGNAL(powerChanged(double)),
            this, SLOT(updatePowerSpacing()));
    connect(ui->powerPoints, SIGNAL(pointsChanged(double)),
            this, SLOT(updatePowerSpacing()));

    // Miscellaneous
    QStringList channels = toStringList(vna.channels());
    ui->channel->clear();
    ui->channel->addItems(channels);

    const uint testPorts = vna.testPorts();
    ui->inputPort->setParameterName("Input port");
    ui->inputPort->setMinimum(1);
    ui->inputPort->setMaximum(testPorts);

    ui->outputPort->setParameterName("Output port");
    ui->outputPort->setMinimum(1);
    ui->outputPort->setMaximum(testPorts);

    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));

    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            this, SLOT(shake()));

    connect(ui->cancel, SIGNAL(clicked()),
            this, SLOT(close()));

    ui->timedProgressBar->showPercentage();

    qRegisterMetaType<RsaToolbox::QRowVector>("RsaToolbox::QRowVector");

    _exportPath.setKey(&_keys, EXPORT_PATH_KEY);
    if (_exportPath.isEmpty())
        _exportPath.setPath(QDir::homePath());

    connect(ui->tracesWidget, SIGNAL(error(QString)),
            ui->traceError, SLOT(showMessage(QString)));

    loadKeys();
}

MainWindow::~MainWindow() {
    _vna.isError();
    _vna.clearStatus();

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (_isMeasuring) {
        event->ignore();

        QMessageBox::StandardButton button = QMessageBox::question(this,
                              "Abort Measurement?",
                              "Measurements are running.\nAbort?");
        if (button == QMessageBox::Yes) {
            if (!_thread.isNull())
                _thread->requestInterruption();
        }
    }
    else {
        QMainWindow::closeEvent(event);
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
void MainWindow::on_measure_clicked() {
    if (ui->pages->currentWidget() == ui->tracesPage)
    {
        if (!ui->tracesWidget->isTracesValid())
            return;

        // Apply traces
        QVector<TraceSettings> traces = ui->tracesWidget->traces();
        uint diagram = max(_vna.diagrams()) + 1;
        for (int i = 0; i < traces.size(); i++) {
            ProcessTrace(&(traces[i]), _results.data(), &_vna, diagram);
        }
        _vna.local();

//        showSettingsPage();
        saveKeys();
        return;
    }

    if (!processSettings())
        return;

    saveKeys();

    _isMeasuring = true;

    _results.reset();

    _thread.reset(createThread());
    _thread->setAppInfo(APP_NAME, APP_VERSION);
    _thread->setVna(&_vna);
    _thread->setSettings(_settings);

    connect(_thread.data(), SIGNAL(progress(int)),
            ui->progressBar, SLOT(setValue(int)));
    connect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->timedProgressBar, SLOT(start(QString,uint)));
    connect(_thread.data(), SIGNAL(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            this, SLOT(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    connect(_thread.data(), SIGNAL(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            this, SLOT(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    connect(_thread.data(), SIGNAL(finishedSweep()),
            ui->timedProgressBar, SLOT(stop()));
    connect(_thread.data(), SIGNAL(finished()),
            this, SLOT(measurementFinished()));

    setupPlot();
    showProgressPage();
    _thread->start();
}
void MainWindow::on_exportData_clicked() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export to...",
                                                    _exportPath.toString(),
                                                    "Zip (*.zip)");
    if (filename.isEmpty())
        return;

    if (_results->exportToZip(filename)) {
        _exportPath.setFromFilePath(filename);
        ui->error->showMessage("Export successful!", Qt::darkGreen);
    }
    else {
        ui->error->showMessage("*Error saving export file.");
    }
}

void MainWindow::updatePowerSpacing() {
    ui->powerSpacing->clear();

    if (!ui->startPower->hasAcceptableInput()) {
        return;
    }
    if (!ui->stopPower->hasAcceptableInput()) {
        return;
    }
    if (!ui->powerPoints->hasAcceptableInput()) {
        return;
    }
    if (ui->startPower->power_dBm() > ui->stopPower->power_dBm()) {
        return;
    }

    const double start = ui->startPower->power_dBm();
    const double stop = ui->stopPower->power_dBm();
    const uint points = ui->powerPoints->points();

    double spacing = (stop - start) / (points - 1);
    QString text = "%1 dBm";
    text = text.arg(formatDouble(spacing, 3));
    ui->powerSpacing->setText(text);
}

MeasureThread *MainWindow::createThread() {
    const QString sweepType = ui->sweepType->currentText();
    const bool isStopAtCompression = ui->stopAtCompression->isChecked();

    if (sweepType == "Frequency") {
        if (isStopAtCompression)
            return new SafeFrequencySweep;
        else
            return new FrequencySweep;
    }

    // Default
    return new SafeFrequencySweep;
}

void MainWindow::loadKeys() {
    bool _bool;
    quint32 _quint32;
    double _double;
    QString _string;

    // Frequency
    if (_keys.exists(START_FREQUENCY_KEY)) {
        _keys.get(START_FREQUENCY_KEY, _double);
        ui->startFrequency->setFrequency(_double);
    }
    if (_keys.exists(STOP_FREQUENCY_KEY)) {
        _keys.get(STOP_FREQUENCY_KEY, _double);
        ui->stopFrequency->setFrequency(_double);
    }
    if (_keys.exists(FREQUENCY_POINTS_KEY)) {
        _keys.get(FREQUENCY_POINTS_KEY, _quint32);
        ui->frequencyPoints->setPoints(_quint32);
    }
    if (_keys.exists(IF_BW_KEY)) {
        _keys.get(IF_BW_KEY, _double);
        ui->ifBw->setFrequency(_double);
    }

    // Power
    if (_keys.exists(START_POWER_KEY)) {
        _keys.get(START_POWER_KEY, _double);
        ui->startPower->setPower(_double);
    }
    if (_keys.exists(STOP_POWER_KEY)) {
        _keys.get(STOP_POWER_KEY, _double);
        ui->stopPower->setPower(_double);
    }
    if (_keys.exists(POWER_POINTS_KEY)) {
        _keys.get(POWER_POINTS_KEY, _quint32);
        ui->powerPoints->setPoints(_quint32);
    }
    if (_keys.exists(COMPRESSION_LEVEL_KEY)) {
        _keys.get(COMPRESSION_LEVEL_KEY, _double);
        ui->compressionLevel->setValue(_double);
    }
    if (_keys.exists(IS_GAIN_EXPANSION_KEY)) {
        _keys.get(IS_GAIN_EXPANSION_KEY, _bool);
        ui->gainExpansion->setChecked(_bool);
    }
    if (_keys.exists(IS_STOP_AT_COMPRESSION_KEY)) {
        _keys.get(IS_STOP_AT_COMPRESSION_KEY, _bool);
        ui->stopAtCompression->setChecked(_bool);
    }
    if (_keys.exists(POST_CONDITION_KEY)) {
        _keys.get(POST_CONDITION_KEY, _string);
        ui->postCondition->setCurrentText(_string);
    }

    // Miscellaneous
    if (_keys.exists(CHANNEL_KEY)) {
        _keys.get(CHANNEL_KEY, _string);
        if (ui->channel->findText(_string) != -1)
            ui->channel->setCurrentText(_string);
    }
    if (_keys.exists(OUTPUT_PORT_KEY)) {
        _keys.get(OUTPUT_PORT_KEY, _quint32);
        ui->outputPort->setPoints(_quint32);
    }
    if (_keys.exists(INPUT_PORT_KEY)) {
        _keys.get(INPUT_PORT_KEY, _quint32);
        ui->inputPort->setPoints(_quint32);
    }
    if (_keys.exists(SWEEP_TYPE_KEY)) {
        _keys.get(SWEEP_TYPE_KEY, _string);
        if (ui->sweepType->findText(_string) != -1)
            ui->sweepType->setCurrentText(_string);
    }

    // Traces
    if (_keys.exists(TRACES_KEY)) {
        QVector<TraceSettings> traces;
        _keys.get(TRACES_KEY, traces);
        ui->tracesWidget->setTraces(traces);
    }
}
void MainWindow::saveKeys() {
    // Assumes valid input

    // Frequency
    _keys.set(START_FREQUENCY_KEY, ui->startFrequency->frequency_Hz());
    _keys.set(STOP_FREQUENCY_KEY, ui->stopFrequency->frequency_Hz());
    _keys.set(FREQUENCY_POINTS_KEY, quint32(ui->frequencyPoints->points()));
    _keys.set(IF_BW_KEY, ui->ifBw->frequency_Hz());

    // Power
    _keys.set(START_POWER_KEY, ui->startPower->power_dBm());
    _keys.set(STOP_POWER_KEY, ui->stopPower->power_dBm());
    _keys.set(POWER_POINTS_KEY, quint32(ui->powerPoints->points()));
    _keys.set(COMPRESSION_LEVEL_KEY, ui->compressionLevel->value_dB());
    _keys.set(IS_GAIN_EXPANSION_KEY, ui->gainExpansion->isChecked());
    _keys.set(IS_STOP_AT_COMPRESSION_KEY, ui->stopAtCompression->isChecked());
    _keys.set(POST_CONDITION_KEY, ui->postCondition->currentText());

    // Miscellaneous
    _keys.set(CHANNEL_KEY, ui->channel->currentText());
    _keys.set(OUTPUT_PORT_KEY, ui->outputPort->points());
    _keys.set(INPUT_PORT_KEY, ui->inputPort->points());
    _keys.set(SWEEP_TYPE_KEY, ui->sweepType->currentText());

    // Traces
    _keys.set(TRACES_KEY, ui->tracesWidget->traces());
}
bool MainWindow::processSettings() {
    if (!ui->startFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter start frequency");
        ui->startFrequency->selectAll();
        ui->startFrequency->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setStartFrequency(ui->startFrequency->frequency_Hz());
    }

    if (!ui->stopFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter stop frequency");
        ui->stopFrequency->selectAll();
        ui->stopFrequency->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setStopFrequency(ui->stopFrequency->frequency_Hz());
    }

    if (!ui->frequencyPoints->hasAcceptableInput()) {
        ui->error->showMessage("*Enter frequency points");
        ui->frequencyPoints->selectAll();
        ui->frequencyPoints->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setFrequencyPoints(ui->frequencyPoints->points());
    }

    if (!ui->ifBw->hasAcceptableInput()) {
        ui->error->showMessage("*Enter IF BW");
        ui->ifBw->selectAll();
        ui->ifBw->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setIfBw(ui->ifBw->frequency_Hz());
    }

    if (!ui->startPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter start power");
        ui->startPower->selectAll();
        ui->startPower->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setStartPower(ui->startPower->power_dBm());
    }

    if (!ui->stopPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter stop power");
        ui->stopPower->selectAll();
        ui->stopPower->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setStopPower(ui->stopPower->power_dBm());
    }

    if (!ui->powerPoints->hasAcceptableInput()) {
        ui->error->showMessage("*Enter power points");
        ui->powerPoints->selectAll();
        ui->powerPoints->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setPowerPoints(ui->powerPoints->points());
    }

    if (!ui->compressionLevel->hasAcceptableInput()) {
        ui->error->showMessage("*Enter compression level");
        ui->compressionLevel->selectAll();
        ui->compressionLevel->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setCompressionLevel(ui->compressionLevel->value_dB());
    }

    _settings.setGainExpansion(ui->gainExpansion->isChecked());
    _settings.setStopAtCompression(ui->stopAtCompression->isChecked());
    _settings.setRfOffPostCondition(ui->postCondition->currentText() == "RF Off");

    _settings.setChannel(ui->channel->currentText().toUInt());

    if (!ui->outputPort->hasAcceptableInput()) {
        ui->error->showMessage("*Enter output port");
        ui->outputPort->selectAll();
        ui->outputPort->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setOutputPort(ui->outputPort->points());
    }

    if (!ui->inputPort->hasAcceptableInput()) {
        ui->error->showMessage("*Enter input port");
        ui->inputPort->selectAll();
        ui->inputPort->setFocus();
        shake();
        return false;
    }
    else {
        _settings.setInputPort(ui->inputPort->points());
    }

    QString message;
    if (!_settings.isValid(_vna, message)) {
        ui->error->showMessage(message);
        shake();
        return false;
    }

    return true;
}

QRect MainWindow::progressGeometry() const {
    const int i = QApplication::desktop()->primaryScreen();
    QRect screen = QApplication::desktop()->screen(i)->geometry();

    QPoint center = screen.center();

    const int w = 600;
    const int h = 450;

    const int x = center.x() - w/2.0;
    const int y = center.y() - h/2.0;
    QRect rect = QRect(x, y, w, h);
    return rect;
}
QRect MainWindow::tracesGeometry() const {
    const int i = QApplication::desktop()->primaryScreen();
    QRect screen = QApplication::desktop()->screen(i)->geometry();

    QPoint center = screen.center();

    const int w = 600;
    const int h = 450;

    const int x = center.x() - w/2.0;
    const int y = center.y() - h/2.0;
    QRect rect = QRect(x, y, w, h);
    return rect;
}

void MainWindow::plotMaxGain(const QRowVector &frequency_Hz, const QRowVector &gain_dB) {
    const double upper = ui->plot->yAxis->range().upper;
    const double lower = ui->plot->yAxis->range().lower;

    double newUpper = max(gain_dB);
    double newLower = min(gain_dB);

    if (newUpper > upper || newLower < lower) {
        newUpper = std::max(newUpper, upper);
        newLower = std::min(newLower, lower);
        roundAxis(newLower, newUpper, 5.0, newLower, newUpper);
        ui->plot->yAxis->setRange(newLower, newUpper);
    }

    ui->plot->graph(0)->setData(frequency_Hz, gain_dB);
    ui->plot->replot();
}
void MainWindow::plotPinAtCompression(const QRowVector &frequency_Hz, const QRowVector &pin_dBm) {
    ui->plot->graph(1)->setData(frequency_Hz, pin_dBm);
    ui->plot->replot();
}
void MainWindow::measurementFinished() {
    _isMeasuring = false;
    disconnect(_thread.data(), SIGNAL(progress(int)),
            ui->progressBar, SLOT(setValue(int)));
    disconnect(_thread.data(), SIGNAL(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            this, SLOT(plotMaxGain(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    disconnect(_thread.data(), SIGNAL(startingSweep(QString,uint)),
            ui->timedProgressBar, SLOT(start(QString,uint)));
    disconnect(_thread.data(), SIGNAL(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)),
            this, SLOT(plotPinAtCompression(RsaToolbox::QRowVector,RsaToolbox::QRowVector)));
    disconnect(_thread.data(), SIGNAL(finishedSweep()),
            ui->timedProgressBar, SLOT(stop()));
    disconnect(_thread.data(), SIGNAL(finished()),
            this, SLOT(measurementFinished()));

    if (_thread->isError()) {
       ui->error->showMessage(_thread->errorMessage());
       shake();
       showSettingsPage();
    }
    else {
        _results.reset(_thread->takeResults());
        ui->exportData->setEnabled(true);
        showTracesPage();
    }

    _thread.reset();
    _vna.local();
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

void MainWindow::showProgressPage() {
    ui->exportData->setDisabled(true);
    ui->cancel->setText("Cancel");
    ui->measure->setText("Measure");
    ui->measure->setDisabled(true);

    _settingsGeometry = geometry();
    ui->pages->setCurrentWidget(ui->progressPage);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(_settingsGeometry);
    animation->setEndValue(progressGeometry());
    animation->start();
}
void MainWindow::showSettingsPage() {
    ui->cancel->setText("Close");
    ui->measure->setText("Measure");
    ui->measure->setEnabled(true);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::OutSine);
    animation->setStartValue(geometry());
    animation->setEndValue(_settingsGeometry);
    animation->start();

    ui->pages->setCurrentWidget(ui->settingsPage);
}
void MainWindow::showTracesPage() {
    ui->cancel->setText("Close");
    ui->measure->setText("Plot");
    ui->measure->setEnabled(true);
    ui->pages->setCurrentWidget(ui->tracesPage);

    ui->tracesWidget->setFrequencies(_results->frequencies_Hz());
    ui->tracesWidget->setPowers(_results->pin_dBm());

    // Need animation?
//    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
//    animation->setDuration(100);
//    animation->setEasingCurve(QEasingCurve::OutSine);
//    animation->setStartValue(geometry());
//    animation->setEndValue(tracesGeometry());
//    animation->start();
}

void MainWindow::setupPlot() {
    ui->plot->clearGraphs();

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
    ui->plot->graph(0)->setName("Max Gain");
    ui->plot->graph(0)->setPen(QPen(Qt::blue));
    ui->plot->graph(0)->setVisible(true);

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2);
    ui->plot->graph(1)->setName("Pin[Compression]");
    ui->plot->graph(0)->setPen(QPen(Qt::red));
    ui->plot->graph(1)->setVisible(true);

    ui->plot->yAxis2->setVisible(true);
    ui->plot->legend->setVisible(true);

    ui->plot->xAxis->setRange(_settings.startFrequency_Hz(), _settings.stopFrequency_Hz());
    ui->plot->xAxis->setLabel("Frequency (Hz)");

    ui->plot->yAxis->setRange(0, 100.0);
    ui->plot->yAxis->setLabel("Gain (dB)");

    ui->plot->yAxis2->setRange(_settings.startPower_dBm(), _settings.stopPower_dBm());
    ui->plot->yAxis2->setLabel("Power (dBm)");
}
