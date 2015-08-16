#include "TracesWidget.h"
#include "ui_TracesWidget.h"

#include <QDebug>

TracesWidget::TracesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TracesWidget)
{
    ui->setupUi(this);
    _model.insertRows(0, 2);
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
}

TracesWidget::~TracesWidget()
{
    delete ui;
}

QVector<TraceSettings> TracesWidget::traces() const {
    return _model.traces();
}

void TracesWidget::on_add_clicked()
{
    QItemSelection selection = ui->table->selectionModel()->selection();
    int row = 0;
    if (!selection.isEmpty() && !selection.first().isEmpty())
        row = selection.first().topLeft().row();
    qDebug() << "index: " << row;
    _model.insertRows(row, 1);
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
        _model.removeRows(row, 1);
        if (!traces().isEmpty()) {
            if (traces().size() > row)
                ui->table->selectRow(row);
            else
                ui->table->selectRow(traces().size()-1);
        }
    }
}
