#ifndef PROCESSTRACE_H
#define PROCESSTRACE_H


// Project
#include "TraceSettings.h"
#include "MeasurementData.h"

// RsaToolbox
#include <General.h>
#include <Vna.h>

// Qt
#include <Qt>
#include <QString>


class ProcessTrace
{
public:
    ProcessTrace(TraceSettings *settings, MeasurementData *data, RsaToolbox::Vna *vna, uint defaultDiagram);
    ~ProcessTrace();

private:
    TraceSettings *_settings;
    MeasurementData *_data;
    RsaToolbox::Vna *_vna;

    uint _diagram;
    uint _channel;
    QString _channelName;
    QString _dataTraceName;
    QString _memoryTraceName;
    bool isPreexistingTrace();

    RsaToolbox::QRowVector _x;
    RsaToolbox::QRowVector _y_formatted;
    RsaToolbox::ComplexRowVector _y_complex;
    static RsaToolbox::ComplexRowVector toComplex_dBm(RsaToolbox::QRowVector values_dBm);
    static RsaToolbox::ComplexRowVector toComplex_deg(RsaToolbox::QRowVector values_deg);
    bool retrieveData();

    void createChannel();
    void updateChannel();

    void createTrace();
    void updateTrace();

    void debugPrint(const QString &name);
};

#endif // PROCESSTRACE_H
