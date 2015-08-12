

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gettracename.h"
#include "ui_gettracename.h"

#include "axis_settings.h"
#include "ui_axis_settings.h"

#include "getCalibration.h"
#include "ui_getCalibration.h"

// RsaToolbox
#include <General.h>
#include <Touchstone.h>
using namespace RsaToolbox;

// Qt
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QVariant>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QTextStream>


// Constructor, destructor
MainWindow::MainWindow(Keys *keys, Log *log, QWidget *parent) :
    QMainWindow(parent), ui(new ::Ui::MainWindow) {
    
    ui->setupUi(this);
    ui->frequency_slider_label->setVisible(false);
    this->setWindowTitle(QString() +
                         APP_NAME + " Version " + APP_VERSION);
    ui->tab_widget->setCurrentIndex(0);

    ui->sweep_mode_combo_box->setCurrentIndex(1);
    ui->sweep_mode_label->hide();
    ui->sweep_mode_combo_box->hide();
    qDebug() << ui->sweep_mode_combo_box->currentText();

    ConnectMenuSignals();

    vna.reset(new Vna());
    this->_keys = keys;
    this->_log = log;
    UpdateStatus();
    UpdateInstrumentInfo();
    ToggleInputs(false);
    TogglePlots(false);
    ToggleSlider(false);

    _data.reset();

    _plotTitle = new QCPPlotTitle(ui->custom_plot, "");
    ui->custom_plot->plotLayout()->insertRow(0);
    ui->custom_plot->plotLayout()->addElement(0, 0, _plotTitle);

    LoadSettings();
}
MainWindow::~MainWindow()
{
    SaveSettings();
    delete ui;
}

// Initialize
void MainWindow::ConnectMenuSignals() {
    QObject::connect(ui->actionOpen, SIGNAL(triggered()),
                     this, SLOT(Open()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()),
                     this, SLOT(Save()));
    QObject::connect(ui->actionExport, SIGNAL(triggered()),
                     this, SLOT(Export()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()),
                     this, SLOT(About()));
}
void MainWindow::LoadSettings() {
    if (_keys->exists(CONNECTION_ADDRESS)) {
        QString address;
        _keys->get(CONNECTION_ADDRESS, address);
        ui->vna_address_line_edit->setText(address);
    }
    if (_keys->exists(CONNECTION_TYPE)) {
        QString type;
        _keys->get(CONNECTION_TYPE, type);
        ui->vna_connection_type_combo_box->setCurrentText(type);
    }
    if (_keys->exists(START_FREQ)) {
        QString start_freq;
        _keys->get(START_FREQ, start_freq);
        ui->start_frequency_line_edit->setText(start_freq);
    }
    if (_keys->exists(START_FREQ_UNITS)) {
        QString start_freq_units;
        _keys->get(START_FREQ_UNITS, start_freq_units);
        ui->start_freq_units_combo_box->setCurrentText(start_freq_units);
    }
    if (_keys->exists(STOP_FREQ)) {
        QString stop_freq;
        _keys->get(STOP_FREQ, stop_freq);
        ui->stop_frequency_line_edit->setText(stop_freq);
    }
    if (_keys->exists(STOP_FREQ_UNITS)) {
        QString stop_freq_units;
        _keys->get(STOP_FREQ_UNITS, stop_freq_units);
        ui->stop_freq_units_combo_box->setCurrentText(stop_freq_units);
    }
    if (_keys->exists(FREQ_POINTS)) {
        QString freq_points;
        _keys->get(FREQ_POINTS, freq_points);
        ui->frequency_points_line_edit->setText(freq_points);
    }
    if (_keys->exists(IF_BW)) {
        QString if_bw;
        _keys->get(IF_BW, if_bw);
        ui->if_value_combo_box->clear();
        if_mantissa_values.clear();
        if_mantissa_values << QStringList(if_bw);
        ui->if_value_combo_box->addItem(if_bw);
        ui->if_value_combo_box->setCurrentText(if_bw);
    }
    if (_keys->exists(IF_BW_UNITS)) {
        QString if_bw_units;
        _keys->get(IF_BW_UNITS, if_bw_units);
        if_units.clear();
        if_units << if_bw_units;
        ui->if_units_combo_box->clear();
        ui->if_units_combo_box->addItem(if_bw_units);
        ui->if_units_combo_box->setCurrentText(if_bw_units);
    }
    if (_keys->exists(START_POWER)) {
        QString start_power;
        _keys->get(START_POWER, start_power);
        ui->start_power_line_edit->setText(start_power);
    }
    if (_keys->exists(STOP_POWER)) {
        QString stop_power;
        _keys->get(STOP_POWER, stop_power);
        ui->stop_power_line_edit->setText(stop_power);
    }
    if (_keys->exists(POWER_POINTS)) {
        QString power_points;
        _keys->get(POWER_POINTS, power_points);
        ui->power_points_line_edit->setText(power_points);
    }
    if (_keys->exists(RECEIVE_ATTENUATION)) {
        QString receive_attenuation;
        _keys->get(RECEIVE_ATTENUATION, receive_attenuation);
        ui->receiver_attenuation_combo_box->addItem(receive_attenuation);
        ui->receiver_attenuation_combo_box->setCurrentText(receive_attenuation);
    }
    if (_keys->exists(SOURCE_ATTENUATION)) {
        QString source_attenuation;
        _keys->get(SOURCE_ATTENUATION, source_attenuation);
        ui->source_attenuation_combo_box->addItem(source_attenuation);
        ui->source_attenuation_combo_box->setCurrentText(source_attenuation);
    }
    if (_keys->exists(INPUT_PORT)) {
        QString input_port;
        _keys->get(INPUT_PORT, input_port);
        ui->input_port_combo_box->setCurrentText(input_port);
    }
    if (_keys->exists(OUTPUT_PORT)) {
        QString output_port;
        _keys->get(OUTPUT_PORT, output_port);
        ui->output_port_combo_box->setCurrentText(output_port);
    }
    if (_keys->exists(COMPRESSION_POINT)) {
        QString compression_point;
        _keys->get(COMPRESSION_POINT, compression_point);
        ui->compression_point_line_edit->setText(compression_point);
    }
    if (_keys->exists(POST_CONDITION)) {
        QString post_condition;
        _keys->get(POST_CONDITION, post_condition);
        ui->post_condition_combo_box->setCurrentText(post_condition);
    }
    if (_keys->exists(SWEEP_MODE)) {
        QString sweep_mode;
        _keys->get(SWEEP_MODE, sweep_mode);
        ui->sweep_mode_combo_box->setCurrentText(sweep_mode);
    }

    if (_keys->exists(OPEN_PATH))
        _keys->get(OPEN_PATH, open_path);
    else
        open_path = QDir::homePath();
    if (_keys->exists(SAVE_PATH))
        _keys->get(SAVE_PATH, save_path);
    else
        save_path = QDir::homePath();
    if (_keys->exists(EXPORT_PATH))
        _keys->get(EXPORT_PATH, export_path);
    else
        export_path = QDir::homePath();
    if (_keys->exists(PRINT_PATH))
        _keys->get(PRINT_PATH, print_path);
    else
        print_path = QDir::homePath();
}
void MainWindow::SaveSettings() {
    if (ui->vna_address_line_edit->text().isEmpty() == false)
        _keys->set(CONNECTION_ADDRESS,
                 ui->vna_address_line_edit->text());
    // connection type combo box
    _keys->set(CONNECTION_TYPE,
             ui->vna_connection_type_combo_box->currentText());
    if (ui->start_frequency_line_edit->hasAcceptableInput())
        _keys->set(START_FREQ,
                 ui->start_frequency_line_edit->text());
    _keys->set(START_FREQ_UNITS,
             ui->start_freq_units_combo_box->currentText());
    if (ui->stop_frequency_line_edit->hasAcceptableInput())
        _keys->set(STOP_FREQ,
                 ui->stop_frequency_line_edit->text());
    _keys->set(STOP_FREQ_UNITS,
             ui->stop_freq_units_combo_box->currentText());
    if (ui->frequency_points_line_edit->hasAcceptableInput())
        _keys->set(FREQ_POINTS,
                 ui->frequency_points_line_edit->text());
    _keys->set(IF_BW,
             ui->if_value_combo_box->currentText());
    _keys->set(IF_BW_UNITS,
             ui->if_units_combo_box->currentText());
    if (ui->start_power_line_edit->hasAcceptableInput())
        _keys->set(START_POWER,
                 ui->start_power_line_edit->text());
    if (ui->stop_power_line_edit->hasAcceptableInput())
        _keys->set(STOP_POWER,
                 ui->stop_power_line_edit->text());
    if (ui->power_points_line_edit->hasAcceptableInput())
        _keys->set(POWER_POINTS,
                 ui->power_points_line_edit->text());
    _keys->set(RECEIVE_ATTENUATION,
             ui->receiver_attenuation_combo_box->currentText());
    _keys->set(SOURCE_ATTENUATION,
             ui->source_attenuation_combo_box->currentText());
    _keys->set(INPUT_PORT,
             ui->input_port_combo_box->currentText());
    _keys->set(OUTPUT_PORT,
             ui->output_port_combo_box->currentText());
    if (ui->compression_point_line_edit->hasAcceptableInput())
        _keys->set(COMPRESSION_POINT,
                 ui->compression_point_line_edit->text());
    _keys->set(POST_CONDITION,
             ui->post_condition_combo_box->currentText());
    _keys->set(SWEEP_MODE,
             ui->sweep_mode_combo_box->currentText());

    _keys->set(OPEN_PATH, open_path);
    _keys->set(SAVE_PATH, save_path);
    _keys->set(EXPORT_PATH, export_path);
    _keys->set(PRINT_PATH, print_path);
}

