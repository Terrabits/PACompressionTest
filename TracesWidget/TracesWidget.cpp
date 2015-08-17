#include "TracesWidget.h"
#include "ui_TracesWidget.h"

#include <QDebug>

TracesWidget::TracesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TracesWidget),
    _model(0)
{
    ui->setupUi(this);
    ui->table->setItemDelegate(&_delegate);
}

TracesWidget::~TracesWidget()
{
    delete ui;
}

TraceSettingsModel *TracesWidget::model() const {
    return _model;
}
void TracesWidget::setModel(TraceSettingsModel *model) {
    if (_model == model)
        return;

    if (_model) {
        disconnect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SIGNAL(tracesChanged()));
        disconnect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SIGNAL(tracesChanged()));
        disconnect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SIGNAL(tracesChanged()));
        disconnect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SIGNAL(tracesChanged()));
        disconnect(_model, SIGNAL(modelReset()),
                this, SIGNAL(tracesChanged()));
    }

    _model = model;
    ui->table->setModel(_model);
    if (_model) {
        _model->insertRows(0, 2);
        connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SIGNAL(tracesChanged()));
        connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SIGNAL(tracesChanged()));
        connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SIGNAL(tracesChanged()));
        connect(_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SIGNAL(tracesChanged()));
        connect(_model, SIGNAL(modelReset()),
                this, SIGNAL(tracesChanged()));
    }
}

QVector<TraceSettings> TracesWidget::traces() const {
    return _model->traces();
}

void TracesWidget::on_add_clicked()
{
    QItemSelection selection = ui->table->selectionModel()->selection();
    int row = 0;
    if (!selection.isEmpty() && !selection.first().isEmpty())
        row = selection.first().topLeft().row();
    qDebug() << "index: " << row;
    _model->insertRows(row, 1);
    ui->table->selectRow(row);
}

void TracesWidget::on_remove_clicked()
{
    QItemSelection selection = ui->table->selectionModel()->selection();
    int row = -1;
    if (!selection.isEmpty() && !selection.first().isEmpty())
        row = selection.first().topLeft().row();
    qDebug() << "index: " << row;
    if (row != -1) {
        _model->removeRows(row, 1);
        if (!traces().isEmpty()) {
            if (traces().size() > row)
                ui->table->selectRow(row);
            else
                ui->table->selectRow(traces().size()-1);
        }
    }
}
