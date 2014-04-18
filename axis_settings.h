#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <QDialog>

namespace Ui {
class axis_settings;
}

class axis_settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit axis_settings(double &xMin,
                           double &xMax,
                           double &yMin,
                           double &yMax,
                           QWidget *parent = 0);
    ~axis_settings();
    
private slots:
    bool isInputValid();
    void on_ok_push_button_clicked();

private:
    Ui::axis_settings *ui;
    double &xMin;
    double &xMax;
    double &yMin;
    double &yMax;
};

#endif // AXIS_SETTINGS_H
