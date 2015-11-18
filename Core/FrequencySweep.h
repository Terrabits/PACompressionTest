#ifndef FREQUENCYSWEEP_H
#define FREQUENCYSWEEP_H


// Project
#include "MeasureThread.h"

// Qt
#include <QBitArray>
#include <QObject>


class FrequencySweep : public MeasureThread
{
    Q_OBJECT
public:
    explicit FrequencySweep(QObject *parent = 0);
    ~FrequencySweep();

protected:
    virtual void run();

private:

};

#endif // FREQUENCYSWEEP_H
