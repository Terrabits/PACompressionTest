#ifndef PROCESSTRACE_H
#define PROCESSTRACE_H


// Project
#include "TraceSettings.h"
#include "MeasurementData.h"

// RsaToolbox
#include <Definitions.h>

// Qt
#include <QString>

class ProcessTrace
{
public:
    ProcessTrace(TraceSettings &settings, MeasurementData &data);
    ~ProcessTrace();

    bool isXFrequency();

    QString name();
    QString yUnits();
    QString xUnits();
    RsaToolbox::QRowVector y();
    RsaToolbox::QRowVector x();
    RsaToolbox::QRowVector x(RsaToolbox::SiPrefix prefix);

private:
    TraceSettings &_settings;
    MeasurementData &_data;
};

#endif // PROCESSTRACE_H
