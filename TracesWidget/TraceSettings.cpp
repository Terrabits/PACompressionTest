#include "TraceSettings.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QTextStream>
#include <QDebug>


TraceSettings::TraceSettings() :
    atValue(0)
{
    yAxis = "Left";
    yParameter = "Gain";
    yFormat = "dB";
    xParameter = "Frequency";
    atParameter = "Compression";
}

TraceSettings::~TraceSettings()
{

}

bool TraceSettings::isValid() const {
    return isValidYAxis()
            && isValidYParameter()
            && isValidYFormat()
            && isValidXParameter()
            && isValidAtParameter();
}

// Y Axis
bool TraceSettings::isLeftYAxis() const {
    return yAxis.compare("Left", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isRightYAxis() const {
    return yAxis.compare("Right", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isValidYAxis() const {
    return possibleYAxis().contains(yAxis, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleYAxis() const {
    QStringList list;
    list << "Left" << "Right";
    return list;
}

// Y Parameter
bool TraceSettings::isYInputReflectionTrace() const {
    return yParameter.compare("Input Reflection", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYOutputReflectionTrace() const {
    return yParameter.compare("Output Reflection", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYGainTrace() const {
    return yParameter.compare("Gain", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYReverseGainTrace() const {
    return yParameter.compare("Reverse Gain", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYPin() const {
    return yParameter.compare("Pin", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYPout() const {
    return yParameter.compare("Pout", Qt::CaseInsensitive) == 0;
}

bool TraceSettings::isYReflection() const {
    return isYInputReflectionTrace() || isYOutputReflectionTrace();
}
bool TraceSettings::isYInsertion() const {
    return isYGainTrace() || isYReverseGainTrace();
}
bool TraceSettings::isYPower() const {
    return isYPin() || isYPout();
}
bool TraceSettings::isValidYParameter() const {
    return possibleYParameters().contains(yParameter, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleYParameters() const {
    QStringList list;
    list << "Input Reflection"
         << "Gain"
         << "Reverse Gain"
         << "Output Reflection"
         << "Pin" << "Pout";
    return list;
}

// Y Format
bool TraceSettings::isYdB() const {
    return yFormat.compare("dB", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYmag() const {
    return yFormat.compare("mag", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYdeg() const {
    return yFormat.compare("deg", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYrad() const {
    return yFormat.compare("rad", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYvswr() const {
    return yFormat.compare("VSWR", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYdBm() const {
    return yFormat.compare("dBm", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isValidYFormat() const {
    return possibleYFormats().contains(yFormat, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleYFormats() const {
    QStringList list;
    if (!isValidYParameter()) {
        return list;
    }
    if (!isYPower()) {
        list << "dB"
             << "mag"
             << "deg"
             << "rad";
        if (isYReflection()) {
            list << "VSWR";
        }
    }
    else {
        list << "dBm";
    }
    return list;
}

// X Parameter
bool TraceSettings::isXFrequency() const {
    return xParameter.compare("Frequency", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isXPin() const {
    return xParameter.compare("Pin", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isValidXParameter() const {
    return possibleXParameters().contains(xParameter, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleXParameters() const {
    QStringList list;
    if (!isValidYParameter())
        return list;
    list << "Frequency";
    if (!isYPower()) {
        list << "Pin";
    }
    else {
        if (!isYPin()) {
          list << "Pin";
        }
    }
    return list;
}

// At Parameter
bool TraceSettings::isAtFrequency() const {
    return atParameter.compare("Frequency", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isAtPin() const {
    return atParameter.compare("Pin", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isAtCompression() const {
    return atParameter.compare("Compression", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isAtMaximumGain() const {
    return atParameter.compare("Maximum Gain", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isValidAtParameter() const {
    return possibleAtParameters().contains(atParameter, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleAtParameters() const {
    QStringList list;
    if (!isValidYParameter() || !isValidXParameter())
        return list;
    if (!isYPower()) {
        if (isXPin()) {
            // y: SParam
            // x: Pin
            list << "Frequency";
        }
        else {
            // y: SParam
            // x: Frequency
            list << "Pin"
                 << "Compression"
                 << "Maximum Gain";
        }
    }
    else {
        if (isYPin()) {
            // y: Pin
            // x: Frequency (default)
            list << "Compression"
                 << "Maximum Gain";
        }
        else {
            if (isXFrequency()) {
                // y: Pout
                // x: Frequency
                 list << "Pin"
                      << "Compression"
                      << "Maximum Gain";
            }
            else {
                // y: Pout
                // x: Pin
                list << "Frequency";
            }
        }
    }

    return list;
}

bool TraceSettings::isAtValue() const {
    if (!isValidAtParameter())
        return false;
    if (isAtFrequency() || isAtPin())
        return true;
    else
        return false;
}
bool TraceSettings::isValidAtValue() const {
    if (!isValidYParameter()
            || !isValidXParameter()
            || !isValidAtParameter())
        return false;

    if (isXFrequency())
        return atValue >= 0;

    // Else
    return true;
}
void TraceSettings::roundAtValue(RsaToolbox::QRowVector values) {
    if (atValue <= values.first()) {
        atValue = values.first();
        return;
    }
    if (atValue >= values.last()) {
        atValue = values.last();
        return;
    }

    int i = 0;
    while (atValue > values[i] && i < values.size() - 1) {
        i++;
    }

    if (abs(atValue - values[i]) < abs(atValue - values[i-1]))
        atValue = values[i];
    else
        atValue = values[i-1];
}
