#include "PowerPoint.h"


// RsaToolbox
using namespace RsaToolbox;

PowerPoint::PowerPoint()
{
    reset();
}
PowerPoint::PowerPoint(const PowerPoint &other) :
    power_dBm(other.power_dBm),
    sParameters(other.sParameters)
{

}
PowerPoint::PowerPoint(double power_dBm, RsaToolbox::ComplexMatrix2D sParameters) {
    this->power_dBm = power_dBm;
    this->sParameters = sParameters;
}

void PowerPoint::reset() {
    power_dBm = 0;
    sParameters.clear();
    sParameters.resize(2);
    sParameters[0].resize(2);
    sParameters[1].resize(2);
}

ComplexRowVector &PowerPoint::operator[](int index) {
    return sParameters[index];
}
void PowerPoint::operator=(const PowerPoint &other) {
    power_dBm = other.power_dBm;
    sParameters = other.sParameters;
}
