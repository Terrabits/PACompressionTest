#ifndef SAFEFREQUENCYSWEEP_H
#define SAFEFREQUENCYSWEEP_H


// Project
#include "MeasureThread.h"

// Qt
#include <QObject>


class SafeFrequencySweep : public MeasureThread
{
    Q_OBJECT
public:
    explicit SafeFrequencySweep(QObject *parent = 0);
    ~SafeFrequencySweep();

protected:
    virtual void run();

};

#endif // SAFEFREQUENCYSWEEP_H
