#ifndef POWERSUPPLYPAGE_H
#define POWERSUPPLYPAGE_H


// RsaToolbox
#include "Keys.h"
#include "LastPath.h"
#include "ErrorLabel.h"


// qt
#include <QWidget>


// forward declare ui
namespace Ui {
class PowerSupplyPage;
}


class PowerSupplyPage : public QWidget
{
    Q_OBJECT

public:
    explicit PowerSupplyPage(QWidget *parent = nullptr);
    ~PowerSupplyPage();


    // keys
    void setKeys(RsaToolbox::Keys *keys);
    void loadKeys();
    void saveKeys();


    // pae on
    bool isPAEOn() const;
    void setPAEOn(bool isOn = true);


    // power supply
    QString visaResource() const;
    QString driverFilePath() const;


    bool hasAcceptableInput();


    RsaToolbox::ErrorLabel *errorLabel();

signals:

    void exportClicked();
    void miniGuiClicked();
    void closeClicked();
    void measureClicked();

    void inputError(QString message);


public slots:

    void getDriverDialog();

    void enableExport();
    void disableExport();


private:

    Ui::PowerSupplyPage *ui;

    RsaToolbox::Keys *_keys;

    // visa
    void setVisaResource(const QString& resource);

    // driver
    QString _driverFilePath;
    RsaToolbox::SharedLastPath _lastPath;
    QString driverFileName() const;
    QString driverPath() const;
    void setDriver(const QString& filePath);
};

#endif // POWERSUPPLYPAGE_H
