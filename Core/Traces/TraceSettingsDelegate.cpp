#include "TraceSettingsDelegate.h"


// Project
#include "TraceSettingsModel.h"
#include "TracesWidget.h"

// RsaToolbox
#include <FrequencyEdit.h>
#include <PowerEdit.h>
using namespace RsaToolbox;

// Qt
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QDebug>


TraceSettingsDelegate::TraceSettingsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

TraceSettingsDelegate::~TraceSettingsDelegate()
{

}

void TraceSettingsDelegate::setFrequencies(const QRowVector &frequencies_Hz) {
    _frequencies_Hz = frequencies_Hz;
}
void TraceSettingsDelegate::setPowers(const QRowVector &powers_dBm) {
    _powers_dBm = powers_dBm;
}
void TraceSettingsDelegate::setTracesWidget(TracesWidget *tracesWidget) {
    _tracesWidget = tracesWidget;
}

QWidget *TraceSettingsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    const int row = index.row();
    const int column = index.column();
    const TraceSettingsModel *model = qobject_cast<const TraceSettingsModel*>(index.model());
    if (!model)
        return 0;

    QPixmap pixmap(10, 5);
    QStringList items;
    QComboBox *combo;
    QLineEdit *lineEdit;
    FrequencyEdit *frequencyEdit;
    PowerEdit *powerEdit;
    QRegExpValidator *validator;
    switch (column) {
    case TraceSettingsModel::Column::name:
        lineEdit = new QLineEdit(parent);
        validator = new QRegExpValidator(TraceSettings::nameRegex, lineEdit);
        lineEdit->setValidator(validator);
        return lineEdit;
    case TraceSettingsModel::Column::yParameter:
        combo = new QComboBox(parent);
        combo->setEditable(false);
        items << model->traces()[row].possibleYParameters();
        combo->addItems(items);
        return combo;
    case TraceSettingsModel::Column::xParameter:
        combo = new QComboBox(parent);
        combo->setEditable(false);
        items << model->traces()[row].possibleXParameters();
        combo->addItems(items);
        return combo;
    case TraceSettingsModel::Column::atParameter:
        combo = new QComboBox(parent);
        combo->setEditable(false);
        items << model->traces()[row].possibleAtParameters();
        combo->addItems(items);
        return combo;
    case TraceSettingsModel::Column::atValue:
        if (model->traces()[row].isAtFrequency()) {
            frequencyEdit = new FrequencyEdit(parent);
            frequencyEdit->setParameterName("Frequency");
            frequencyEdit->setAcceptedValues(_frequencies_Hz);
            connect(frequencyEdit, SIGNAL(outOfRange(QString)),
                    _tracesWidget, SIGNAL(error(QString)));
            return frequencyEdit;
        }
        else if (model->traces()[row].isAtPin()) {
            powerEdit = new PowerEdit(parent);
            powerEdit->setParameterName("Power");
            powerEdit->setAcceptedValues(_powers_dBm);
            connect(powerEdit, SIGNAL(outOfRange(QString)),
                    _tracesWidget, SIGNAL(error(QString)));
            return powerEdit;
        }
    default:
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}
void TraceSettingsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox *combo = qobject_cast<QComboBox*>(editor);
    if (combo) {
        combo->setCurrentText(index.data(Qt::EditRole).toString());
        return;
    }

    FrequencyEdit *frequencyEdit = qobject_cast<FrequencyEdit*>(editor);
    if (frequencyEdit) {
        frequencyEdit->setFrequency(index.data(Qt::EditRole).toDouble());
        return;
    }

    PowerEdit *powerEdit = qobject_cast<PowerEdit*>(editor);
    if (powerEdit) {
        powerEdit->setPower(index.data(Qt::EditRole).toDouble());
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit) {
        lineEdit->setText(index.data(Qt::EditRole).toString());
        return;
    }

    // Default:
    QStyledItemDelegate::setEditorData(editor, index);
}
void TraceSettingsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (!model)
        return;

    QComboBox *combo = qobject_cast<QComboBox*>(editor);
    if (combo) {
        model->setData(index, combo->currentText());
        return;
    }

    FrequencyEdit *frequencyEdit = qobject_cast<FrequencyEdit*>(editor);
    if (frequencyEdit) {
        if (frequencyEdit->hasAcceptableInput())
            model->setData(index, frequencyEdit->frequency_Hz());
        return;
    }

    PowerEdit *powerEdit = qobject_cast<PowerEdit*>(editor);
    if (powerEdit) {
        if (powerEdit->hasAcceptableInput())
            model->setData(index, powerEdit->power_dBm());
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit) {
        if (lineEdit->hasAcceptableInput())
            model->setData(index, lineEdit->text());
        return;
    }

    // Default:
    QStyledItemDelegate::setModelData(editor, model, index);
}
void TraceSettingsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
