#include "Results.h"


// RsaToolbox
#include "General.h"
using namespace RsaToolbox;

Results::Results(QObject *parent) :
    QObject(parent)
{
    reset();
}

Results::~Results()
{

}

bool Results::isGainExpansionMode() const {
    return _isGainExpansionMode;
}
void Results::setGainExpansionMode(bool on) {
    _isGainExpansionMode = on;
}

bool Results::isCompressionValue() const {
    return _compressionValue > 0;
}
double Results::compressionValue() const {
    return _compressionValue;
}
void Results::setCompressionValue(double value_dB) {

    _compressionValue = value_dB;
}

bool Results::hasCompressionPoint(uint frequencyIndex) const {
    if (!isCompressionValue())
        return false;

    PowerSweep sweep = _sweeps[frequencyIndex];
    double reference;
    if (isGainExpansionMode())
        reference = max(toDb(sweep.sParameter(2, 1)));
    else
        reference = toDb(sweep.sParameter(2,1).at(0));

    for (int i = 0; i < sweep.points(); i++)
        if (sweep)
}

void Results::reset() {
    _isGainExpansionMode = false;
    _compressionValue = 0;
    _sweeps.clear();
}
