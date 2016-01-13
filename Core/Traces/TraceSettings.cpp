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
    yParameter("S21"),
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
bool TraceSettings::isYS11Trace() const {
    return yParameter.compare("S11", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYS22Trace() const {
    return yParameter.compare("S22", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYS21Trace() const {
    return yParameter.compare("S21", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYS12Trace() const {
    return yParameter.compare("S12", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYPin() const {
    return yParameter.compare("Pin", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isYPout() const {
    return yParameter.compare("Pout", Qt::CaseInsensitive) == 0;
}

bool TraceSettings::isYSParameter() const {
    return isYReflection() || isYInsertion();
}
bool TraceSettings::isYReflection() const {
    return isYS11Trace() || isYS22Trace();
}
bool TraceSettings::isYInsertion() const {
    return isYS21Trace() || isYS12Trace();
}
bool TraceSettings::isYPower() const {
    return isYPin() || isYPout();
}
bool TraceSettings::isValidYParameter() const {
    return possibleYParameters().contains(yParameter, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleYParameters() const {
    QStringList list;
    list << "S11"
         << "S21"
         << "S12"
         << "S22"
         << "Pin"
         << "Pout";
    return list;
}

// X Parameter
bool TraceSettings::isXFrequency() const {
    return xParameter.compare("Frequency", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isXPower() const {
    return isXPin() || isXPout();
}
bool TraceSettings::isXPin() const {
    return xParameter.compare("Pin", Qt::CaseInsensitive) == 0;
}
bool TraceSettings::isXPout() const {
    return xParameter.compare("Pout", Qt::CaseInsensitive) == 0;
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
    if (isYSParameter()) {
        list << "Pout";
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
    if (isYSParameter()) {
        if (isXPin()) {
            // y: SParam
            // x: Pin
            list << "Frequency";
        }
        else if (isXPout()) {
            // y: SParam
            // x: Pout
            list << "Frequency";
        }
        else {
            // y: SParam
            // x: Frequency
            list << "Pin"
//                 << "Pout"
                 << "Compression"
                 << "Maximum Gain";
        }
    }
    else {
        // Y is power
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

bool operator==(const TraceSettings &trace1, const TraceSettings &trace2) {
    if (trace1.name != trace2.name)
        return false;
    if (trace1.yParameter != trace2.yParameter)
        return false;
    if (trace1.xParameter != trace2.xParameter)
        return false;
    if (trace1.atParameter != trace2.atParameter)
        return false;
    if (trace1.atValue != trace2.atValue)
        return false;

    return true;
}

QDataStream &operator<<(QDataStream &stream, const TraceSettings &settings) {
    stream << settings.name;
    stream << settings.yParameter;
    stream << settings.xParameter;
    stream << settings.atParameter;
    stream << settings.atValue;
    return stream;
}
QDataStream &operator>>(QDataStream &stream, TraceSettings &settings) {
    stream >> settings.name;
    stream >> settings.yParameter;
    stream >> settings.xParameter;
    stream >> settings.atParameter;
    stream >> settings.atValue;
    return stream;
}

