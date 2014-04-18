#include "gettracename.h"
#include "ui_gettracename.h"

getTraceName::getTraceName(QStringList traces,
                           QWidget *parent)
    : QDialog(parent),
      ui(new Ui::getTraceName)
{
    ui->setupUi(this);
    ui->listWidget->clear();
    ui->listWidget->addItems(traces);
    ui->listWidget->setCurrentRow(0);
    trace_name = "";
}

getTraceName::~getTraceName() {
    delete ui;
}

QString getTraceName::traceName() {
    // ui->listWidget->currentItem()->text()
    return(trace_name);
}

void getTraceName::on_pushButton_clicked() {
    trace_name = ui->listWidget->currentItem()->text();
    this->close();
}
