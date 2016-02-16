#ifndef PROGRESSPAGE_H
#define PROGRESSPAGE_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QWidget>

namespace Ui {
class ProgressPage;
}

class ProgressPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressPage(QWidget *parent = 0);
    ~ProgressPage();

public slots:
    void startMeasurement(double startFrequency_Hz, double stopFrequency_Hz, double startPower_dBm, double stopPower_dBm);

    void startSweep(const QString &caption, uint time_ms);
    void stopSweep();
    void updateTotalProgress(int percent);

    void plotMaxGain(const RsaToolbox::QRowVector &frequencies_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequencies_Hz, const RsaToolbox::QRowVector &pin_dBm);

    void finishMeasurement();

private:
    Ui::ProgressPage *ui;

    void initializePlot(double startFrequency_Hz, double stopFrequency_Hz, double startPower_dBm, double stopPower_dBm);
};

#endif // PROGRESSPAGE_H
