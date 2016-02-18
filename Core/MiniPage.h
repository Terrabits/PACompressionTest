#ifndef MINIPAGE_H
#define MINIPAGE_H


// RsaToolbox
#include <ErrorLabel.h>

// Qt
#include <QWidget>


namespace Ui {
class MiniPage;
}

class MiniPage : public QWidget
{
    Q_OBJECT

public:
    explicit MiniPage(QWidget *parent = 0);
    ~MiniPage();

    RsaToolbox::ErrorLabel *errorLabel();

signals:
    void exportClicked();
    void standardGuiClicked();
    void closeClicked();
    void measureAndPlotClicked();

public slots:
    void startMeasurement();
    void startSweep(const QString &caption, uint time_ms);
    void stopSweep();
    void updateTotalProgress(int percent);
    void finishMeasurement();

    void enableExport();
    void disableExport();

private:
    Ui::MiniPage *ui;
};

#endif // MINIPAGE_H
