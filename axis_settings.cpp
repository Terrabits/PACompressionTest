#include "axis_settings.h"
#include "ui_axis_settings.h"

// Qt
#include <QDoubleValidator>
#include <QMessageBox>

axis_settings::axis_settings(double &xMin, double &xMax, double &yMin, double &yMax, QWidget *parent)
    : xMin(xMin),
      xMax(xMax),
      yMin(yMin),
      yMax(yMax),
      QDialog(parent),
      ui(new Ui::axis_settings)
{
    ui->setupUi(this);

    ui->xMin_line_edit->setText(QVariant(xMin).toString());
    ui->xMax_line_edit->setText(QVariant(xMax).toString());
    ui->yMin_line_edit->setText(QVariant(yMin).toString());
    ui->yMax_line_edit->setText(QVariant(yMax).toString());

    ui->xMin_line_edit->setValidator(new QDoubleValidator(this));
    ui->xMax_line_edit->setValidator(new QDoubleValidator(this));
    ui->yMin_line_edit->setValidator(new QDoubleValidator(this));
    ui->yMax_line_edit->setValidator(new QDoubleValidator(this));
}

axis_settings::~axis_settings()
{
    delete ui;
}

bool axis_settings::isInputValid() {
    double xMin, xMax,
           yMin, yMax;
    xMin = ui->xMin_line_edit->text().toDouble();
    xMax = ui->xMax_line_edit->text().toDouble();
    yMin = ui->yMin_line_edit->text().toDouble();
    yMax = ui->yMax_line_edit->text().toDouble();
    if (xMin > xMax) {
        ui->xMin_line_edit->selectAll();
        ui->xMin_line_edit->setFocus();
        QMessageBox::warning(this,
                             "X-Axis Invalid",
                             "Min value must be less than Max value");
        return(false);
    }
    if (yMin > yMax) {
        ui->yMin_line_edit->selectAll();
        ui->yMin_line_edit->setFocus();
        QMessageBox::warning(this,
                             "Y-Axis Invalid",
                             "Min value must be less than Max value");
        return(false);
    }

    // else
    return(true);


}
void axis_settings::on_ok_push_button_clicked()
{
    if (isInputValid() == true) {
        xMin = ui->xMin_line_edit->text().toDouble();
        xMax = ui->xMax_line_edit->text().toDouble();
        yMin = ui->yMin_line_edit->text().toDouble();
        yMax = ui->yMax_line_edit->text().toDouble();
        close();
    }
}
