#ifndef MEASURETHREAD_H
#define MEASURETHREAD_H


// PA Compression Test
#include "MeasurementData.h"

// RsaToolbox
#include "Definitions.h"
#include "Vna.h"

// Qt
#include <QThread>


class MeasureThread : public QThread {
    Q_OBJECT

public:
    MeasureThread(RsaToolbox::Vna *vna, MeasurementData *data,
              QObject *parent = 0);
    ~MeasureThread();

signals:
    void progress(int percent);

private slots:
    void run();

    void initialize();
    void runSweeps();

    void displayResultOnVna();
    static void flipPorts(QVector<RsaToolbox::NetworkData> &sweeps);

private:
    RsaToolbox::Vna *_vna;
    MeasurementData &_data;
    QVector<uint> _ports;
};


#endif // MEASURETHREAD_H
