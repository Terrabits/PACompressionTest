#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Project
#include "MeasurementData.h"

// RsaToolbox
#include <Keys.h>
#include <Vna.h>

// Qt
#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(RsaToolbox::Vna &_vna, RsaToolbox::Keys &_keys, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void plot();

private:
    Ui::MainWindow *ui;

    RsaToolbox::Keys &_keys;
    RsaToolbox::Vna &_vna;

    MeasurementData _data;
};

#endif // MAINWINDOW_H
