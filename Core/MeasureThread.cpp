#include <QDebug>

#include "MeasureThread.h"

// RsaToolbox
#include <General.h>
#include <NetworkData.h>
#include <NetworkTraceData.h>

// C++ std lib
#include <complex>

// Qt
#include <QApplication>
#include <QMessageBox>

using namespace RsaToolbox;


MeasureThread::MeasureThread(QObject *parent)
    : QThread(parent)
{

}
MeasureThread::~MeasureThread()
{

}

void MeasureThread::flipPorts(QVector<NetworkData> &sweeps) {
    int iMax = sweeps.size();
    int jMax = int(sweeps[0].points());
    for (int i = 0; i < iMax; i++) {
        for (int j = 0; j < jMax; j++) {
            ComplexMatrix2D current_sweep = sweeps[i].y()[j];
            sweeps[i].y()[j][0][0] = current_sweep[1][1];
            sweeps[i].y()[j][1][1] = current_sweep[0][0];
            sweeps[i].y()[j][0][1] = current_sweep[1][0];
            sweeps[i].y()[j][1][0] = current_sweep[0][1];
        }
    }
}
