#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// RsaToolbox
#include "Keys.h"
#include "Vna.h"

// Qt
#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(RsaToolbox::Vna &vna, RsaToolbox::Keys &keys, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    // Buttons
    void on_cancel_clicked();
    void on_measure_clicked();
    void on_exportData_clicked();

    void shake();

private:
    Ui::MainWindow *ui;
    RsaToolbox::Vna &vna;
    RsaToolbox::Keys &keys;

    bool _isMeasuring;
    QRect _settingsGeometry;
    QRect progressGeometry() const;
    void showProgressPage();
    void showSettingsPage();
};


#endif // MAINWINDOW_H