// Update GUI
void MainWindow::UpdateStatus() {
    ui->status_bar->clearMessage();
    QString new_message;
    if (vna->isConnected()) {
        new_message = toString(vna->properties().model()) + " connected. Ready for measurements.";
    }
    else {
        new_message = "No instrument connected";
    }
    ui->status_bar->showMessage(new_message);
}
void MainWindow::UpdateInstrumentInfo() {
    if (vna->isConnected() && vna->properties().isKnownModel()) {
        VnaProperties::Model model = vna->properties().model();
        ports = vna->properties().physicalPorts();
        max_freq_Hz = vna->properties().maximumFrequency_Hz();
        min_freq_Hz = vna->properties().minimumFrequency_Hz();
        max_points = vna->properties().maximumPoints();
        source_attenuations = toStringList(vna->properties().sourceAttenuations_dB());
        receiver_attenuations = toStringList(vna->properties().receiverAttenuations_dB());
        if_mantissa_values.clear();
        if_mantissa_values << toStringList(vna->properties().ifBandwidthMantissa_Hz())
                           << toStringList(vna->properties().ifBandwidthMantissa_KHz())
                           << toStringList(vna->properties().ifBandwidthMantissa_MHz());
        if_units.clear();
        if_units << "Hz" << "KHz" << "MHz";
        min_power_dBm = vna->properties().minimumPower_dBm();
        max_power_dBm = vna->properties().maximumPower_dBm();

        ui->make_line_edit->setText("Rohde & Schwarz");
        ui->model_line_edit->setText(toString(model));
        ui->serial_no_line_edit->setText(vna->properties().serialNumber());
        ui->firmware_version_line_edit->setText(vna->properties().firmwareVersion());
        QString min_freq = formatValue(min_freq_Hz,
                                       1, // decimal place
                                       Units::Hertz);
        QString max_freq = formatValue(max_freq_Hz,
                                       1, // decimal place
                                       Units::Hertz);
        ui->frequency_range_line_edit->setText(min_freq + " to " + max_freq);
        QString min_power = formatValue(min_power_dBm,
                                        1, // decimal place
                                        Units::dBm);
        QString max_power = formatValue(max_power_dBm,
                                        1, // decimal place
                                        Units::dBm);
        ui->power_range_line_edit->setText(min_power + " to " + max_power);
        if (receiver_attenuations.size() == 1)
            ui->receiver_attenuators_line_edit->setText("None");
        else
            ui->receiver_attenuators_line_edit->setText(
                        receiver_attenuations.last()
                        + " dB in "
                        + QLocale().toString(QLocale().toInt(receiver_attenuations[1]) - QLocale().toInt(receiver_attenuations[0]))
                    + " dB steps");
        if (source_attenuations.size() == 1)
            ui->source_attenuators_line_edit->setText("None");
        else
            ui->source_attenuators_line_edit->setText(
                        source_attenuations.last()
                        + " dB in "
                        + QLocale().toString(QLocale().toInt(source_attenuations[1]) - QLocale().toInt(source_attenuations[0]))
                    + " dB steps");
    }
    else {
        ui->make_line_edit->setText("");
        ui->model_line_edit->setText("");
        ui->serial_no_line_edit->setText("");
        ui->firmware_version_line_edit->setText("");
        ui->frequency_range_line_edit->setText("");
        ui->power_range_line_edit->setText("");
        ui->receiver_attenuators_line_edit->setText("");
        ui->source_attenuators_line_edit->setText("");
    }
}
void MainWindow::UpdateValidators() {
    QScopedPointer<QIntValidator> int_validator;
    QScopedPointer<QDoubleValidator> double_validator;

    // Points
    int_validator.reset(new QIntValidator(1, max_points, ui->frequency_points_line_edit));
    ui->frequency_points_line_edit->setValidator(int_validator.take());
    int_validator.reset(new QIntValidator(1, max_points, ui->power_points_line_edit));
    ui->power_points_line_edit->setValidator(int_validator.take());

    // Frequency
    UpdateFreqValidators();

    // Power
    int decimal_places = 2;
    double_validator.reset(new QDoubleValidator(min_power_dBm,
                                                max_power_dBm,
                                                decimal_places,
                                                ui->start_power_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->start_power_line_edit->setValidator(double_validator.take());
    double_validator.reset(new QDoubleValidator(min_power_dBm,
                                                max_power_dBm,
                                                decimal_places,
                                                ui->stop_power_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->stop_power_line_edit->setValidator(double_validator.take());

    // IF BW
    UpdateIfBwValues();

    // Attenuation
    QString receive_value = ui->receiver_attenuation_combo_box->currentText();
    ui->receiver_attenuation_combo_box->clear();
    ui->receiver_attenuation_combo_box->addItems(receiver_attenuations);
    if (ui->receiver_attenuation_combo_box->findText(receive_value) != -1)
        ui->receiver_attenuation_combo_box->setCurrentText(receive_value);

    QString source_value = ui->source_attenuation_combo_box->currentText();
    ui->source_attenuation_combo_box->clear();
    ui->source_attenuation_combo_box->addItems(source_attenuations);
    if (ui->source_attenuation_combo_box->findText(source_value) != -1)
        ui->source_attenuation_combo_box->setCurrentText(source_value);

    // Ports
    QString input_value = ui->input_port_combo_box->currentText();
    QString output_value = ui->output_port_combo_box->currentText();
    ui->input_port_combo_box->clear();
    ui->output_port_combo_box->clear();
    for (int i = 1; i <= ports; i++) {
        ui->input_port_combo_box->addItem(QLocale().toString(i));
        ui->output_port_combo_box->addItem(QLocale().toString(i));
    }
    if (ui->input_port_combo_box->findText(input_value) != -1)
        ui->input_port_combo_box->setCurrentText(input_value);
    else
        ui->input_port_combo_box->setCurrentText("1");
    if (ui->output_port_combo_box->findText(output_value) != -1)
        ui->output_port_combo_box->setCurrentText(output_value);
    else
        ui->output_port_combo_box->setCurrentText("2");

    // Compression point
    double_validator.reset(new QDoubleValidator(0, // min
                                                10, // max
                                                1, // decimal places
                                                ui->compression_point_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->compression_point_line_edit->setValidator(double_validator.take());
}
void MainWindow::UpdateFreqValidators() {
    QScopedPointer<QDoubleValidator> double_validator;
    QString units = ui->start_freq_units_combo_box->currentText();
    units.chop(2);
    SiPrefix prefix = toSiPrefix(units);
    int decimal_places;
    if (prefix == SiPrefix::None)
        decimal_places = 0;
    else
        decimal_places = 3;
    double_validator.reset(new QDoubleValidator(min_freq_Hz/toDouble(prefix),
                                                max_freq_Hz/toDouble(prefix),
                                                decimal_places,
                                                ui->start_frequency_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->start_frequency_line_edit->setValidator(double_validator.take());
    units = ui->stop_freq_units_combo_box->currentText();
    units.chop(2);
    prefix = toSiPrefix(units);
    if (prefix == SiPrefix::None)
        decimal_places = 0;
    else
        decimal_places = 3;
    double_validator.reset(new QDoubleValidator(min_freq_Hz/toDouble(prefix),
                                                max_freq_Hz/toDouble(prefix),
                                                decimal_places,
                                                ui->stop_frequency_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->stop_frequency_line_edit->setValidator(double_validator.take());
}
void MainWindow::UpdateIfBwValues() {
    QObject::disconnect(ui->if_units_combo_box, SIGNAL(currentIndexChanged(QString)),
                        this, SLOT(on_if_units_combo_box_currentIndexChanged(QString)));
    QString units = ui->if_units_combo_box->currentText();
    QString value = ui->if_value_combo_box->currentText();
    ui->if_value_combo_box->clear();
    ui->if_units_combo_box->clear();

    ui->if_units_combo_box->addItems(if_units);
    int units_index = ui->if_units_combo_box->findText(units);
    if (units_index != -1) {
        ui->if_units_combo_box->setCurrentIndex(units_index);
        ui->if_value_combo_box->addItems(if_mantissa_values[units_index]);
        int value_index = ui->if_value_combo_box->findText(value);
        if (value_index != -1)
            ui->if_value_combo_box->setCurrentIndex(value_index);
        else
            ui->if_value_combo_box->setCurrentIndex(0);
    }
    else {
        int row = 0;
        if (if_units.contains("KHz", Qt::CaseInsensitive))
            row = if_units.indexOf("KHz", Qt::CaseInsensitive);
        ui->if_units_combo_box->setCurrentIndex(row);
        ui->if_value_combo_box->addItems(if_mantissa_values[row]);
        ui->if_value_combo_box->setCurrentIndex(0);
    }

    QObject::connect(ui->if_units_combo_box, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(on_if_units_combo_box_currentIndexChanged(QString)));
}

// Toggle Inputs
void MainWindow::ToggleConnect(bool enabled) {
    ui->vna_address_line_edit->setEnabled(enabled);
    ui->vna_connection_type_combo_box->setEnabled(enabled);
    ui->vna_connect_push_button->setEnabled(enabled);
}
void MainWindow::ToggleInputs(bool enabled) {
    ui->start_frequency_line_edit->setEnabled(enabled);
    ui->start_freq_units_combo_box->setEnabled(enabled);
    ui->stop_frequency_line_edit->setEnabled(enabled);
    ui->stop_freq_units_combo_box->setEnabled(enabled);
    ui->frequency_points_line_edit->setEnabled(enabled);
    ui->if_units_combo_box->setEnabled(enabled);
    ui->if_value_combo_box->setEnabled(enabled);
    ui->start_power_line_edit->setEnabled(enabled);
    ui->stop_power_line_edit->setEnabled(enabled);
    ui->power_points_line_edit->setEnabled(enabled);
    ui->receiver_attenuation_combo_box->setEnabled(enabled);
    ui->source_attenuation_combo_box->setEnabled(enabled);
    ui->input_port_combo_box->setEnabled(enabled);
    ui->output_port_combo_box->setEnabled(enabled);
    ui->compression_point_line_edit->setEnabled(enabled);
    ui->post_condition_combo_box->setEnabled(enabled);
    ui->sweep_mode_combo_box->setEnabled(enabled);
    ui->calibration_push_button->setEnabled(enabled);
    ui->retrieve_settings_push_button->setEnabled(enabled);
    ui->measure_push_button->setEnabled(enabled);
}
void MainWindow::TogglePlots(bool enabled) {
    ui->plot_type_combo_box->setEnabled(enabled);
    ui->print_plot_push_button->setEnabled(enabled);
    ui->actionExport->setEnabled(enabled);
    ui->actionSave->setEnabled(enabled);
    ui->axis_push_button->setEnabled(enabled);
    if (enabled == false) {
        ToggleSlider(false);
    }
}
void MainWindow::ToggleSlider(bool enabled) {
    ui->frequency_slider->setVisible(enabled);
    ui->frequency_slider_label->setVisible(enabled);
    if (enabled == false) {
        QObject::disconnect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                            this, SLOT(PlotGainVsPin()));
        QObject::disconnect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                            this, SLOT(PlotPinVsPout()));
    }
}

// Run sweeps
bool MainWindow::isValidInput() {
    if (ui->start_frequency_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The start frequency is invalid.");
        ui->start_frequency_line_edit->setFocus();
        ui->start_frequency_line_edit->selectAll();
        return(false);
    }

    if (ui->stop_frequency_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The stop frequency is invalid.");
        ui->stop_frequency_line_edit->setFocus();
        ui->stop_frequency_line_edit->selectAll();
        return(false);
    }

    double start_freq, stop_freq;
    QString start_freq_units, stop_freq_units;
    SiPrefix start_prefix, stop_prefix;
    start_freq = QLocale().toDouble(ui->start_frequency_line_edit->text());
    start_freq_units = ui->start_freq_units_combo_box->currentText();
    start_freq_units.chop(2); // chop off "Hz"
    start_prefix = toSiPrefix(start_freq_units);
    stop_freq = QLocale().toDouble(ui->stop_frequency_line_edit->text());
    stop_freq_units = ui->stop_freq_units_combo_box->currentText();
    stop_freq_units.chop(2); // chop off "Hz"
    stop_prefix = toSiPrefix(stop_freq_units);
    if (start_freq * toDouble(start_prefix) >= stop_freq * toDouble(stop_prefix)) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "Stop frequency must be greater than start frequency");
        ui->stop_frequency_line_edit->setFocus();
        ui->stop_frequency_line_edit->selectAll();
        return(false);
    }

    if (ui->frequency_points_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The number of frequency sweep points is invalid.");
        ui->frequency_points_line_edit->setFocus();
        ui->frequency_points_line_edit->selectAll();
        return(false);
    }

    if (ui->start_power_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The start power is invalid.");
        ui->start_power_line_edit->setFocus();
        ui->start_power_line_edit->selectAll();
        return(false);
    }

    if (ui->stop_power_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The stop power is invalid.");
        ui->stop_power_line_edit->setFocus();
        ui->stop_power_line_edit->selectAll();
        return(false);
    }

    if (ui->power_points_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The number of power sweep points is invalid.");
        ui->power_points_line_edit->setFocus();
        ui->power_points_line_edit->selectAll();
        return(false);
    }

    if (ui->compression_point_line_edit->hasAcceptableInput() == false) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The compression point is invalid.");
        ui->compression_point_line_edit->setFocus();
        ui->compression_point_line_edit->selectAll();
        return(false);
    }

    if (ui->input_port_combo_box->currentIndex() == ui->output_port_combo_box->currentIndex()) {
        QMessageBox::warning(this,
                             "PA Compression Test",
                             "The *input and output port cannot be the same.");
        ui->output_port_combo_box->setFocus();
        return(false);
    }

    //else
    return(true);
}
void MainWindow::GetInput() {
    _data.compressionLevel_dB = QLocale().toDouble(ui->compression_point_line_edit->text());
    _data.inputPort = ui->input_port_combo_box->currentIndex() + 1;
    _data.outputPort = ui->output_port_combo_box->currentIndex() + 1;
    _data.startPower_dBm = QLocale().toDouble(ui->start_power_line_edit->text());
    _data.stopPower_dBm = QLocale().toDouble(ui->stop_power_line_edit->text());
    _data.powerPoints = QLocale().toInt(ui->power_points_line_edit->text());

    const double start_freq = QLocale().toDouble(ui->start_frequency_line_edit->text());
    QString start_freq_prefix = ui->start_freq_units_combo_box->currentText();
    start_freq_prefix.chop(2);
    const SiPrefix start_freq_SiPrefix = toSiPrefix(start_freq_prefix);
    _data.startFreq_Hz = start_freq * toDouble(start_freq_SiPrefix);

    const double stop_freq = QLocale().toDouble(ui->stop_frequency_line_edit->text());
    QString stop_freq_prefix = ui->stop_freq_units_combo_box->currentText();
    stop_freq_prefix.chop(2);
    const SiPrefix stop_freq_SiPrefix = toSiPrefix(stop_freq_prefix);
    _data.stopFreq_Hz = stop_freq * toDouble(stop_freq_SiPrefix);

    _data.frequencyPoints = QLocale().toInt(ui->frequency_points_line_edit->text());

    const double if_bw = QLocale().toDouble(ui->if_value_combo_box->currentText());
    QString if_bw_prefix = ui->if_units_combo_box->currentText();
    if_bw_prefix.chop(2);
    SiPrefix if_bw_SiPrefix = toSiPrefix(if_bw_prefix);
    _data.ifBw_Hz = if_bw * toDouble(if_bw_SiPrefix);

    _data.sourceAttenuation_dB = QLocale().toInt(ui->source_attenuation_combo_box->currentText());
    _data.receiverAttenuation_dB = QLocale().toInt(ui->receiver_attenuation_combo_box->currentText());

    _data.processSettings();
}
void MainWindow::Progress(int percent) {
    if (percent < 100) {
        QString message = "Measurement sweeps " + QLocale().toString(percent) + "% complete";
        ui->status_bar->showMessage(message);
    }
    else {
        ui->status_bar->showMessage("Writing Compression Points to Instrument...");
    }
}
void MainWindow::Finished() {
    run_sweeps->disconnect();

    // Post-condition
    QString post_condition = ui->post_condition_combo_box->currentText();
    if (post_condition == "RF Off")
        vna->settings().rfOutputPowerOff();
    else if (post_condition == "Minimum Power") {
        vna->channel().linearSweep().setPower(_data.startPower_dBm);
        vna->channel(2).linearSweep().setPower(_data.startPower_dBm);
    }
    else if (post_condition == "Maximum Power") {
        vna->channel().linearSweep().setPower(_data.stopPower_dBm);
        vna->channel(2).linearSweep().setPower(_data.stopPower_dBm);
    }
    double min = RsaToolbox::min(_data.referenceGain_dB);
    double max = RsaToolbox::max(_data.referenceGain_dB);
    if (min > 0)
        min = 0;
    roundAxis(min, max, 5.0, min, max);
    vna->trace("Trc1").setYAxis(min, max);
    vna->local();

    // Enable GUI options
    ToggleConnect(true);
    UpdateStatus();
    ui->actionOpen->setEnabled(true);
    TogglePlots(true);
    ToggleInputs(true);
    ui->frequency_slider->setMaximum(_data.frequencyPoints-1);
    ui->frequency_slider->setValue(1);
    ui->frequency_slider->setValue(0);
    ui->plot_type_combo_box->setEnabled(true);
    ui->plot_type_combo_box->setCurrentText("Reflection");
    ui->plot_type_combo_box->setCurrentText("Pin Compression");

    ui->tab_widget->setCurrentIndex(2);
    vna->isError();
    vna->clearStatus();
}

// Plot
void MainWindow::PlotReflection() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();


    _plotTitle->setTextColor(QColor(Qt::black));
    _plotTitle->setFont(QFont("Helvetica", 12));
    _plotTitle->setText("Reflection Coefficients");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);

    QRowVector scaled_freq = multiply(_data.frequencies_Hz, 1E-9);

    // S11
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Input");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(0)->setData(scaled_freq, _data.s11_dB);

    // S22
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Output");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(1)->setData(scaled_freq, _data.s22_dB);

    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (GHz)");
    ui->custom_plot->xAxis->setRange(scaled_freq.first(), scaled_freq.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Reflection Coefficient (dB)");
    ui->custom_plot->yAxis->setRange(qMin(min(_data.s11_dB), min(_data.s22_dB)) - 0.1,
                                     qMax(max(_data.s11_dB), max(_data.s22_dB)) + 0.1);

    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotPinVsPout() {
    int frequency_index = ui->frequency_slider->value();

    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    _plotTitle->setTextColor(QColor(Qt::black));
    _plotTitle->setFont(QFont("Helvetica", 12));
    _plotTitle->setText("Power Sweep: Pin vs Pout");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    // Pin vs Pout
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Measured");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(_data.power_dBm, _data.powerOut_dBm[frequency_index]);

    // Ideal Pin vs Pout (no compression)
    QVector<double> ideal_Pout;
    ideal_Pout.resize(_data.powerPoints);
    for (uint i = 0; i < _data.powerPoints; i++)
        ideal_Pout[i] = _data.power_dBm[i] + _data.referenceGain_dB[frequency_index];
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Ideal");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(1)->setData(_data.power_dBm, ideal_Pout);

    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Pin (dBm)");
    ui->custom_plot->xAxis->setRange(_data.power_dBm.first(), _data.power_dBm.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Pout (dBm)");
    ui->custom_plot->yAxis->setRange(qMin(min(ideal_Pout), min(_data.powerOut_dBm[frequency_index])) - 0.1,
                                     qMax(max(ideal_Pout), max(_data.powerOut_dBm[frequency_index])) + 0.1);


    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotGainVsFreq() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    _plotTitle->setTextColor(QColor(Qt::black));
    _plotTitle->setFont(QFont("Helvetica", 12));
    _plotTitle->setText("Nominal Gain vs Frequency");

    ui->custom_plot->legend->setVisible(false);

    // Nominal gain
    QRowVector scaled_freq = multiply(_data.frequencies_Hz, 1E-9);
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(scaled_freq, _data.referenceGain_dB);

    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (GHz)");
    ui->custom_plot->xAxis->setRange(scaled_freq.first(), scaled_freq.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Nominal Gain (dB)");
    ui->custom_plot->yAxis->setRange(min(_data.referenceGain_dB) - 0.1,
                                     max(_data.referenceGain_dB) + 0.1);

    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotGainVsPin() {
    int freq_index = ui->frequency_slider->value();

    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    _plotTitle->setTextColor(QColor(Qt::black));
    _plotTitle->setFont(QFont("Helvetica", 12));
    _plotTitle->setText("Gain vs Pin");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom);

    // Nominal gain
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Gain vs Pin");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(_data.power_dBm, _data.gain_dB[freq_index]);

    QRowVector compression_points;
    compression_points.fill(_data.referenceGain_dB[freq_index] - _data.compressionLevel_dB, _data.powerPoints);
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Compression Point");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(1)->setData(_data.power_dBm, compression_points);

    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Pin (dBm)");
    ui->custom_plot->xAxis->setRange(_data.power_dBm.first(), _data.power_dBm.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Gain (dB)");
    ui->custom_plot->yAxis->setRange(qMin(compression_points[0], min(_data.gain_dB[freq_index])) - 0.1,
            qMax(compression_points[0], max(_data.gain_dB[freq_index])) + 0.1);

    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotPinCompression() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    _plotTitle->setFont(QFont("Helvetica", 12));
    if (_data.powerInAtCompression_dBm.size() == 0) {
        _plotTitle->setTextColor(QColor(Qt::red));
        _plotTitle->setText("Compression point not found.");
        ui->custom_plot->legend->setVisible(false);
        ui->custom_plot->replot();
        return;
    }
    else
        _plotTitle->setText(
                    formatValue(_data.compressionLevel_dB, 1, Units::NoUnits)
                    + " dB Input-Referred Compression Point");

    ui->custom_plot->legend->setVisible(false);

    // Pin compression points
    QRowVector scaled_freq = multiply(_data.compressionFrequencies_Hz, 1.0E-9);
    ui->custom_plot->addGraph();
    // ui->custom_plot->graph(0)->setName("Pin");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(scaled_freq, _data.powerInAtCompression_dBm);

    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (GHz)");
    ui->custom_plot->xAxis->setRange(scaled_freq.first(), scaled_freq.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Compression Point (Pin, dBm)");
    ui->custom_plot->yAxis->setRange(min(_data.powerInAtCompression_dBm) - 0.1,
                                     max(_data.powerInAtCompression_dBm) + 0.1);

    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotPoutCompression() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    _plotTitle->setFont(QFont("Helvetica", 12));
    if (_data.powerOutAtCompression_dBm.size() == 0) {
        _plotTitle->setTextColor(QColor(Qt::red));
        _plotTitle->setText("Compression point not found.");
        ui->custom_plot->legend->setVisible(false);
        ui->custom_plot->replot();
        return;
    }
    else
        _plotTitle->setText(
                    formatValue(_data.compressionLevel_dB, 1, Units::NoUnits)
                    + " dB Output-Referred Compression Point");

    ui->custom_plot->legend->setVisible(false);

    // Pout
    QRowVector scaled_freq = multiply(_data.compressionFrequencies_Hz, 1E-9);
    ui->custom_plot->addGraph();
    // ui->custom_plot->graph(0)->setName("Pout");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(0)->setData(scaled_freq, _data.powerOutAtCompression_dBm);


    ui->custom_plot->axisRect()->setupFullAxesBox();
    // ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (GHz)");
    ui->custom_plot->xAxis->setRange(scaled_freq.first(), scaled_freq.last());
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Compression Point (Pout, dBm)");
    ui->custom_plot->yAxis->setRange(min(_data.powerOutAtCompression_dBm) - 0.1,
                                     max(_data.powerOutAtCompression_dBm) + 0.1);

    ui->custom_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteractions(QCP::iSelectPlottables);

    ui->custom_plot->replot();
}

// Export Data
void MainWindow::Export() {
    QString path = QFileDialog::getSaveFileName(this,
                                                "Export as...",
                                                export_path,
                                                "Zip (*.zip)");
    if (path.isEmpty())
        return;

    if (!_data.exportToZip(path)) {
        QMessageBox::warning(this,
                             APP_NAME,
                             "Export failed: could not write to directory");
    }
    else {
        export_path = QFileInfo(path).path();
    }
}

// Open, Save
void MainWindow::Open() {
    QString path = QFileDialog::getOpenFileName(this, "Open...", open_path, "PA Compression Test (*.amp)");
    if (path.isEmpty())
        return;

    ui->status_bar->showMessage(QString("Opening ")
                                + path
                                + "... Please wait");
    if (!_data.open(path)) {
        TogglePlots(false);
        QFileInfo file_info(path);
        QMessageBox::warning(this,
                             "RSA PA Compression Test",
                             QString() + "Could not open file " + file_info.fileName()
                             + " in " + QDir::toNativeSeparators(file_info.absolutePath()));
        UpdateStatus();
        return;
    }

    open_path = QFileInfo(path).path();
    TogglePlots(true);
    ui->plot_type_combo_box->setCurrentText("Reflection");
    ui->plot_type_combo_box->setCurrentText("Pin Compression");
    ui->tab_widget->setCurrentIndex(2);
    UpdateStatus();
}
void MainWindow::Save() {
    QString path = QFileDialog::getSaveFileName(this, "Save as", save_path, "PA Compression Test (*.amp)");
    if (path.isEmpty())
        return;
    if (path.endsWith(".amp", Qt::CaseInsensitive) == false)
        path.append(".amp");

    ui->status_bar->showMessage(QString("Saving data to ")
                                + path
                                + "... Please wait");
    if (!_data.save(path)) {
        QFileInfo file_info(path);
        QMessageBox::warning(this,
                             "RSA PA Compression Test",
                             QString() + "Could not create file " + file_info.fileName()
                             + " in " + file_info.absolutePath());
        UpdateStatus();
        return;
    }

    UpdateStatus();
}

void MainWindow::About() {

}

//////// GUI CONTROLS ////////////////

// Connection
void MainWindow::on_vna_connect_push_button_clicked()
{
    if (ui->vna_connect_push_button->text() == "Connect") {
        ConnectionType connection_type;
        QString address;
        if (ui->vna_connection_type_combo_box->currentText() == "GPIB")
            connection_type = GPIB_CONNECTION;
        else
            connection_type = TCPIP_CONNECTION;
        address = ui->vna_address_line_edit->text();
        vna.reset(new Vna(connection_type, address));
        vna->useLog(_log);
        vna->printInfo();

        if (vna->isConnected()) {
            ui->vna_connect_push_button->setText("Disconnect");
            UpdateStatus();
            UpdateInstrumentInfo();
            UpdateValidators();
            ToggleInputs(true);
            vna->lock();
            vna->settings().displayOn();
            vna->local();
        }
        else {
            QString warning = QString()
                    + "Could not connect to VNA at "
                    + ui->vna_connection_type_combo_box->currentText()
                    + " address " + address
                    + "\nPlease check your setup and try again.";
            QMessageBox::warning(this, "RSA PA Compression Test", warning);
            return;
        }
    }
    else { // Disconnect
        vna.reset(new Vna());
        ui->vna_connect_push_button->setText("Connect");
        UpdateStatus();
        UpdateInstrumentInfo();
        ToggleInputs(false);
    }
}

// Settings, Measure
void MainWindow::on_start_freq_units_combo_box_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    UpdateFreqValidators();
}
void MainWindow::on_stop_freq_units_combo_box_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    UpdateFreqValidators();
}
void MainWindow::on_if_units_combo_box_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    UpdateIfBwValues();
}
void MainWindow::on_retrieve_settings_push_button_clicked()
{
    QStringList traces = vna->traces();
    QString trace_name;
    if (traces.size() == 0) {
        QMessageBox::warning(this,
                             APP_NAME,
                             "There must be at least one trace present on the network analyzer to retrieve settings");
        return;
    }
    else if (traces.size() == 1) {
        trace_name = traces.front();
        QMessageBox::information(this,
                                 "RSA PA Compression Test",
                                 "Measurement settings retrieved from trace \"" + trace_name + "\".");
    }
    else {
        getTraceName w(traces, this);
        w.exec();
        trace_name = w.traceName();
    }
    if (!trace_name.isEmpty()) {
        uint i = vna->trace(trace_name).channel();
        uint input_port;
        uint output_port;
        NetworkParameter parameter;
        vna->trace(trace_name).networkParameter(parameter, output_port, input_port);
        ui->input_port_combo_box->setCurrentText(QLocale().toString(input_port));
        ui->output_port_combo_box->setCurrentText(QLocale().toString(output_port));
        if (vna->properties().hasSourceAttenuators()) {
            ui->source_attenuation_combo_box->setCurrentText(
                        QLocale().toString(vna->channel(i).sourceAttenuation_dB(input_port)));
        }
        else {
            ui->source_attenuation_combo_box->setCurrentText("0");
        }
        if (vna->properties().hasReceiverAttenuators()) {
            ui->receiver_attenuation_combo_box->setCurrentText(
                        QLocale().toString(vna->channel(i).receiverAttenuation_dB(output_port)));
        }
        else {
            ui->receiver_attenuation_combo_box->setCurrentText("0");
        }
        QStringList list = formatValue(vna->channel(i).linearSweep().ifBandwidth_Hz(), 0, Units::Hertz).split(' ');
        ui->if_units_combo_box->setCurrentText(list[1]);
        ui->if_value_combo_box->setCurrentText(list[0]);
        VnaChannel::SweepType sweep_type = vna->channel(i).sweepType();
        if (sweep_type == VnaChannel::SweepType::Linear) {
            QStringList list = formatValue(vna->channel(i).linearSweep().start_Hz(), 3, Units::Hertz).split(' ');
            ui->start_freq_units_combo_box->setCurrentText(list[1]);
            ui->start_frequency_line_edit->setText(list[0]);
            list = formatValue(vna->channel(i).linearSweep().stop_Hz(), 3, Units::Hertz).split(' ');
            ui->stop_freq_units_combo_box->setCurrentText(list[1]);
            ui->stop_frequency_line_edit->setText(list[0]);
            ui->frequency_points_line_edit->setText(QLocale().toString(vna->channel(i).linearSweep().points()));
            ui->sweep_mode_combo_box->setCurrentText("Frequency");
        }
        else if (sweep_type = VnaChannel::SweepType::Power) {
            ui->start_power_line_edit->setText(QLocale().toString(vna->channel(i).powerSweep().start_dBm()));
            ui->stop_power_line_edit->setText(QLocale().toString(vna->channel(i).powerSweep().stop_dBm()));

            // FIX
//            ui->compression_point_line_edit->setText(QLocale().toString(vna->channel(i).compressionLevel_dBm()));

            ui->power_points_line_edit->setText(QLocale().toString(vna->channel(i).powerSweep().points()));
            ui->sweep_mode_combo_box->setCurrentText("Power");
        }
        QString cal_group = vna->channel(i).calGroup();
        if (!cal_group.isEmpty()) {
            ui->calibration_line_edit->setText(cal_group + ".cal");
        }
        else if (vna->channel(i).isCalibrated()) {
            // Save calibration for use later?
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Save Calibration?",
                                          QString("The calibration for trace \'%1\' is not saved\n"
                                                  "Save calibration?").arg(trace_name),
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                // save, use cal

            }
        }
//        else if(vna->channel(i).isCalibrationPresent()
//                || vna->Channel(i).isPowerCalPresent()) {
//            // local channel cal
//            ui->calibration_line_edit->setText("Channel " + QLocale().toString(i));
//        }
        else {
            ui->calibration_line_edit->setText("None");
        }
    }
    vna->local();
}
void MainWindow::on_calibration_push_button_clicked() {
//    QVector<uint> channels = vna->GetChannels();
//    int number_of_channels = channels.size();
//    QStringList calibrations;
//    for (int i = 0; i < number_of_channels; i++) {
//        if ((vna->Channel(channels[i]).isCalibrationPresent()
//             || vna->Channel(channels[i]).isPowerCalPresent())
//                && vna->Channel(channels[i]).GetCalGroup().isEmpty()) {
//            // Local calibration
//            calibrations.append("Channel " + QLocale().toString(channels[i]));
//        }
//        else {
//            channels.remove(i);
//            number_of_channels--;
//        }
//    }
//    calibrations << vna->GetCalGroups();
//    vna->Local();

    getCalibration cal_menu(vna.data(), this);
    cal_menu.exec();
    if (cal_menu.isCalSelected()) {
        ui->calibration_line_edit->setText(cal_menu.selectedCal());
    }
    else {
        ui->calibration_line_edit->setText("None");
    }
}
void MainWindow::on_measure_push_button_clicked() {
    if (isValidInput() == false)
        return;

    // Test instrument for disconnect
    if (vna->idString().isEmpty()) {
        QMessageBox::warning(this,
                             "Instrument not found",
                             "Instrument is not responding. The VNA appears to have disconnected.");
        on_vna_connect_push_button_clicked();
        return;
    }

    ToggleInputs(false);
    TogglePlots(false);
    ui->actionOpen->setDisabled(true);
    ToggleConnect(false);
    GetInput();

    vna->remote();

    // Handle calibration before preset
    bool apply_cal;
    bool delete_cal;
    QString calibration = ui->calibration_line_edit->text();
    if (!calibration.isEmpty() && calibration != "None") {
        apply_cal = true;
        delete_cal = false;
    }
//    if (calibration.contains(".cal")) {
//        calibration.chop(4);
//        apply_cal = true;
//        delete_cal = false;
//    } else if (calibration != "None") {
//        apply_cal = true;
//        delete_cal = true;
//        calibration = "channel_cal";
//        vna->Channel().SaveCalGroup(calibration);
//    }
    else {
        apply_cal = false;
        delete_cal = false;
    }

    // Clear instrument
    vna->settings().calibrationPresetOff();
    vna->settings().userPresetOff();
    vna->clearStatus();
    vna->preset();
    vna->settings().displayOn();

    // Apply calibration
    if (apply_cal) {
        vna->channel().setCalGroup(calibration);
    }
    if (delete_cal) {
        vna->channel().dissolveCalGroup();
        vna->deleteCalGroup(calibration);
    }

    // Basic settings
    if (calibration.contains(".cal")) {
        calibration.chop(4);
        vna->channel().setCalGroup(calibration);
    }
    vna->channel().linearSweep().setIfbandwidth(_data.ifBw_Hz);
    if (vna->properties().hasSourceAttenuators() && source_attenuations.size() != 1)
        vna->channel().setSourceAttenuations(_data.sourceAttenuation_dB);
    if (vna->properties().hasReceiverAttenuators() && receiver_attenuations.size() != 1)
        vna->channel().setReceiverAttenuations(_data.receiverAttenuation_dB);

    // Start measurement
    qDebug() << "measure -> Start Measurment...";
    ui->status_bar->showMessage("Initializing...");
    run_sweeps.reset(new RunSweeps(vna.data(), &_data));
    QObject::connect(run_sweeps.data(), SIGNAL(finished()),
                     this, SLOT(Finished()));
    QObject::connect(run_sweeps.data(), SIGNAL(progress(int)),
                     this, SLOT(Progress(int)));
    vna->moveToThread(run_sweeps.data());
    run_sweeps->start();
}

// Plot, data
void MainWindow::on_plot_type_combo_box_currentIndexChanged(const QString &arg1)
{
    ToggleSlider(false);
    if (arg1 == "Reflection") {
        PlotReflection();
    }
    else if (arg1 == "Pin vs Pout") {
        ToggleSlider(true);
        QObject::connect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                         this, SLOT(PlotPinVsPout()));
        PlotPinVsPout();
    }
    else if (arg1 == "Nominal Gain") {
        PlotGainVsFreq();
    }
    else if (arg1 == "Gain vs Pin") {
        ToggleSlider(true);
        QObject::connect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                         this, SLOT(PlotGainVsPin()));
        PlotGainVsPin();
    }
    else if (arg1 == "Pin Compression") {
        PlotPinCompression();
    }
    else { // "Pout Compression" point plot
        PlotPoutCompression();
    }
}
void MainWindow::on_axis_push_button_clicked() {
    QCPRange xRange = ui->custom_plot->xAxis->range();
    QCPRange yRange = ui->custom_plot->yAxis->range();
    double xMin = xRange.lower,
            xMax = xRange.upper,
            yMin = yRange.lower,
            yMax = yRange.upper;
    axis_settings w(xMin, xMax,
                    yMin, yMax,
                    this);
    w.exec();
    ui->custom_plot->xAxis->setRange(xMin, xMax);
    ui->custom_plot->yAxis->setRange(yMin, yMax);
    ui->custom_plot->replot();
}
void MainWindow::on_frequency_slider_valueChanged(int value)
{
    ui->frequency_slider_label->setText(formatValue(_data.frequencies_Hz[value], 3, Units::Hertz));
}
void MainWindow::on_print_plot_push_button_clicked()
{
    QString save_file_name = QFileDialog::getSaveFileName(this,
                                                          "Save As",
                                                          print_path,
                                                          "PDF (*.pdf);;JPEG (*.jpg);;PNG (*.png);;Bitmap (*.bmp)");
    bool success;
    QFileInfo save_file(save_file_name);
    QString suffix = save_file.suffix();
    if (suffix.toLower() == "pdf")
        success = ui->custom_plot->savePdf(save_file_name);
    else if (suffix.toLower() == "jpg" || suffix.toLower() == "jpeg")
        success = ui->custom_plot->saveJpg(save_file_name);
    else if (suffix.toLower() == "png")
        success = ui->custom_plot->savePng(save_file_name);
    else if (suffix.toLower() == "bmp")
        success = ui->custom_plot->saveBmp(save_file_name);

    if (success == true)
        print_path = QFileInfo(save_file_name).path();
}
