#ifndef RUNSWEEPS_H
#define RUNSWEEPS_H

// Rsa
#include "Definitions.h"
#include "TraceData.h"
#include "Vna.h"

// Qt
#include <QThread>

class RunSweeps : public QThread {
    Q_OBJECT

public:
    RunSweeps(RsaToolbox::Vna &vna,
              RsaToolbox::QRowVector &frequency_points, RsaToolbox::QMatrix2D &data,
              QObject *parent = 0);
    //~RunSweeps();

signals:
    void Progress(int percent);

private slots:
    void run();

private:
    RsaToolbox::Vna &vna;
    RsaToolbox::QRowVector &frequency_points;
    RsaToolbox::QMatrix2D &data;
};


#endif // RUNSWEEPS_H
