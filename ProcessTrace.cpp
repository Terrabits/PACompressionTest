#include "ProcessTrace.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>


ProcessTrace::ProcessTrace(TraceSettings &settings, MeasurementData &data) :
    _settings(settings),
    _data(data)
{

}

ProcessTrace::~ProcessTrace()
{

}

QString ProcessTrace::name() {
    QString s = _settings.yParameter;
    s += " @ ";
    if (_settings.isAtValue()) {
        if (_settings.isAtFrequency())
            s += formatValue(_settings.atValue, 3, Units::Hertz);
        else if (_settings.isAtPin())
            s += formatValue(_settings.atValue, 2, Units::dBm);
        else
            s += formatValue(_settings.atValue, 3, Units::NoUnits);
    }
    else {
        s += _settings.atParameter;
    }
    return s;
}
QString ProcessTrace::yUnits() {
    if (_settings.isYdB())
        return "dB";
    if (_settings.isYmag())
        return "Mag";
    if (_settings.isYdeg())
        return "Deg";
    if (_settings.isYrad())
        return "Rad";
    if (_settings.isYvswr())
        return "VSWR";
    if (_settings.isYdBm())
        return "dBm";
    // Else
    return "";
}
bool ProcessTrace::isXFrequency() {
    return _settings.isXFrequency();
}

QString ProcessTrace::xUnits() {
    if (_settings.isXFrequency())
        return "Hz";
    if (_settings.isXPin())
        return "dBm";
    //Else
    return Units::NoUnits;
}

QRowVector ProcessTrace::x() {
    qDebug() << "ProcessTrace::x()";
    if (_settings.isXPin()) {
        qDebug() << "  Pin.size: " << _data.power_dBm.size();
        return _data.power_dBm;
    }
    if (_settings.isXFrequency()) {
        qDebug() << "  frequencies.size: " << _data.frequencies_Hz.size();
        return _data.frequencies_Hz;
    }
    // Else
    qDebug() << "No X value?";
    return QRowVector();
}
QRowVector ProcessTrace::y() {
    qDebug() << "ProcessTrace::y()";
    // atValue, atIndex
    int atIndex = 0;
    if (_settings.isAtPin()) {
        qDebug() << "Rounding at Pin value...";
        _settings.roundAtValue(_data.power_dBm);
        qDebug() << "Retrieving indexOf...";
        atIndex = _data.power_dBm.indexOf(_settings.atValue);
    }
    else if (_settings.isAtFrequency()) {
        qDebug() << "Rouding at Frequency value...";
        _settings.roundAtValue(_data.frequencies_Hz);
        atIndex = _data.frequencies_Hz.indexOf(_settings.atValue);
    }

    qDebug() << "at index: " << atIndex;
    if (atIndex < 0)
        return QRowVector(); // ?

    if (_settings.isYPower()) {
        if (_settings.isXFrequency()) {
            if (_settings.isYPin()) {
                if (_settings.isAtCompression()) {
                    // y: Pin
                    // x: Frequency
                    // @: Compression
                    return _data.powerInAtCompression_dBm;
                }
                else if (_settings.isAtMaximumGain()) {
                    // y: Pin
                    // x: Frequency
                    // @: Maximum Gain
                    QRowVector v(_data.frequencyPoints);
                    for (int i = 0; i < v.size(); i++) {
                        const int index = _data.referenceGainIndexes[i];
                        v[i] = _data.power_dBm[index];
                    }
                    return v;
                }
            }
            else {
                if (_settings.isAtPin()) {
                    // y: Pout
                    // x: Frequency
                    // @: Pin = atIndex
                    QRowVector v(_data.frequencyPoints);
                    for (int i = 0; i < v.size(); i++) {
                        v[i] = _data.powerOut_dBm[i][atIndex];
                    }
                    return v;
                }
                else if (_settings.isAtCompression()) {
                    // y: Pout
                    // x: Frequency
                    // @: Compression
                    return _data.powerOutAtCompression_dBm;
                }
                else if (_settings.isAtMaximumGain()) {
                    // y: Pout
                    // x: Frequency
                    // @: Maximum Gain
                    QRowVector v(_data.frequencyPoints);
                    for (int i = 0; i < v.size(); i++) {
                        const int index = _data.referenceGainIndexes[i];
                        v[i] = _data.powerOut_dBm[i][index];
                    }
                    return v;
                }
            }
        }
        else {
            if (_settings.isAtFrequency()) {
                // y: Pout
                // x: Pin
                // @: Frequency = atIndex
                qDebug() << "  y: Pout";
                qDebug() << "  x: Pin";
                qDebug() << "  @: Frequency index: " << atIndex;
                qDebug() << "  y.size: " << _data.powerOut_dBm[atIndex].size();
                qDebug() << "  y[0]: " << _data.powerOut_dBm[atIndex].first();
                qDebug() << "  y[-1]: " << _data.powerOut_dBm[atIndex].last();
                return _data.powerOut_dBm[atIndex];
            }
        }
    }
    else {
        // SParameters

        // Ports
        uint input, output;
        if (_settings.isYInputReflectionTrace()) {
            qDebug() << "input reflection ports...";
            input = output = 0;
        }
        else if (_settings.isYOutputReflectionTrace()) {
            input = output = 1;
        }
        else if (_settings.isYGainTrace()) {
            input = 0;
            output = 1;
        }
        else {
            // Reverse Gain
            input = 1;
            output = 0;
        }

        ComplexRowVector result;
        if (_settings.isXFrequency()) {
            if (_settings.isAtPin()) {
                // y: Sxy
                // x: Frequency
                // @: Pin = atValue
                qDebug() << "Returning Sxy vs Freq @ Pin...";
                result = _data.data[atIndex].y(output+1, input+1);
            }
            else if (_settings.isAtCompression()) {
                // y: Sxy
                // x: Frequency
                // @: Compression
                result.resize(_data.frequencyPoints);
                for (uint i = 0; i < result.size(); i++) {
                    result[i] = _data.s_compression[i][output][input];
                }
            }
            else if (_settings.isAtMaximumGain()) {
                // y: Sxy
                // x: Frequency
                // @: Maximum Gain
                result.resize(_data.frequencyPoints);
                for (uint i = 0; i < result.size(); i++) {
                    result[i] = _data.s_referenceGain[i][output][input];
                }
            }
        }
        else {
            // y: SParameter
            // x: Pin
            // @: Frequency = atValue
            result.resize(_data.powerPoints);
            for (uint i = 0; i < result.size(); i++) {
                result[i] = _data.data[i].y()[atIndex][output][input];
            }
        }

        // Format
        if (_settings.isYdB())
            return toDb(result);
        else if (_settings.isYmag())
            return toMagnitude(result);
        else if (_settings.isYdeg())
            return angle_deg(result);
        else if (_settings.isYrad())
            return angle_rad(result);
        else if (_settings.isYvswr())
            return toVswr(result);
    }

    qDebug() << "No Y values?";
    return QRowVector();
}

