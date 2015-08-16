#include "TraceSettingsDelegate.h"


// Project
#include "TraceSettingsModel.h"

// Qt
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>


TraceSettingsDelegate::TraceSettingsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

TraceSettingsDelegate::~TraceSettingsDelegate()
{

}

QWidget *TraceSettingsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    const int row = index.row();
    const int column = index.column();
    const TraceSettingsModel *model = qobject_cast<const TraceSettingsModel*>(index.model());
    if (!model)
        return 0;

    QStringList items;
    QComboBox *combo;
    QLineEdit *edit;
    switch (column) {
    case TraceSettingsModel::Column::yAxis:
        combo = new QComboBox(parent);
        items << model->traces()[row].possibleYAxis();
        combo->addItems(items);
        return combo;
    case TraceSettingsModel::Column::yParameter:
        combo = new QComboBox(parent);
        combo->setEditable(false);
        items << model->traces()[row].possibleYParameters();
        combo->addItems(items);
        return combo;
    case TraceSettingsModel::Column::yFormat:
        combo = new QComboBox(parent);
        combo->setEditable(false);
        items << model->traces()[row].possibleYFormats();
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
        edit = new QLineEdit(parent);
        if (model->traces()[row].isAtFrequency()) {
            edit->setValidator(new QDoubleValidator(0, 1E12, 3, edit));
        }
        else if (model->traces()[row].isAtPin()) {
            edit->setValidator(new QDoubleValidator(-150, 100, 2, edit));
        }
        return edit;
    default:
        return 0;
    }
}
void TraceSettingsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox *combo = qobject_cast<QComboBox*>(editor);
    if (combo) {
        combo->setCurrentText(index.data(Qt::EditRole).toString());
        return;
    }

    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.data(Qt::EditRole).toString());
        return;
    }
}
void TraceSettingsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (!model)
        return;

    QComboBox *combo = qobject_cast<QComboBox*>(editor);
    if (combo) {
        model->setData(index, combo->currentText());
        return;
    }

    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        if (edit->hasAcceptableInput())
            model->setData(index, edit->text());
        return;
    }
}
void TraceSettingsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}
