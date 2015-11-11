#include "FrequencySweep.h"

FrequencySweep::FrequencySweep(QObject *parent) :
    MeasureThread(parent)
{

}

FrequencySweep::~FrequencySweep()
{

}

void FrequencySweep::run() {
    QString msg;
    if (!_settings.isValid(*_vna, msg)) {
        setError(msg);
        emit finished();
        return;
    }


}
