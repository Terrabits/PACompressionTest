#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H


// Project
#include "MeasurementSettings.h"

// RsaToolbox
#include <Vna.h>
#include <Keys.h>
#include <ErrorLabel.h>

// Qt
#include <QWidget>


namespace Ui {
class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = 0);
    ~SettingsPage();

    void setVna(RsaToolbox::Vna *vna);
    void setKeys(RsaToolbox::Keys *keys);
    void loadKeys();
    void saveKeys() const;

    bool hasAcceptableInput();
    MeasurementSettings settings() const;

    RsaToolbox::ErrorLabel *errorLabel();

signals:
    void exportClicked();
    void miniGuiClicked();
    void closeClicked();
    void measureClicked();

    void inputError();

public slots:
    void enableExport();
    void disableExport();

private slots:
    void updatePowerStepSize();

private:
    Ui::SettingsPage *ui;

    mutable RsaToolbox::Vna *_vna;
    mutable RsaToolbox::Keys *_keys;
};

#endif // SETTINGSPAGE_H
