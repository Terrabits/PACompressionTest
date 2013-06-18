

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Rsa
using namespace RsaToolbox;

// Qt
#include <QMessageBox>
#include <QVariant>
#include <QIntValidator>
#include <QDoubleValidator>


MainWindow::MainWindow(Key &key, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    
    ui->setupUi(this);
    ui->frequency_slider_label->setVisible(false);
    this->setWindowTitle(QString() +
                         APP_NAME + " Version " + APP_VERSION);

    vna.reset(new Vna());
    this->key = &key;
    UpdateStatus();
    UpdateInstrumentInfo();
    ToggleInputs(false);

    sweep_data_dBm.clear();
    frequency_points_Hz.clear();
    power_points_dBm.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}


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
    int_validator.reset(new QIntValidator(0, max_points, ui->frequency_points_line_edit));
    ui->frequency_points_line_edit->setValidator(int_validator.take());
    int_validator.reset(new QIntValidator(0, max_points, ui->power_points_line_edit));
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
    ui->receiver_attenuation_combo_box->clear();
    ui->receiver_attenuation_combo_box->addItems(receiver_attenuations);
    ui->source_attenuation_combo_box->clear();
    ui->source_attenuation_combo_box->addItems(source_attenuations);

    // Ports
    ui->input_port_combo_box->clear();
    ui->output_port_combo_box->clear();
    for (int i = 1; i <= ports; i++) {
        ui->input_port_combo_box->addItem(QVariant(i).toString());
        ui->output_port_combo_box->addItem(QVariant(i).toString());
    }
    ui->input_port_combo_box->setCurrentText("1");
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
    //else
    return(true);
}

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

    this->compression_point_dB = QVariant(ui->compression_point_line_edit->text()).toDouble();
    const int output_port = ui->input_port_combo_box->currentIndex() + 1;
    const int input_port = ui->output_port_combo_box->currentIndex() + 1;
    const double start_power = QVariant(ui->start_power_line_edit->text()).toDouble();
    const double stop_power = QVariant(ui->stop_power_line_edit->text()).toDouble();
    number_power_points = QVariant(ui->power_points_line_edit->text()).toInt();

    const double start_freq = QVariant(ui->start_frequency_line_edit->text()).toDouble();
    QString start_freq_units_string = ui->start_freq_units_combo_box->currentText();
    start_freq_units_string.chop(2);
    const SiPrefix start_freq_prefix = String_To_SiPrefix(start_freq_units_string);

    const double stop_freq = QVariant(ui->stop_frequency_line_edit->text()).toDouble();
    QString stop_freq_units_string = ui->stop_freq_units_combo_box->currentText();
    stop_freq_units_string.chop(2);
    const SiPrefix stop_freq_prefix = String_To_SiPrefix(stop_freq_units_string);

    number_frequency_points = QVariant(ui->frequency_points_line_edit->text()).toInt();

    const double if_bw = QVariant(ui->if_value_combo_box->currentText()).toDouble();
    QString if_bw_units_string = ui->if_units_combo_box->currentText();
    if_bw_units_string.chop(2);
    SiPrefix if_bw_prefix = String_To_SiPrefix(if_bw_units_string);

    const int source_atten = QVariant(ui->source_attenuation_combo_box->currentText()).toInt();
    const int receiver_atten = QVariant(ui->receiver_attenuation_combo_box->currentText()).toInt();

    vna->Preset();
    vna->Trace().SetParameters(RsaToolbox::S_PARAMETER, output_port, input_port);
    vna->Channel().SetSweepType(RsaToolbox::POWER_SWEEP);
    vna->Channel().SetStartPower(start_power);
    vna->Channel().SetStopPower(stop_power);
    vna->Channel().SetPoints(number_power_points);
    vna->Channel().SetIfBandwidth(if_bw, if_bw_prefix);
    if (source_attenuations.size() != 1)
        vna->Channel().SetSourceAttenuations_dB(source_atten);
    vna->Channel().SetReceiverAttenuations_dB(receiver_atten);

    RowVector std_row_vector;
    vna->Channel().GetStimulusValues(std_row_vector);
    power_points_dBm = QRowVector::fromStdVector(std_row_vector);
    LinearSpacing(frequency_points_Hz,
                  start_freq * ToDouble(start_freq_prefix),
                  stop_freq * ToDouble(stop_freq_prefix),
                  number_frequency_points);

    run_sweeps.reset(new RunSweeps(*vna.data(), frequency_points_Hz, sweep_data_dBm));
    QObject::connect(run_sweeps.data(), SIGNAL(finished()),
                     this, SLOT(Finished()));
    QObject::connect(run_sweeps.data(), SIGNAL(Progress(int)),
                     this, SLOT(Progress(int)));
    vna->moveToThread(run_sweeps.data());
    run_sweeps->start();
}

void MainWindow::Progress(int percent) {
    QString message = "Measurement sweeps " + QVariant(percent).toString() + "% complete";
    ui->status_bar->showMessage(message);
}

