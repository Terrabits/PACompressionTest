#ifndef FREQUENCYSWEEP_H
#define FREQUENCYSWEEP_H


// Project
#include "MeasureThread.h"

// Qt
#include <QObject>


class FrequencySweep : public MeasureThread
{
    Q_OBJECT
public:
    explicit FrequencySweep(QObject *parent = 0);
    ~FrequencySweep();

protected:
    virtual void run();

};

#endif // FREQUENCYSWEEP_H
