#ifndef RUNSWEEPS_H
#define RUNSWEEPS_H


// PA Compression Test
#include "MeasurementData.h"

// RsaToolbox
#include "Definitions.h"
#include "Vna.h"

// Qt
#include <QThread>


class RunSweeps : public QThread {
    Q_OBJECT

public:
    RunSweeps(RsaToolbox::Vna *vna, MeasurementData *data,
              QObject *parent = 0);
    ~RunSweeps();

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


#endif // RUNSWEEPS_H
