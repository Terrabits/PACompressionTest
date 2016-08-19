#ifndef VnaTrace_H
#define VnaTrace_H


// RsaToolbox
#include "Definitions.h"
#include "BalancedPort.h"
#include "NetworkTraceData.h"
#include "VnaMarker.h"
#include "VnaReferenceMarker.h"
#include "VnaLimits.h"
#include "VnaMath.h"
#include "VnaTimeDomain.h"

// Qt
#include <QObject>
#include <QScopedPointer>


namespace RsaToolbox {
class Vna;

class VnaTrace : public QObject
{
    Q_OBJECT

public:
    explicit VnaTrace(QObject *parent = 0);
    VnaTrace(const VnaTrace &other);
    VnaTrace(Vna *vna, QString name, QObject *parent = 0);
    ~VnaTrace();

    bool isVisible();
    bool isHidden();

    void select();
    void autoscale();
    void show(bool isVisible = true);
    void hide(bool isInvisible = true);

    QString name();
    uint channel();
    // void setChannel(); // ?
    uint diagram();
    void setDiagram(uint index);

    bool isNetworkParameter();
    void networkParameter(NetworkParameter &parameter, uint &outputPort, uint &inputPort);
    void networkParameter(NetworkParameter &parameter, BalancedPort &outputPort, BalancedPort &inputPort);
    void setNetworkParameter(NetworkParameter parameter, uint outputPort, uint inputPort);
    void setNetworkParameter(NetworkParameter parameter, BalancedPort outputPort, BalancedPort inputPort);
    void measure(NetworkTraceData &data);

    bool isWaveQuantity();
//    void waveQuantity(WaveQuantity &wave, uint &port);
    void setWaveQuantity(WaveQuantity wave, uint wavePort, uint sourcePort);
    // void measure(WaveTraceData &data);

    bool isWaveRatio();
//    void waveRatio(WaveQuantity &numeratorWave, uint &numeratorPort, WaveQuantity &denominatorWave, uint &denominatorPort);
    void setWaveRatio(WaveQuantity numeratorWave, uint numeratorWavePort, uint numeratorSourcePort, WaveQuantity denominatorWave, uint denominatorWavePort, uint denominatorSourcePort);
    // void measure(WaveRatioTraceData &data);

    bool isImpedance();
    void impedance(uint &outputPort, uint &inputPort);
    void impedance(BalancedPort &outputPort, BalancedPort &inputPort);
    void setImpedance(uint outputPort, uint inputPort);
    void setImpedance(BalancedPort outputPort, BalancedPort inputPort);
    // void measure(ImpedanceTraceData &data);

    bool isAdmittance();
    void admittance(uint &outputPort, uint &inputPort);
    void admittance(BalancedPort &outputPort, BalancedPort &inputPort);
    void setAdmittance(uint outputPort, uint inputPort);
    void setAdmittance(BalancedPort outputPort, BalancedPort inputPort);
    // void measure(ImpedanceTraceData &data);

    bool isTimeDomain();
    bool isNotTimeDomain();
    VnaTimeDomain &timeDomain();

    TraceFormat format();
    void setFormat(TraceFormat format);

    void setDisplayProperties(int red, int green, int blue, Qt::PenStyle style = Qt::SolidLine, int width = 1);
    void setDisplayProperties(QColor color, Qt::PenStyle style = Qt::SolidLine, int width = 1);
    void setDisplayProperties(QPen pen);

    void setYAxisMinimum(double min);
    void setYAxisMaximum(double max);
    void setYAxis(double min, double max);

    uint points();
    QRowVector x();
    ComplexRowVector y();
    void y(QRowVector &y);
    void y(ComplexRowVector &y);

    void toMemory(QString name);
    void write(QRowVector data);
    void write(QRowVector frequencies_Hz, QRowVector data);
    void write(ComplexRowVector data);
    void write(QRowVector frequencies_Hz, ComplexRowVector data);

    // Marker
    bool isMarker(uint index);
    bool isNotMarker(uint index);
    bool isMarkers();
    bool isNotMarkers();
    QVector<uint> markers();
    void createMarker(uint index);
    uint createMarker();
    void deleteMarker(uint index);
    void deleteMarkers();
    VnaMarker &marker(uint index);

    // Reference Marker
    VnaReferenceMarker &referenceMarker();

    // Limit Lines
    VnaLimits &limits();

    // Trace math
    VnaMath &math();

    void operator=(VnaTrace const &other);

private:
    Vna *_vna;
    QScopedPointer<Vna> placeholder;
    QString _name;
    QScopedPointer<VnaMarker> _marker;
    QScopedPointer<VnaReferenceMarker> _referenceMarker;
    QScopedPointer<VnaLimits> _limits;
    QScopedPointer<VnaMath> _math;
    QScopedPointer<VnaTimeDomain> _timeDomain;
    
    bool isFullyInitialized() const;

    // Scpi
    QString measurementString();
    uint bufferSize();
    uint complexBufferSize();
    void parseParameters(QString scpi);
    void parseTwoPortIndices(QString scpi, uint &output, uint &input);
};
}

#endif // VnaTrace_H
