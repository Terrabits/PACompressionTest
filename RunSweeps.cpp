

#include "RunSweeps.h"

#include <QApplication>

using namespace RsaToolbox;


RunSweeps::RunSweeps(Vna &vna, QRowVector &frequency_points, QMatrix2D &data, QObject *parent)
    : QThread(parent),
      vna(vna),
      frequency_points(frequency_points),
      data(data)
{
    // Not much else to do...
}

void RunSweeps::run() {
    const int points = frequency_points.size();
    data.clear();
    data.resize(points);
    for (int i = 0; i < points; i++) {
        RsaToolbox::TraceData trace_data;
        vna.Channel().SetCwFrequency(frequency_points[i]);
        frequency_points[i] = vna.Channel().GetCwFrequency_Hz();
        vna.Channel().InitiateSweep();
        vna.FinishPreviousCommandsFirst();
        vna.Trace().MeasureTrace(trace_data);
        data[i] = QRowVector::fromStdVector(trace_data.data);
        emit Progress(((i+1.0)/points)*100);
    }
    vna.moveToThread(QApplication::instance()->thread());
}

