

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Rsa
#include "General.h"
#include "Touchstone.h"
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
MainWindow::MainWindow(Key *key, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    
    ui->setupUi(this);
    ui->frequency_slider_label->setVisible(false);
    this->setWindowTitle(QString() +
                         APP_NAME + " Version " + APP_VERSION);

    ConnectMenuSignals();

    vna.reset(new Vna());
    this->key = key;
    UpdateStatus();
    UpdateInstrumentInfo();
    ToggleInputs(false);
    TogglePlots(false);
    ToggleSlider(false);

    power_sweeps_dBm.clear();
    frequency_points_Hz.clear();
    power_points_dBm.clear();
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
}
void MainWindow::LoadSettings() {
    if (key->Exists(CONNECTION_ADDRESS)) {
        QString address;
        key->Get(CONNECTION_ADDRESS, address);
        ui->vna_address_line_edit->setText(address);
    }
    if (key->Exists(CONNECTION_TYPE)) {
        QString type;
        key->Get(CONNECTION_TYPE, type);
        ui->vna_connection_type_combo_box->setCurrentText(type);
    }
    if (key->Exists(START_FREQ)) {
        QString start_freq;
        key->Get(START_FREQ, start_freq);
        ui->start_frequency_line_edit->setText(start_freq);
    }
    if (key->Exists(START_FREQ_UNITS)) {
        QString start_freq_units;
        key->Get(START_FREQ_UNITS, start_freq_units);
        ui->start_freq_units_combo_box->setCurrentText(start_freq_units);
    }
    if (key->Exists(STOP_FREQ)) {
        QString stop_freq;
        key->Get(STOP_FREQ, stop_freq);
        ui->stop_frequency_line_edit->setText(stop_freq);
    }
    if (key->Exists(STOP_FREQ_UNITS)) {
        QString stop_freq_units;
        key->Get(STOP_FREQ_UNITS, stop_freq_units);
        ui->stop_freq_units_combo_box->setCurrentText(stop_freq_units);
    }
    if (key->Exists(FREQ_POINTS)) {
        QString freq_points;
        key->Get(FREQ_POINTS, freq_points);
        ui->frequency_points_line_edit->setText(freq_points);
    }
    if (key->Exists(IF_BW_UNITS)) {
        QString if_bw_units;
        key->Get(IF_BW_UNITS, if_bw_units);
        ui->if_units_combo_box->setCurrentText(if_bw_units);
    }
    if (key->Exists(IF_BW)) {
        QString if_bw;
        key->Get(IF_BW, if_bw);
        ui->if_value_combo_box->addItem(if_bw);
        ui->if_value_combo_box->setCurrentText(if_bw);
    }
    if (key->Exists(START_POWER)) {
        QString start_power;
        key->Get(START_POWER, start_power);
        ui->start_power_line_edit->setText(start_power);
    }
    if (key->Exists(STOP_POWER)) {
        QString stop_power;
        key->Get(STOP_POWER, stop_power);
        ui->stop_power_line_edit->setText(stop_power);
    }
    if (key->Exists(POWER_POINTS)) {
        QString power_points;
        key->Get(POWER_POINTS, power_points);
        ui->power_points_line_edit->setText(power_points);
    }
    if (key->Exists(RECEIVE_ATTENUATION)) {
        QString receive_attenuation;
        key->Get(RECEIVE_ATTENUATION, receive_attenuation);
        ui->receiver_attenuation_combo_box->addItem(receive_attenuation);
        ui->receiver_attenuation_combo_box->setCurrentText(receive_attenuation);
    }
    if (key->Exists(SOURCE_ATTENUATION)) {
        QString source_attenuation;
        key->Get(SOURCE_ATTENUATION, source_attenuation);
        ui->source_attenuation_combo_box->addItem(source_attenuation);
        ui->source_attenuation_combo_box->setCurrentText(source_attenuation);
    }
    if (key->Exists(INPUT_PORT)) {
        QString input_port;
        key->Get(INPUT_PORT, input_port);
        ui->input_port_combo_box->setCurrentText(input_port);
    }
    if (key->Exists(OUTPUT_PORT)) {
        QString output_port;
        key->Get(OUTPUT_PORT, output_port);
        ui->output_port_combo_box->setCurrentText(output_port);
    }
    if (key->Exists(COMPRESSION_POINT)) {
        QString compression_point;
        key->Get(COMPRESSION_POINT, compression_point);
        ui->compression_point_line_edit->setText(compression_point);
    }
    if (key->Exists(POST_CONDITION)) {
        QString post_condition;
        key->Get(POST_CONDITION, post_condition);
        ui->post_condition_combo_box->setCurrentText(post_condition);
    }

    if (key->Exists(OPEN_PATH))
        key->Get(OPEN_PATH, open_path);
    else
        open_path = QDir::homePath();
    if (key->Exists(SAVE_PATH))
        key->Get(SAVE_PATH, save_path);
    else
        save_path = QDir::homePath();
    if (key->Exists(EXPORT_PATH))
        key->Get(EXPORT_PATH, export_path);
    else
        export_path = QDir::homePath();
    if (key->Exists(PRINT_PATH))
        key->Get(PRINT_PATH, print_path);
    else
        print_path = QDir::homePath();
}
void MainWindow::SaveSettings() {
    if (ui->vna_address_line_edit->text().isEmpty() == false)
        key->Set(CONNECTION_ADDRESS,
                 ui->vna_address_line_edit->text());
    // connection type combo box
    key->Set(CONNECTION_TYPE,
             ui->vna_connection_type_combo_box->currentText());
    if (ui->start_frequency_line_edit->hasAcceptableInput())
        key->Set(START_FREQ,
                 ui->start_frequency_line_edit->text());
    key->Set(START_FREQ_UNITS,
             ui->start_freq_units_combo_box->currentText());
    if (ui->stop_frequency_line_edit->hasAcceptableInput())
        key->Set(STOP_FREQ,
                 ui->stop_frequency_line_edit->text());
    key->Set(STOP_FREQ_UNITS,
             ui->stop_freq_units_combo_box->currentText());
    if (ui->frequency_points_line_edit->hasAcceptableInput())
        key->Set(FREQ_POINTS,
                 ui->frequency_points_line_edit->text());
    key->Set(IF_BW,
             ui->if_value_combo_box->currentText());
    key->Set(IF_BW_UNITS,
             ui->if_units_combo_box->currentText());
    if (ui->start_power_line_edit->hasAcceptableInput())
        key->Set(START_POWER,
                 ui->start_power_line_edit->text());
    if (ui->stop_power_line_edit->hasAcceptableInput())
        key->Set(STOP_POWER,
                 ui->stop_power_line_edit->text());
    if (ui->power_points_line_edit->hasAcceptableInput())
        key->Set(POWER_POINTS,
                 ui->power_points_line_edit->text());
    key->Set(RECEIVE_ATTENUATION,
             ui->receiver_attenuation_combo_box->currentText());
    key->Set(SOURCE_ATTENUATION,
             ui->source_attenuation_combo_box->currentText());
    key->Set(INPUT_PORT,
             ui->input_port_combo_box->currentText());
    key->Set(OUTPUT_PORT,
             ui->output_port_combo_box->currentText());
    if (ui->compression_point_line_edit->hasAcceptableInput())
        key->Set(COMPRESSION_POINT,
                 ui->compression_point_line_edit->text());
    key->Set(POST_CONDITION,
             ui->post_condition_combo_box->currentText());

    key->Set(OPEN_PATH, open_path);
    key->Set(SAVE_PATH, save_path);
    key->Set(EXPORT_PATH, export_path);
    key->Set(PRINT_PATH, print_path);
}

