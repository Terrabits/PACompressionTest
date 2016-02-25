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

signals:
    void exportClicked();
    void standardGuiClicked();
    void closeClicked();
    void measureAndPlotClicked();

    void animationFinished();

public slots:
    void drag(int x, int y);
    void animateMove(int x, int y);

    void showError(const QString &message);
    void showInfo(const QString &message);

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