void MainWindow::Finished() {
    run_sweeps->disconnect();
    ToggleConnect(true);
    CalculateGain();
    FindNominalGain();
    FindCompressionPoints();
    ToggleInputs(true);
    UpdateStatus();

    TogglePlots(true);
    ui->frequency_slider->setMaximum(number_frequency_points-1);
    ui->frequency_slider->setValue(1);
    ui->frequency_slider->setValue(0);
    ui->plot_type_combo_box->setEnabled(true);
    ui->plot_type_combo_box->setCurrentIndex(1);
    ui->plot_type_combo_box->setCurrentIndex(0);
}

void MainWindow::TogglePlots(bool enabled) {
    ui->plot_type_combo_box->setEnabled(enabled);
    if (enabled == false) {
        ui->frequency_slider->setEnabled(false);
        ui->frequency_slider_label->setVisible(false);
    }
}

void MainWindow::ToggleConnect(bool enabled) {
    ui->vna_address_line_edit->setEnabled(enabled);
    ui->vna_connection_type_combo_box->setEnabled(enabled);
    ui->vna_connect_push_button->setEnabled(enabled);
}

void MainWindow::CalculateGain() {
    gain_data_dB.clear();
    gain_data_dB.resize(number_frequency_points);
    for (int freq_index = 0; freq_index < number_frequency_points; freq_index++) {
        gain_data_dB[freq_index].resize(number_power_points);
        for (int power_index = 0; power_index < number_power_points; power_index++) {
            gain_data_dB[freq_index][power_index] = sweep_data_dBm[freq_index][power_index] - power_points_dBm[power_index];
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
    int number_power_points = this->power_points_dBm.size();
    int freq_points = this->frequency_points_Hz.size();
    compression_points_dBm.clear();
    compression_points_dBm.resize(freq_points);
    for (int i = 0; i < freq_points; i++) {
        int power_index = number_power_points - 1;
        double nominal_gain = nominal_gain_dB[i];
        while (power_index > 0 && gain_data_dB[i][power_index] < nominal_gain - compression_point_dB) {
            power_index--;
        }
        if (power_index == number_power_points - 1)
            compression_points_dBm[i] = power_points_dBm[number_power_points-1];
        else if (gain_data_dB[i][power_index] == nominal_gain_dB[i] - compression_point_dB)
            compression_points_dBm[i] = power_points_dBm[power_index];
        else // compression point found inbetween data points
            compression_points_dBm[i] = LinearInterpolate(power_points_dBm[power_index],
                                                      gain_data_dB[i][power_index],
                                                      power_points_dBm[power_index+1],
                                                      gain_data_dB[i][power_index+1],
                                                      nominal_gain - compression_point_dB);
    }
}

double MainWindow::LinearInterpolate(double x1, double y1, double x2, double y2, double y_desired) {
    double slope = (y2 - y1)/(x2 - x1);
    // y_desired = y1 + slope*(x_desired - x1)
    // ... (y_desired - y1) / slope + x1 = x_desired;
    return((y_desired - y1)/slope + x1);
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
    ui->custom_plot->graph(0)->setData(power_points_dBm, sweep_data_dBm[frequency_index]);

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

    ui->custom_plot->legend->setVisible(false);

    // Nominal gain
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(power_points_dBm, gain_data_dB[freq_index]);

    ui->custom_plot->addGraph();
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

void MainWindow::CompressionPointVsFreq() {
    ui->custom_plot->clearGraphs();
    ui->custom_plot->clearItems();
    ui->custom_plot->legend->clearItems();

    ui->custom_plot->setTitleFont(QFont("Helvetica", 12));
    ui->custom_plot->setTitle("Compression Point vs Freq");

    ui->custom_plot->legend->setVisible(false);

    // Nominal gain
    ui->custom_plot->addGraph();
    ui->custom_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->custom_plot->graph(0)->setData(frequency_points_Hz, compression_points_dBm);

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

void MainWindow::on_plot_type_combo_box_currentIndexChanged(const QString &arg1)
{
    QObject::disconnect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                        this, SLOT(PlotGainVsPin()));
    QObject::disconnect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                        this, SLOT(PlotPinVsPout()));

    if (arg1 == "Pin vs Pout") {
        ui->frequency_slider->setEnabled(true);
        ui->frequency_slider_label->setVisible(true);
        QObject::connect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                         this, SLOT(PlotPinVsPout()));
        PlotPinVsPout();
    }
    else if (arg1 == "Nominal Gain") {
        ui->frequency_slider->setDisabled(true);
        ui->frequency_slider_label->setVisible(false);
        PlotGainVsFreq();
    }
    else if (arg1 == "Gain vs Power") {
        ui->frequency_slider->setEnabled(true);
        ui->frequency_slider_label->setVisible(true);
        QObject::connect(ui->frequency_slider, SIGNAL(valueChanged(int)),
                         this, SLOT(PlotGainVsPin()));
        PlotGainVsPin();
    }
    else { // Compression point plot
        ui->frequency_slider->setDisabled(true);
        ui->frequency_slider_label->setVisible(false);
        CompressionPointVsFreq();
    }
}

void MainWindow::on_frequency_slider_valueChanged(int value)
{
    ui->frequency_slider_label->setText(FormatValue(frequency_points_Hz[value], 4, HERTZ_UNITS));
}