// Update GUI
void MainWindow::UpdateStatus() {
    ui->status_bar->clearMessage();
    QString new_message;
    if (vna->isConnected()) {
        new_message = ToString(vna->GetModel()) + " connected. Ready for measurements.";
    }
    else {
        new_message = "No instrument connected";
    }
    ui->status_bar->showMessage(new_message);
}
void MainWindow::UpdateInstrumentInfo() {
    if (vna->isConnected() && vna->GetModel() != UNKNOWN_MODEL) {
        VnaModel model = vna->GetModel();
        ports = vna->GetPorts();
        max_freq_Hz = vna->GetMaximumFrequency_Hz();
        min_freq_Hz = vna->GetMinimumFrequency_Hz();
        max_points = 100001;
        if (model == ZVA_MODEL) {
            min_power_dBm = -150;
            max_power_dBm = 100;
            if_mantissa_Hz.clear();
            if_mantissa_Hz << "1"
                           << "2"
                           << "3"
                           << "4"
                           << "5"
                           << "7"
                           << "10"
                           << "15"
                           << "20"
                           << "30"
                           << "50"
                           << "70"
                           << "100"
                           << "150"
                           << "200"
                           << "300"
                           << "500"
                           << "700";
            if_mantissa_KHz.clear();
            if_mantissa_KHz << "1"
                            << "1.5"
                            << "2"
                            << "3"
                            << "5"
                            << "7"
                            << "10"
                            << "15"
                            << "20"
                            << "30"
                            << "50"
                            << "70"
                            << "100"
                            << "150"
                            << "200"
                            << "300"
                            << "500"
                            << "700";
            if_mantissa_MHz.clear();
            if_mantissa_MHz << "1"
                            << "1.5"
                            << "2"
                            << "3"
                            << "5"
                            << "7"
                            << "10";
            receiver_attenuations.clear();
            receiver_attenuations << "0"
                                  << "10"
                                  << "20"
                                  << "30"
                                  << "40"
                                  << "50"
                                  << "60"
                                  << "70"
                                  << "80";
            source_attenuations.clear();
            source_attenuations << "0"
                                << "10"
                                << "20"
                                << "30"
                                << "40"
                                << "50"
                                << "60"
                                << "70"
                                << "80";
        }
        else {
            min_power_dBm = -40;
            max_power_dBm = 10;
            if_mantissa_Hz.clear();
            if_mantissa_Hz << "1"
                           << "2"
                           << "5"
                           << "10"
                           << "20"
                           << "50"
                           << "100"
                           << "200"
                           << "500";
            if_mantissa_KHz.clear();
            if_mantissa_KHz << "1"
                            << "2"
                            << "5"
                            << "10"
                            << "20"
                            << "50"
                            << "100"
                            << "200"
                            << "500";
            if_mantissa_MHz.clear();
            if_mantissa_MHz << "1"
                            << "2"
                            << "5"
                            << "10"
                            << "20"
                            << "30";
            receiver_attenuations.clear();
            receiver_attenuations << "0"
                                  << "10"
                                  << "20"
                                  << "30";
            source_attenuations.clear();
            source_attenuations << "0";
        }

        ui->make_line_edit->setText("Rohde & Schwarz");
        ui->model_line_edit->setText(ToString(model));
        ui->serial_no_line_edit->setText(vna->GetSerialNumber());
        ui->firmware_version_line_edit->setText(vna->GetFirmwareVersion());
        QString min_freq = FormatValue(min_freq_Hz,
                                       1, // decimal place
                                       HERTZ_UNITS);
        QString max_freq = FormatValue(max_freq_Hz,
                                       1, // decimal place
                                       HERTZ_UNITS);
        ui->frequency_range_line_edit->setText(min_freq + " to " + max_freq);
        QString min_power = FormatValue(min_power_dBm,
                                        1, // decimal place
                                        DECIBEL_MILLIWATTS_UNITS);
        QString max_power = FormatValue(max_power_dBm,
                                        1, // decimal place
                                        DECIBEL_MILLIWATTS_UNITS);
        ui->power_range_line_edit->setText(min_power + " to " + max_power);
        if (model == ZVA_MODEL) {
            ui->source_attenuators_line_edit->setText("80 dB in 10 dB steps");
            ui->receiver_attenuators_line_edit->setText("80 dB in 10 dB steps");
        }
        else {
            ui->source_attenuators_line_edit->setText("None");
            ui->receiver_attenuators_line_edit->setText("30 dB in 10 dB steps");
        }
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
        ui->input_port_combo_box->addItem(QVariant(i).toString());
        ui->output_port_combo_box->addItem(QVariant(i).toString());
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
    SiPrefix prefix = String_To_SiPrefix(units);
    int decimal_places;
    if (prefix == NO_PREFIX)
        decimal_places = 0;
    else
        decimal_places = 3;
    double_validator.reset(new QDoubleValidator(min_freq_Hz/ToDouble(prefix),
                                                max_freq_Hz/ToDouble(prefix),
                                                decimal_places,
                                                ui->start_frequency_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->start_frequency_line_edit->setValidator(double_validator.take());
    units = ui->stop_freq_units_combo_box->currentText();
    units.chop(2);
    prefix = String_To_SiPrefix(units);
    if (prefix == NO_PREFIX)
        decimal_places = 0;
    else
        decimal_places = 3;
    double_validator.reset(new QDoubleValidator(min_freq_Hz/ToDouble(prefix),
                                                max_freq_Hz/ToDouble(prefix),
                                                decimal_places,
                                                ui->stop_frequency_line_edit));
    double_validator->setNotation(QDoubleValidator::StandardNotation);
    ui->stop_frequency_line_edit->setValidator(double_validator.take());
}
void MainWindow::UpdateIfBwValues() {
    QString if_units = ui->if_units_combo_box->currentText();
    QString if_value = ui->if_value_combo_box->currentText();
    ui->if_value_combo_box->clear();
    if (if_units == "Hz") {
        ui->if_value_combo_box->addItems(if_mantissa_Hz);
    }
    else if (if_units == "KHz") {
        ui->if_value_combo_box->addItems(if_mantissa_KHz);
    }
    else {
        ui->if_value_combo_box->addItems(if_mantissa_MHz);
    }
    int index = ui->if_value_combo_box->findText(if_value);
    if (index != -1)
        ui->if_value_combo_box->setCurrentIndex(index);
    else
        ui->if_value_combo_box->setCurrentIndex(0);
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
    ui->measure_push_button->setEnabled(enabled);
}
void MainWindow::TogglePlots(bool enabled) {
    ui->plot_type_combo_box->setEnabled(enabled);
    ui->print_plot_push_button->setEnabled(enabled);
    ui->actionExport->setEnabled(enabled);
    ui->actionSave->setEnabled(enabled);
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
    start_freq = QVariant(ui->start_frequency_line_edit->text()).toDouble();
    start_freq_units = ui->start_freq_units_combo_box->currentText();
    start_freq_units.chop(2); // chop off "Hz"
    start_prefix = String_To_SiPrefix(start_freq_units);
    stop_freq = QVariant(ui->stop_frequency_line_edit->text()).toDouble();
    stop_freq_units = ui->stop_freq_units_combo_box->currentText();
    stop_freq_units.chop(2); // chop off "Hz"
    stop_prefix = String_To_SiPrefix(stop_freq_units);
    if (start_freq * ToDouble(start_prefix) >= stop_freq * ToDouble(stop_prefix)) {
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
    compression_point_dB = QVariant(ui->compression_point_line_edit->text()).toDouble();
    output_port = ui->input_port_combo_box->currentIndex() + 1;
    input_port = ui->output_port_combo_box->currentIndex() + 1;
    start_power_dBm = QVariant(ui->start_power_line_edit->text()).toDouble();
    stop_power_dBm = QVariant(ui->stop_power_line_edit->text()).toDouble();
    number_power_points = QVariant(ui->power_points_line_edit->text()).toInt();

    const double start_freq = QVariant(ui->start_frequency_line_edit->text()).toDouble();
    QString start_freq_prefix = ui->start_freq_units_combo_box->currentText();
    start_freq_prefix.chop(2);
    const SiPrefix start_freq_SiPrefix = String_To_SiPrefix(start_freq_prefix);
    start_freq_Hz = start_freq * ToDouble(start_freq_SiPrefix);

    const double stop_freq = QVariant(ui->stop_frequency_line_edit->text()).toDouble();
    QString stop_freq_prefix = ui->stop_freq_units_combo_box->currentText();
    stop_freq_prefix.chop(2);
    const SiPrefix stop_freq_SiPrefix = String_To_SiPrefix(stop_freq_prefix);
    stop_freq_Hz = stop_freq * ToDouble(stop_freq_SiPrefix);

    number_frequency_points = QVariant(ui->frequency_points_line_edit->text()).toInt();

    const double if_bw = QVariant(ui->if_value_combo_box->currentText()).toDouble();
    QString if_bw_prefix = ui->if_units_combo_box->currentText();
    if_bw_prefix.chop(2);
    SiPrefix if_bw_SiPrefix = String_To_SiPrefix(if_bw_prefix);
    if_bw_Hz = if_bw * ToDouble(if_bw_SiPrefix);

    source_attenuation = QVariant(ui->source_attenuation_combo_box->currentText()).toInt();
    receiver_attenuation = QVariant(ui->receiver_attenuation_combo_box->currentText()).toInt();

}
void MainWindow::Progress(int percent) {
    QString message = "Measurement sweeps " + QVariant(percent).toString() + "% complete";
    ui->status_bar->showMessage(message);
}
void MainWindow::Finished() {
    run_sweeps->disconnect();
    vna->Channel().EnableContinuousSweep();
    ToggleConnect(true);
    CalculateReflectionMags();
    CalculateGain();
    FindNominalGain();
    // FindCompressionPoints();
    ToggleInputs(true);
    UpdateStatus();

    QString post_condition = ui->post_condition_combo_box->currentText();
    if (post_condition == "RF Off")
        vna->DisableRfOutputPower();
    else if (post_condition == "Minimum Power")
        vna->Channel().SetChannelPower_dBm(start_power_dBm);
    else if (post_condition == "Maximum Power")
        vna->Channel().SetChannelPower_dBm(stop_power_dBm);

    TogglePlots(true);
    ui->frequency_slider->setMaximum(number_frequency_points-1);
    ui->frequency_slider->setValue(1);
    ui->frequency_slider->setValue(0);
    ui->plot_type_combo_box->setEnabled(true);
    ui->plot_type_combo_box->setCurrentIndex(1);
    ui->plot_type_combo_box->setCurrentIndex(0);
}

// Calculate
void MainWindow::CalculateReflectionMags() {
    RowVector std_row_vector;
    s_parameter_data[0].GetDb(1, 1, std_row_vector);
    s11_dB = QRowVector::fromStdVector(std_row_vector);
    s_parameter_data[0].GetDb(2, 2, std_row_vector);
    s22_dB = QRowVector::fromStdVector(std_row_vector);
}
void MainWindow::CalculateGain() {
    gain_data_dB.clear();
    gain_data_dB.resize(number_frequency_points);
    for (int freq_index = 0; freq_index < number_frequency_points; freq_index++) {
        gain_data_dB[freq_index].resize(number_power_points);
        for (int power_index = 0; power_index < number_power_points; power_index++) {
            gain_data_dB[freq_index][power_index] = power_sweeps_dBm[freq_index][power_index] - power_points_dBm[power_index];
        }
    }
}
void MainWindow::FindNominalGain() {
    nominal_gain_dB.clear();
    nominal_gain_dB.resize(number_frequency_points);
    for (int i = 0; i < number_frequency_points; i++) {
        nominal_gain_dB[i] = gain_data_dB[i][0];
    }
}
void MainWindow::FindCompressionPoints() {
//    int number_power_points = this->power_points_dBm.size();
//    int freq_points = this->frequency_points_Hz.size();
//    compression_points_in_dBm.clear();
//    compression_points_in_dBm.resize(freq_points);
//    for (int i = 0; i < freq_points; i++) {
//        int power_index = number_power_points - 1;
//        double nominal_gain = nominal_gain_dB[i];
//        while (power_index > 0 && gain_data_dB[i][power_index] < nominal_gain - compression_point_dB) {
//            power_index--;
//        }
//        if (power_index == number_power_points - 1)
//            compression_points_in_dBm[i] = power_points_dBm[number_power_points-1];
//        else if (gain_data_dB[i][power_index] == nominal_gain_dB[i] - compression_point_dB)
//            compression_points_in_dBm[i] = power_points_dBm[power_index];
//        else // compression point found inbetween data points
//            compression_points_in_dBm[i] = LinearInterpolateX(power_points_dBm[power_index],
//                                                           gain_data_dB[i][power_index],
//                                                           power_points_dBm[power_index+1],
//                    gain_data_dB[i][power_index+1],
//                    nominal_gain - compression_point_dB);
//    }
}

// Plot
void MainWindow::PlotReflection() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle("Reflection Coefficients");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->legend->setPositionStyle(QCPLegend::psRight);

    // S11
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Input");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(0)->setData(frequency_points_Hz, s11_dB);

    // S22
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Output");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(1)->setData(frequency_points_Hz, s22_dB);

    ui->custom_plot->setupFullAxesBox();
    ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (Hz)");
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Reflection Coefficient (dB)");

    ui->custom_plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteraction(QCustomPlot::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotPinVsPout() {
    int frequency_index = ui->frequency_slider->value();

    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle("Power Sweep: Pin vs Pout");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->legend->setPositionStyle(QCPLegend::psRight);

    // Pin vs Pout
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Measured");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(power_points_dBm, power_sweeps_dBm[frequency_index]);

    // Ideal Pin vs Pout (no compression)
    QVector<double> ideal_Pout;
    ideal_Pout.resize(number_power_points);
    for (int i = 0; i < number_power_points; i++)
        ideal_Pout[i] = power_points_dBm[i] + nominal_gain_dB[frequency_index];
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Ideal");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(1)->setData(power_points_dBm, ideal_Pout);

    ui->custom_plot->setupFullAxesBox();
    ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Pin (dBm)");
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Pout (dBm)");

    ui->custom_plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteraction(QCustomPlot::iSelectPlottables);

    //    QScopedPointer<QCPItemText> text(new QCPItemText(ui->custom_plot));
    //    text->setPen(QPen(Qt::black));
    //    text->setFont(QFont("Helvetica", 9));
    //    text->setText("Frequency:");
    //    text->setPositionAlignment(Qt::AlignCenter);
    //    text->position->setCoords(QPointF(1,2));
    //    ui->custom_plot->addItem(text.take());

    ui->custom_plot->replot();
}
void MainWindow::PlotGainVsFreq() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle("Nominal Gain vs Frequency");

    ui->custom_plot->legend->setVisible(false);

    // Nominal gain
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(frequency_points_Hz, nominal_gain_dB);

    ui->custom_plot->setupFullAxesBox();
    ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (Hz)");
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Nominal Gain (dB)");

    ui->custom_plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteraction(QCustomPlot::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotGainVsPin() {
    int freq_index = ui->frequency_slider->value();

    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle("Gain vs Pin");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->legend->setPositionStyle(QCPLegend::psRight);

    // Nominal gain
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Gain vs Pin");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(power_points_dBm, gain_data_dB[freq_index]);

    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Compression Point");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::red));
    QRowVector compression_points;
    compression_points.fill(nominal_gain_dB[freq_index] - compression_point_dB, number_power_points);
    ui->custom_plot->graph(1)->setData(power_points_dBm, compression_points);

    ui->custom_plot->setupFullAxesBox();
    ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Pin (dBm)");
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Gain (dB)");

    ui->custom_plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteraction(QCustomPlot::iSelectPlottables);

    ui->custom_plot->replot();
}
void MainWindow::PlotCompressionPoint() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle(
                FormatValue(compression_point_dB, 1, DECIBELS_UNITS)
                + " Compression Point vs Freq");

    ui->custom_plot->legend->setVisible(true);
    ui->custom_plot->legend->setFont(QFont("Helvetica", 9));
    ui->custom_plot->legend->setPositionStyle(QCPLegend::psRight);

    // Pin compression points
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setName("Pin");
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(frequency_points_Hz, compression_points_in_dBm);

    // Pout
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(1)->setName("Pout");
    ui->custom_plot->graph(1)->setPen(QPen(Qt::red));
    ui->custom_plot->graph(1)->setData(frequency_points_Hz, compression_points_out_dBm);


    ui->custom_plot->setupFullAxesBox();
    ui->custom_plot->rescaleAxes();
    ui->custom_plot->xAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->xAxis->setLabel("Frequency (Hz)");
    ui->custom_plot->yAxis->setLabelFont(QFont("Helvetica",9));
    ui->custom_plot->yAxis->setLabel("Compression Point (dBm)");

    ui->custom_plot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->custom_plot->setInteraction(QCustomPlot::iSelectPlottables);

    ui->custom_plot->replot();
}

// Export Data
void MainWindow::Export() {
    QString path = QFileDialog::getSaveFileName(this, "Export folder", export_path, "Folder name (*)");
    if (path.isEmpty())
        return;

    QString touchstone_folder = "Touchstone Data";
    QString power_sweep_folder = "Power Sweep Data";
    QString gain_sweep_folder = "Gain Sweep Data";
    QString nominal_gain_filename = "Nominal Gain.csv";
    QString compression_points_filename = "Compression points.csv";
    QFileInfo file_info(path);
    QDir q_dir(file_info.absolutePath());
    if (q_dir.mkdir(file_info.fileName())) {
        // Touchstone
        q_dir.cd(file_info.fileName());
        q_dir.mkdir(touchstone_folder);
        q_dir.cd(touchstone_folder);
        ExportTouchstone(q_dir.absolutePath());

        // Power Sweeps
        q_dir.cdUp();
        q_dir.mkdir(power_sweep_folder);
        q_dir.cd(power_sweep_folder);
        ExportPowerSweeps(q_dir.absolutePath());

        // Gain Sweeps
        q_dir.cdUp();
        q_dir.mkdir(gain_sweep_folder);
        q_dir.cd(gain_sweep_folder);
        ExportGainSweeps(q_dir.absolutePath());

        // Nominal Gain
        q_dir.cdUp();
        nominal_gain_filename = q_dir.absolutePath()
                + "/" + nominal_gain_filename;
        ExportNominalGain(nominal_gain_filename);

        // Compression Point
        compression_points_filename = q_dir.absolutePath()
                + "/" + compression_points_filename;
        ExportCompressionPoints(compression_points_filename);

        export_path = path;
    }
    else {
        QMessageBox::warning(this,
                             APP_NAME,
                             "Export failed: could not write to directory");
    }
}
void MainWindow::ExportTouchstone(QString path) {
    for (int i = 0; i < number_power_points; i++) {
        QString filename = path + "/"
                + "Pin " + QVariant(power_points_dBm[i]).toString() + " dBm.s2p";
        Touchstone::Write(s_parameter_data[i], filename);
    }
}
void MainWindow::ExportPowerSweeps(QString path) {
    for (int i = 0; i < number_frequency_points; i++) {
        QString filename = path + "/"
                + FormatValue(frequency_points_Hz[i], 3, HERTZ_UNITS)
                + ".csv";
        ExportPowerSweep(filename, i);
    }
}
void MainWindow::ExportPowerSweep(QString filename, int freq_index) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! (Pin, Pout) in dBm" << endl;
    stream << "! @ "
           << FormatValue(frequency_points_Hz[freq_index], 3, HERTZ_UNITS) << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    for (int i = 0; i < number_power_points; i++) {
        stream.setFieldWidth(8);
        stream.setRealNumberPrecision(4);
        stream << power_points_dBm[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(14);
        stream << power_sweeps_dBm[freq_index][i]
                  << endl;
    }
    stream.flush();
    file.close();
}
void MainWindow::ExportGainSweeps(QString path) {
    for (int i = 0; i < number_frequency_points; i++) {
        QString filename = path + "/"
                + FormatValue(frequency_points_Hz[i], 3, HERTZ_UNITS)
                + ".csv";
        ExportPowerSweep(filename, i);
    }
}
void MainWindow::ExportGainSweep(QString filename, int freq_index) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! (Pin, Gain) in dBm, dB" << endl;
    stream << "! @ "
           << FormatValue(frequency_points_Hz[freq_index], 3, HERTZ_UNITS) << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    for (int i = 0; i < number_power_points; i++) {
        stream.setFieldWidth(8);
        stream.setRealNumberPrecision(4);
        stream << power_points_dBm[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(14);
        stream << gain_data_dB[freq_index][i]
                  << endl;
    }
    stream.flush();
    file.close();
}
void MainWindow::ExportNominalGain(QString filename) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! (frequency, Nominal Gain) in Hz, dB" << endl;
    stream << "! @ Pin = "
           << FormatValue(power_points_dBm[0], 2, DECIBEL_MILLIWATTS_UNITS)
            << " (minimum power)" << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    for (int i = 0; i < number_frequency_points; i++) {
        stream.setFieldWidth(18);
        stream.setRealNumberPrecision(18);
        stream << frequency_points_Hz[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(5);
        stream << nominal_gain_dB[i]
                  << endl;
    }
    stream.flush();
    file.close();
}
void MainWindow::ExportCompressionPoints(QString filename) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! Compression points" << endl;
    stream << "! (frequency, Pin, Pout) in Hz, dBm, dBm" << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    for (int i = 0; i < number_frequency_points; i++) {
        stream.setFieldWidth(18);
        stream.setRealNumberPrecision(18);
        stream << frequency_points_Hz[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setFieldWidth(12);
        stream.setRealNumberPrecision(6);
        stream << compression_points_in_dBm[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(6);
        stream << compression_points_out_dBm[i]
                  << endl;
    }
    stream.flush();
    file.close();
}


// Open, Save
void MainWindow::Open() {
    QString path = QFileDialog::getOpenFileName(this, "Open file", open_path, "PA Compression Test (*.amp)");
    if (path.isEmpty())
        return;

    QFile input_file(path);
    input_file.open(QFile::ReadOnly);
    if (input_file.isReadable() == false) {
        QFileInfo file_info(path);
        QMessageBox::warning(this,
                             "RSA PA Compression Test",
                             QString() + "Could not open file " + file_info.fileName()
                             + " in " + QDir::toNativeSeparators(file_info.absolutePath()));
        return;
    }
    QDataStream input_stream(&input_file);
    if (Open(input_stream) == false) {
        TogglePlots(false);
        QFileInfo file_info(path);
        QMessageBox::warning(this,
                             "RSA PA Compression Test",
                             QString() + "Could not open file " + file_info.fileName()
                             + " in " + QDir::toNativeSeparators(file_info.absolutePath()));
        return;
    }

    open_path = path;
    TogglePlots(true);
    ui->plot_type_combo_box->setCurrentIndex(1);
    ui->plot_type_combo_box->setCurrentIndex(0);
}
bool MainWindow::Open(QDataStream &input) {
    QString app_name, app_version;

    try {
        input >> app_name
              >> app_version
              >> time_stamp
              >> make
              >> model
              >> serial_no
              >> firmware_version
              >> if_bw_Hz
              >> number_frequency_points
              >> number_power_points
              >> frequency_points_Hz
              >> power_points_dBm
              >> s11_dB
              >> s22_dB
              >> power_sweeps_dBm
              >> gain_data_dB
              >> nominal_gain_dB
              >> compression_points_in_dBm
              >> compression_points_out_dBm;
        s_parameter_data.resize(number_power_points);
        for (int i = 0; i < number_power_points; i++) {
            input >> s_parameter_data[i];
        }
    }
    catch(void *) {
        return(false);
    }

    // else
    return(true);
}
void MainWindow::Save() {
    QString path = QFileDialog::getSaveFileName(this, "Save as", save_path, "PA Compression Test (*.amp)");
    if (path.isEmpty())
        return;

    QFile output_file(path);
    output_file.open(QFile::WriteOnly);
    if (output_file.isWritable() == false) {
        QFileInfo file_info(path);
        QMessageBox::warning(this,
                             "RSA PA Compression Test",
                             QString() + "Could not create file " + file_info.fileName()
                             + " in " + file_info.absolutePath());
        return;
    }
    QDataStream output_stream(&output_file);
    Save(output_stream);
    save_path = path;
}
bool MainWindow::Save(QDataStream &output) {
    try {
        output << QString(APP_NAME)
               << QString(APP_VERSION)
               << time_stamp
               << make
               << model
               << serial_no
               << firmware_version
               << if_bw_Hz
               << number_frequency_points
               << number_power_points
               << frequency_points_Hz
               << power_points_dBm
               << s11_dB
               << s22_dB
               << power_sweeps_dBm
               << gain_data_dB
               << nominal_gain_dB
               << compression_points_in_dBm
               << compression_points_out_dBm;
        for (int i = 0; i < number_power_points; i++) {
            output << s_parameter_data[i];
        }
    }
    catch(void *) {
        return(false);
    }

    // else
    return(true);
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
        vna.reset(new Vna(connection_type,
                          address,
                          TIMEOUT_MS,
                          LOG_PATH,
                          LOG_FILENAME,
                          APP_NAME,
                          APP_VERSION));
        if (vna->isConnected()) {
            ui->vna_connect_push_button->setText("Disconnect");
            vna->Lock();
            vna->Local();
            UpdateStatus();
            UpdateInstrumentInfo();
            UpdateValidators();
            ToggleInputs(true);
        }
        else {
            QString warning = QString()
                    + "Could not connect to VNA at "
                    + ui->vna_connection_type_combo_box->currentText()
                    + " address " + address
                    + "\nPlease try again";
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
void MainWindow::on_measure_push_button_clicked()
{
    if (isValidInput() == false)
        return;

    ToggleInputs(false);
    TogglePlots(false);
    ToggleConnect(false);
    GetInput();

    uint power_channel = 1;
    uint freq_channel = 2;
    vna->DisableUserCalPreset();
    vna->DisableUserPreset();
    vna->Preset();
    vna->ClearErrors();

    // Power sweep
    // vna->CreateChannel(power_channel);
    vna->Channel(power_channel).SetSweepType(POWER_SWEEP);
    vna->Trace().SetParameters(S_PARAMETER, output_port, input_port);
    vna->Trace().SetFormat(DB_MAGNITUDE_TRACE);
    vna->Channel(power_channel).SetStartPower(start_power_dBm);
    vna->Channel(power_channel).SetStopPower(stop_power_dBm);
    vna->Channel(power_channel).SetPoints(number_power_points);
    vna->Channel(power_channel).SetIfBandwidth(if_bw_Hz);
    if (source_attenuations.size() != 1)
        vna->Channel(power_channel).SetSourceAttenuations(source_attenuation);
    vna->Channel(power_channel).SetReceiverAttenuations(receiver_attenuation);
    vna->Channel(power_channel).SetCompressionLevel(compression_point_dB);
    vna->Channel(power_channel).EnableCompressionCalc();
    vna->EnableRfOutputPower();
    vna->Channel(power_channel).DisableContinuousSweep();
    vna->Channel(power_channel).GetStimulusValues(power_points_dBm);

    // Frequency sweep (full two-port)
    vna->CreateChannel(freq_channel);
    vna->Channel(freq_channel).SetSweepType(LINEAR_FREQUENCY_SWEEP);
    vna->Channel(freq_channel).SetStartFrequency(start_freq_Hz);
    vna->Channel(freq_channel).SetStopFrequency(stop_freq_Hz);
    vna->Channel(freq_channel).SetPoints(number_frequency_points);
    vna->Channel(freq_channel).SetIfBandwidth(if_bw_Hz);
    if (source_attenuations.size() != 1)
        vna->Channel(freq_channel).SetSourceAttenuations(source_attenuation);
    vna->Channel(freq_channel).SetReceiverAttenuations(receiver_attenuation);

    QVector<uint> ports;
    ports << input_port << output_port;
    vna->Channel(freq_channel).CreateSParameterGroup(ports);
    vna->Channel(freq_channel).DisableContinuousSweep();
    vna->Channel(freq_channel).GetStimulusValues(frequency_points_Hz);

    // Start measurement
    run_sweeps.reset(new RunSweeps(*vna.data(),
                                   power_channel,
                                   freq_channel,
                                   ports,
                                   frequency_points_Hz,
                                   power_points_dBm,
                                   power_sweeps_dBm,
                                   compression_points_in_dBm,
                                   compression_points_out_dBm,
                                   s_parameter_data));
    QObject::connect(run_sweeps.data(), SIGNAL(finished()),
                     this, SLOT(Finished()));
    QObject::connect(run_sweeps.data(), SIGNAL(Progress(int)),
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
    else { // Compression point plot
        PlotCompressionPoint();
    }
}
void MainWindow::on_frequency_slider_valueChanged(int value)
{
    ui->frequency_slider_label->setText(FormatValue(frequency_points_Hz[value], 4, HERTZ_UNITS));
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
        print_path = save_file_name;
}
