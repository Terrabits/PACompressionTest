#include "FrequencyEdit.h"
#include "ui_FrequencyEdit.h"


// RsaToolbox
#include "Definitions.h"
#include "General.h"
using namespace RsaToolbox;

// Qt
#include <QDebug>


FrequencyEdit::FrequencyEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyEdit)
{
    ui->setupUi(this);

    QRegExp regex("\\d+(?:\\.\\d*)?(?:[gmk])?", Qt::CaseInsensitive);
    QRegExpValidator *validator = new QRegExpValidator(regex, this);
    ui->frequency->setValidator(validator);

    connect(ui->frequency, SIGNAL(textChanged(QString)),
            this, SLOT(processChanges()));
    connect(ui->prefix, SIGNAL(currentIndexChanged(int)),
            this, SLOT(processChanges()));
}

FrequencyEdit::~FrequencyEdit()
{
    delete ui;
}

bool FrequencyEdit::isEmpty() const {
    return ui->frequency->text().isEmpty();
}
bool FrequencyEdit::hasAcceptableInput() const {
    return ui->frequency->hasAcceptableInput();
}
double FrequencyEdit::frequency_Hz() const {
    return _frequency_Hz;
}

void FrequencyEdit::setFocus() {
    ui->frequency->setFocus();
}
void FrequencyEdit::selectAll() {
    ui->frequency->selectAll();
}
void FrequencyEdit::setFrequency(double frequency_Hz) {
    if (frequency_Hz < 0)
        return;
    if (frequency_Hz == _frequency_Hz)
        return;

    bool isFrequencyBlocked = ui->frequency->blockSignals(true);
    bool isPrefixBlocked = ui->prefix->blockSignals(true);

    _frequency_Hz = frequency_Hz;
    SiPrefix prefix = getPrefix(frequency_Hz);
    ui->frequency->setText(QVariant(_frequency_Hz/toDouble(prefix)).toString());
    ui->prefix->setCurrentText(toString(prefix, Units::Hertz));

    ui->frequency->blockSignals(isFrequencyBlocked);
    ui->prefix->blockSignals(isPrefixBlocked);

    emit frequencyChanged(_frequency_Hz);
}

void FrequencyEdit::processChanges() {
    bool isFrequencyBlocked = ui->frequency->blockSignals(true);
    bool isPrefixBlocked = ui->prefix->blockSignals(true);

    QString text = ui->frequency->text();
    if (text.contains("g", Qt::CaseInsensitive)) {
        text.remove("g", Qt::CaseInsensitive);
        ui->frequency->setText(text);
        ui->prefix->setCurrentText("GHz");
    }
    else if (text.contains("m", Qt::CaseInsensitive)) {
        text.remove("m", Qt::CaseInsensitive);
        ui->frequency->setText(text);
        ui->prefix->setCurrentText("MHz");
    }
    else if (text.contains("k", Qt::CaseInsensitive)) {
        text.remove("k", Qt::CaseInsensitive);
        ui->frequency->setText(text);
        ui->prefix->setCurrentText("KHz");
    }

    ui->frequency->blockSignals(isFrequencyBlocked);
    ui->prefix->blockSignals(isPrefixBlocked);

    double frequency = text.toDouble();
    if (ui->prefix->currentText() == "GHz")
        frequency *= 1.0E9;
    else if (ui->prefix->currentText() == "MHz")
        frequency *= 1.0E6;
    else if (ui->prefix->currentText() == "KHz")
        frequency *= 1.0E3;

    if (frequency != _frequency_Hz) {
        _frequency_Hz = frequency;
        emit frequencyChanged(_frequency_Hz);
    }
}
