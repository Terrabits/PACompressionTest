#ifndef TRACESETTINGS_H
#define TRACESETTINGS_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QString>
#include <QRegExp>


class TraceSettings
{
public:
    TraceSettings();
    ~TraceSettings();

    static QRegExp nameRegex;

    bool isValid();
    QString name;
    QString yParameter; // Input Reflection, Gain, Reverse Gain, Output Reflection, Pin, Pout
    QString xParameter; // Frequency, Pin
    QString atParameter; // Frequency, Pin, Compression, Maximum Gain
    double atValue; // if atParameter != Compression

    // Name
    bool isValidName(); // Trims as well
    void generateNameFromSettings();

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