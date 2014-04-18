#ifndef GETCALIBRATION_H
#define GETCALIBRATION_H

#include <QDialog>

namespace Ui {
class GetCalibration;
}

class GetCalibration : public QDialog
{
    Q_OBJECT
    
public:
    explicit GetCalibration(QStringList calibrations,
                            QWidget *parent = 0);
    ~GetCalibration();

public slots:
    QString calibration();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::GetCalibration *ui;
    QString _calibration;
};

#endif // GETCALIBRATION_H
