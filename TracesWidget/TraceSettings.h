#ifndef TRACESETTINGS_H
#define TRACESETTINGS_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QString>


class TraceSettings
{
public:
    TraceSettings();
    ~TraceSettings();

    bool isValid() const;
    QString yAxis; // Left, Right
    QString yParameter; // Input Reflection, Gain, Reverse Gain, Output Reflection, Pin, Pout
    QString yFormat; // dB, mag, deg, rad, VSWR, dBm
    QString xParameter; // Frequency, Pin
    QString atParameter; // Frequency, Pin, Compression, Maximum Gain
    double atValue; // if atParameter != Compression

    // Y Axis
    bool isLeftYAxis() const;
    bool isRightYAxis() const;
    bool isValidYAxis() const;
    QStringList possibleYAxis() const;

    // Y Parameter
    bool isYInputReflectionTrace() const;
    bool isYOutputReflectionTrace() const;
    bool isYGainTrace() const;
    bool isYReverseGainTrace() const;
    bool isYPin() const;
    bool isYPout() const;


    bool isYReflection() const;
    bool isYInsertion() const;
    bool isYPower() const;
    bool isValidYParameter() const;
    QStringList possibleYParameters() const;

    // Y Format
    bool isYdB() const;
    bool isYmag() const;
    bool isYdeg() const;
    bool isYrad() const;
    bool isYvswr() const;
    bool isYdBm() const;
    bool isValidYFormat() const;
    QStringList possibleYFormats() const;

    // X Parameter
    bool isXFrequency() const;
    bool isXPin() const;
    bool isValidXParameter() const;
    QStringList possibleXParameters() const;

    // At
    bool isAtFrequency() const;
    bool isAtPin() const;
    bool isAtCompression() const;
    bool isAtMaximumGain() const;
    bool isValidAtParameter() const;
    QStringList possibleAtParameters() const;

    bool isAtValue() const;
    bool isValidAtValue() const;
    void roundAtValue(RsaToolbox::QRowVector values);
};

#endif // TRACESETTINGS_H
