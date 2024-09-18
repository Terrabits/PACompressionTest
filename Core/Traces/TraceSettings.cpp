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


    // add at value?
    if (isAtValue()) {

        _name += "_";

        if (isAtFrequency()) {
            _name += formatValue(atValue, 3, Units::Hertz);
        }
        else if (isAtPin()) {
            _name += formatDouble(atValue, 3);
            _name += "_dBm";
        }
    }


    // Clean up incompatible characters
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


bool TraceSettings::isYCurrent() const {
  return yParameter.compare("Current", Qt::CaseInsensitive) == 0;
}


bool TraceSettings::isYVoltage() const {
  return yParameter.compare("Voltage", Qt::CaseInsensitive) == 0;
}


bool TraceSettings::isYPowerAddedEfficiency() const {
  return yParameter.compare("Power Added Efficiency", Qt::CaseInsensitive) == 0;
}


bool TraceSettings::isYDrainEfficiency() const {
  return yParameter.compare("Drain Efficiency", Qt::CaseInsensitive) == 0;
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


bool TraceSettings::isYAmPm() const {
    return yParameter.compare("AMPM", Qt::CaseInsensitive) == 0;
}


bool TraceSettings::isYPaeRelated() const {

    if (isYVoltage()) {
        return true;
    }


    if (isYCurrent()) {
        return true;
    }


    if (isYPowerAddedEfficiency()) {
        return true;
    }


    if (isYDrainEfficiency()) {
        return true;
    }


    // not pae related
    return false;
}


bool TraceSettings::isValidYParameter() const {
    return possibleYParameters().contains(yParameter, Qt::CaseInsensitive);
}
QStringList TraceSettings::possibleYParameters(bool isPae) const {
    QStringList list;
    list << "S11"
         << "S21"
         << "S12"
         << "S22"
         << "Pin" // TODO: valid?
         << "Pout"
         << "AMPM";

    if (isPae) {
      list << "Current"
           << "Voltage"
           << "Power Added Efficiency"
           << "Drain Efficiency";
    }

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


    // y valid?
    if (!isValidYParameter()) {
        return list;
    }


    // y is s parameter?
    if (isYSParameter()) {
      list << "Frequency"
           << "Pin"
           << "Pout";
      return list;
    }


    // y ampm?
    if (isYAmPm()) {
        list << "Pin"
             << "Pout";
        return list;
    }


    // y pout?
    if (isYPout()) {
        list << "Frequency"
             << "Pin";
        return list;
    }


    // y pin?
    if (isYPin()) {
        list << "Frequency";
        return list;
    }


    // voltage, current, pae, de?
    if (isYPaeRelated()) {
        list << "Frequency"
             << "Pin"
             << "Pout";
        return list;
    }


    // this should never happen...
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

    // y, x valid?
    if (!isValidYParameter() || !isValidXParameter()) {
        return list;
    }


    // y s parameter?
    if (isYSParameter()) {

        if (isXFrequency()) {
            // y: SParam
            // x: Frequency
            list << "Compression"
                 << "Maximum Gain"
                 << "Pin";
            return list;
        }


        if (isXPower()) {
            // y: SParam
            // x: Pin, pout
            list << "Frequency";
            return list;
        }


        // this should never happen...
        return list;
    }


    // y is ampm?
    if (isYAmPm()) {

        // x must be power
        if (!isXPower()) {
            return list;
        }


        // y: AMPM
        // x: Pin, Pout
        list << "Frequency";
        return list;
    }

    // y is Pin?
    if (isYPin()) {

        // x must be frequency
        if (!isXFrequency()) {
            return list;
        }


        // y: Pin
        // x: Frequency
        list << "Compression"
             << "Maximum Gain";
        return list;
    }


    // y is Pout
    if (isYPout()) {


        if (isXFrequency()) {
            // y: Pout
            // x: Frequency
             list << "Pin"
                  << "Compression"
                  << "Maximum Gain";
            return list;
        }


        if (isXPin()) {
            // y: Pout
            // x: Pin
            list << "Frequency";
            return list;
        }

        // this should never happen...
        return list;
    }


    // y is current, voltage, pae, de?
    if (isYPaeRelated()) {

        if (isXFrequency()) {
            // y: pae-related
            // x: frequency
            list << "Compression"
                 << "Maximum Gain"
                 << "Pin";
            return list;
        }


        if (isXPower()) {
            // y: pae-related
            // x: pin, pout
            list << "Frequency";
            return list;
        }


        // this should never happen...
        return list;
    }


    // this should never happen...
    return list;
}


bool TraceSettings::isAtValue() const {

    if (!isValidAtParameter()) {
        return false;
    }


    if (isAtFrequency() || isAtPin()) {
        return true;
    }

    // default
    return false;
}


bool TraceSettings::isValidAtValue() const {

    // valid y?
    if (!isValidYParameter()) {
          return false;
    }


    // valid x?
    if (!isValidXParameter()) {
        return false;
    }


    // valid at parameter?
    if (!isValidAtParameter()) {
        return false;
    }


    // validate frequency?
    if (isAtFrequency()) {
        return atValue >= 0;
    }


    // power in dBm; assume valid
    return true;
}


void TraceSettings::roundAtValue(RsaToolbox::QRowVector values) {

    // check bounds

    // at value should be first value?
    if (atValue <= values.first()) {
        atValue = values.first();
        return;
    }


    // at value should be last value?
    if (atValue >= values.last()) {
        atValue = values.last();
        return;
    }


    // find index of value greater than at value
    int highIndex;
    for (highIndex = 0; highIndex < values.size(); highIndex++) {
        if (atValue > values[highIndex]) {
            break;
        }
    }


    // low index, value
    const int    lowIndex  = highIndex - 1;
    const double lowValue  = values[lowIndex];
    const double lowDiff   = atValue - lowValue;


    // high value
    const double highValue = values[highIndex];
    const double highDiff  = highValue - atValue;


    // closest value
    const int closestValueIndex = highDiff < lowDiff ? highIndex : lowIndex;
    atValue = values[closestValueIndex];
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
