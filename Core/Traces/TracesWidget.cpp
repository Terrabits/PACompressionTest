#include "TracesWidget.h"
#include "ui_TracesWidget.h"


// Qt
#include <QDebug>


TracesWidget::TracesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TracesWidget)
{
    ui->setupUi(this);

    ui->table->setModel(&_model);
    ui->table->setItemDelegate(&_delegate);

    connect(&_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SIGNAL(tracesChanged()));
    connect(&_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SIGNAL(tracesChanged()));
    connect(&_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SIGNAL(tracesChanged()));
    connect(&_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SIGNAL(tracesChanged()));
    connect(&_model, SIGNAL(modelReset()),
            this, SIGNAL(tracesChanged()));

    setFixedColumnWidths();
}

TracesWidget::~TracesWidget()
{
    delete ui;
}

void TracesWidget::setFrequencies(const RsaToolbox::QRowVector &frequencies_Hz) {
    _delegate.setFrequencies(frequencies_Hz);
    _model.setFrequencies(frequencies_Hz);
}
void TracesWidget::setPowers(const RsaToolbox::QRowVector &powers_dBm) {
    _delegate.setPowers(powers_dBm);
    _model.setPinValues(powers_dBm);
}

bool TracesWidget::isTracesValid() {
    QString msg;
    const bool valid = isTracesValid(msg);
    if (!valid)
        emit error(msg);
    return valid;
}
bool TracesWidget::isTracesValid(QString &message) const {
    message.clear();
    QVector<TraceSettings> _traces = traces();

    QStringList names;
    for (int i = 0; i < _traces.size(); i++) {
        const QString name = _traces[i].name;
        if (name.isEmpty()) {
            message = "*Name cannot be blank";
            ui->table->selectionModel()->select(_model.index(i, TraceSettingsModel::Column::name), QItemSelectionModel::ClearAndSelect);
            ui->table->selectionModel()->setCurrentIndex(_model.index(i, TraceSettingsModel::Column::name), QItemSelectionModel::ClearAndSelect);
            ui->table->setFocus();
            return false;
        }
        if (names.contains(name, Qt::CaseInsensitive)) {
            message = "*Names must be unique";
            ui->table->selectionModel()->select(_model.index(i, TraceSettingsModel::Column::name), QItemSelectionModel::ClearAndSelect);
            ui->table->selectionModel()->setCurrentIndex(_model.index(i, TraceSettingsModel::Column::name), QItemSelectionModel::ClearAndSelect);
            ui->table->setFocus();
            return false;
        }

        names << name;
    }

    return true;
}
QVector<TraceSettings> TracesWidget::traces() const {
    return _model.traces();
}
void TracesWidget::setTraces(const QVector<TraceSettings> &traces) {
    _model.setTraces(traces);
}

void TracesWidget::on_add_clicked()
{
    QItemSelection selection = ui->table->selectionModel()->selection();
    int row = 0;
    if (!selection.isEmpty() && !selection.first().isEmpty())
        row = selection.first().topLeft().row();
    _model.insertRows(row, 1);
    ui->table->selectRow(row);
}

void TracesWidget::on_remove_clicked()
{
    QItemSelection selection = ui->table->selectionModel()->selection();
    int row = -1;
    if (!selection.isEmpty() && !selection.first().isEmpty())
        row = selection.first().topLeft().row();
    if (row != -1) {
        _model.removeRows(row, 1);
        if (!traces().isEmpty()) {
            if (traces().size() > row)
                ui->table->selectRow(row);
            else
                ui->table->selectRow(traces().size()-1);
        }
    }
}

void TracesWidget::setFixedColumnWidths() {
    ui->table->setColumnWidth(0, 240); // name
    ui->table->setColumnWidth(1, 130); // y parameter
    ui->table->setColumnWidth(2, 100); // x parameter
    ui->table->setColumnWidth(3, 110); // at parameter
    // 4: atValue (stretch)
}
