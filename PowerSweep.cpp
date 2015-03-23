#include "PowerSweep.h"


// RsaToolbox
#include "General.h"
using namespace RsaToolbox;


PowerSweep::PowerSweep()
{
    reset();
}
PowerSweep::PowerSweep(const PowerSweep &other) :
    _frequency_Hz(other.frequency_Hz()),
    _points(other._points)
{

}
PowerSweep::PowerSweep(double frequency_Hz, QRowVector powers_dBm, ComplexMatrix3D sParameters) {
    reset();
    _frequency_Hz = frequency_Hz;
    addPoints(powers_dBm, sParameters);
}

double PowerSweep::frequency_Hz() const {
    return _frequency_Hz;
}
void PowerSweep::setFrequency(double frequency, RsaToolbox::SiPrefix prefix) {
    _frequency_Hz = frequency * toDouble(prefix);
}

uint PowerSweep::points() const {
    return _points.size();
}
QRowVector PowerSweep::powers_dBm() const {
    QRowVector result;
    foreach (const PowerPoint &p, _points) {
        result << p.power_dBm;
    }
    return result;
}
ComplexRowVector PowerSweep::sParameter(uint outputPort, uint inputPort) const {
    ComplexRowVector result;
    for (int i = 0; i < points(); i++)
        result.push_back(_points[i][outputPort][inputPort]);
    return result;
}

void PowerSweep::addPoints(QRowVector powers_dBm, ComplexMatrix3D sParameters) {
    int size = powers_dBm.size();
    for (int i = 0; i < size; i++) {
        PowerPoint point(powers_dBm[i], sParameters[i]);
        _points << point;
    }
}

PowerSweep &PowerSweep::operator<<(const PowerPoint &point) {
    _points << point;
}
PowerPoint &PowerSweep::operator[](int index) {
    return _points[index];
}
void PowerSweep::operator=(const PowerSweep &other) {
    _frequency_Hz = other._frequency_Hz;
    _points = other._points;
}

void PowerSweep::reset() {
    _frequency_Hz = 0;
    _points.clear();
}
