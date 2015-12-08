#include "TraceSettings.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QRegExp>
#include <QTextStream>
#include <QDebug>

QRegExp TraceSettings::nameRegex("^[a-z_][0-9a-z_]*$", Qt::CaseInsensitive);

TraceSettings::TraceSettings() :
    yParameter("Gain"),
    xParameter("Frequency"),
    atParameter("Compression"),
    atValue(0)
{
    generateNameFromSettings();
}

TraceSettings::~TraceSettings()
{

}

bool TraceSettings::isValid() {
    return isValidName()
            && isValidYParameter()
            && isValidXParameter()
            && isValidAtParameter();
}

// Name
bool TraceSettings::isValidName() {
    this->name = name.trimmed();
    return !name.isEmpty() && name.contains(nameRegex);
}
void TraceSettings::generateNameFromSettings() {
    QString _name = "%1_at_%2";
    _name = _name.arg(yParameter);
    _name = _name.arg(atParameter);

    if (isAtValue()) {
        _name += "_";
        if (isAtFrequency()) {
            _name += formatValue(atValue, 3, Units::Hertz);
        }
        else {
            _name += formatDouble(atValue, 3);
            _name += "_dBm";
        }
    }

    // Clean up
    _name.replace(" ", "_");
    _name.replace("-", "neg");
    _name.replace(".", "_");
    name = _name;
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
    if (!isYPin()) {
        list << "Pin";
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

    if (isAtFrequency())
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
