#ifndef GETCALIBRATION_H
#define GETCALIBRATION_H


#include <Vna.h>

#include <QDialog>

namespace Ui {
class getCalibration;
}

class getCalibration : public QDialog
{
    Q_OBJECT

public:
    explicit getCalibration(RsaToolbox::Vna *vna, QWidget *parent = 0);
    ~getCalibration();

    bool isCalSelected() const;
    QString selectedCal() const;

public slots:
    virtual int exec();

private slots:
    void processCal();

private:
    Ui::getCalibration *ui;
    RsaToolbox::Vna *_vna;

    QStringList _calibrations;
    QString _selectedCal;
};

#endif // GETCALIBRATION_H
