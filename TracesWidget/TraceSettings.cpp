#include "TraceSettings.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QTextStream>
#include <QDebug>


TraceSettings::TraceSettings() :
    yAxis("Left"),
    color(Qt::black),
    yParameter("Gain"),
    yFormat("dB"),
    xParameter("Frequency"),
    atParameter("Compression"),
    atValue(0)
{

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

// Color
bool TraceSettings::isColor(Qt::GlobalColor color) const {
    return color == this->color;
}
QString TraceSettings::colorString() const {
    switch (color) {
    case Qt::white:
        return "White";
    case Qt::black:
        return "Black";
    case Qt::red:
        return "Red";
    case Qt::darkRed:
        return "Dark Red";
    case Qt::green:
        return "Green";
    case Qt::darkGreen:
        return "Dark Green";
    case Qt::blue:
        return "Blue";
    case Qt::darkBlue:
        return "Dark Blue";
    case Qt::cyan:
        return "Cyan";
    case Qt::darkCyan:
        return "Dark Cyan";
    case Qt::magenta:
        return "Magenta";
    case Qt::darkMagenta:
        return "Dark Magenta";
    case Qt::yellow:
        return "Yellow";
    case Qt::darkYellow:
        return "Dark Yellow";
    case Qt::gray:
        return "Gray";
    case Qt::darkGray:
        return "Dark Gray";
    case Qt::lightGray:
        return "Light Gray";
    case Qt::transparent:
        return "Transparent";
    case Qt::color0:
        return "Color 0";
    case Qt::color1:
        return "Color 1";
    default:
        return "Unknown color";
    }
}
bool TraceSettings::setColorFromString(QString colorString) {
    colorString = colorString.toUpper();
    if (colorString == "WHITE") {
        color = Qt::white;
        return true;
    }
    else if (colorString == "BLACK") {
        color = Qt::black;
        return true;
    }
    else if (colorString == "RED") {
        color = Qt::red;
        return true;
    }
    else if (colorString == "DARK RED") {
        color = Qt::darkRed;
        return true;
    }
    else if (colorString == "GREEN") {
        color = Qt::green;
        return true;
    }
    else if (colorString == "DARK GREEN") {
        color = Qt::darkGreen;
        return true;
    }
    else if (colorString == "BLUE") {
        color = Qt::blue;
        return true;
    }
    else if (colorString == "DARK BLUE") {
        color = Qt::darkBlue;
        return true;
    }
    else if (colorString == "CYAN") {
        color = Qt::cyan;
        return true;
    }
    else if (colorString == "DARK CYAN") {
        color = Qt::darkCyan;
        return true;
    }
    else if (colorString == "MAGENTA") {
        color = Qt::magenta;
        return true;
    }
    else if (colorString == "DARK MAGENTA") {
        color = Qt::darkMagenta;
        return true;
    }
    else if (colorString == "YELLOW") {
        color = Qt::yellow;
        return true;
    }
    else if (colorString == "DARK YELLOW") {
        color = Qt::darkYellow;
        return true;
    }
    else if (colorString == "GRAY") {
        color = Qt::gray;
        return true;
    }
    else if (colorString == "DARK GRAY") {
        color = Qt::darkGray;
        return true;
    }
    else if (colorString == "LIGHT GRAY") {
        color = Qt::lightGray;
        return true;
    }
    else if (colorString == "TRANSPARENT") {
        color = Qt::transparent;
        return true;
    }
    else if (colorString == "COLOR 0") {
        color = Qt::color0;
        return true;
    }
    else if (colorString == "COLOR 1") {
        color = Qt::color1;
        return true;
    }
    else {
        return false;
    }
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
