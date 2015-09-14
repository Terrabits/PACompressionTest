#include <QDebug>

#include "RunSweeps.h"

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


RunSweeps::RunSweeps(Vna *vna, MeasurementData *data,
                     QObject *parent)
    : QThread(parent),
      _vna(vna),
      _data(*data)
{

}
RunSweeps::~RunSweeps()
{

}

void RunSweeps::run() {
    qDebug() << "RunSweeps::run";
    _data.processSettings();
    _data.resizeToPoints();

    _ports << _data.inputPort
           << _data.outputPort;

    runSweeps();

    _vna->channel().linearSweep().clearSParameterGroup();
    displayResultOnVna();
    _vna->channel().continuousSweepOn();
    _vna->channel(2).continuousSweepOn();
    _vna->moveToThread(QApplication::instance()->thread());
}

// Freq Sweep
void RunSweeps::initialize() {
    _vna->channel().setSweepType(VnaChannel::SweepType::Linear);
    _vna->trace().setNetworkParameter(NetworkParameter::S, _data.outputPort, _data.inputPort);
    _vna->trace().setFormat(TraceFormat::DecibelMagnitude);
    _vna->channel().linearSweep().setStart(_data.startFreq_Hz);
    _vna->channel().linearSweep().setStop(_data.stopFreq_Hz);
    _vna->channel().linearSweep().setPoints(_data.frequencyPoints);
    _vna->channel().manualSweepOn();

    _data.frequencies_Hz = _vna->channel().linearSweep().frequencies_Hz();
}
void RunSweeps::runSweeps() {
    qDebug() << "RunSweeps::runSweeps";
    qDebug() << "powerPoints: " << _data.powerPoints;
    qDebug() << "power_dBm.size: " << _data.power_dBm.size();
    qDebug() << "data.size: " << _data.data.size();

    qDebug() << "initialize() ...";
    initialize();

    qDebug() << "Starting sweeps";
    emit progress(0);
    qDebug() << "Progress: " << 0;
    for (uint i = 0; i < _data.powerPoints; i++) {
        _vna->channel().linearSweep().setPower(_data.power_dBm[i]);
        _data.power_dBm[i] = _vna->channel().linearSweep().power_dBm();
        _data.data[i] = _vna->channel().linearSweep().measure(_ports);
        emit progress((i+1.0)/_data.powerPoints*100);
        qDebug() << "Progress: " << (i+1.0)/_data.powerPoints*100;
    }

    if (_data.outputPort < _data.inputPort)
        flipPorts(_data.data);

    qDebug() << "Calculating metrics";
    _data.calculateMetrics();
}

// Misc.
void RunSweeps::displayResultOnVna() {
    if (_data.compressionFrequencies_Hz.size() == 0) {
        _vna->settings().displayOn();
        return;
    }

    // Create new channel, diagram
    uint d = _vna->createDiagram();
    uint c = _vna->createChannel();

    // Set segmented sweep
    _vna->channel(c).setFrequencies(_data.compressionFrequencies_Hz);

    // Y-Axis min/max
    double min = std::min(RsaToolbox::min(_data.powerInAtCompression_dBm), RsaToolbox::min(_data.powerOutAtCompression_dBm));
    double max = std::max(RsaToolbox::max(_data.powerInAtCompression_dBm), RsaToolbox::max(_data.powerOutAtCompression_dBm));
    roundAxis(min, max, 5, min, max);

    // Create traces
    _vna->createTrace("Trc2", c);
    _vna->trace("Trc2").setNetworkParameter(NetworkParameter::S, 2, 1);
    _vna->trace("Trc2").setDiagram(d);
    _vna->trace("Trc2").setYAxis(min, max);
    _vna->diagram(d).setTitle("Compression Point");
    _vna->trace("Trc2").toMemory("Pin");
    _vna->trace("Pin").setDiagram(d);
    _vna->trace("Pin").write(_data.powerInAtCompression_dBm);
    _vna->trace("Pin").setYAxis(min, max);
    _vna->trace("Trc2").toMemory("Pout");
    _vna->trace("Pout").setDiagram(d);
    _vna->trace("Pout").write(_data.powerOutAtCompression_dBm);
    _vna->trace("Pout").setYAxis(min, max);
    _vna->channel(2).startSweep();
    _vna->wait();
    _vna->settings().displayOn();
    _vna->channel(2).manualSweepOn();
    _vna->trace("Trc2").hide();
}
void RunSweeps::flipPorts(QVector<NetworkData> &sweeps) {
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
