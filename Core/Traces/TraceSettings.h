#ifndef TRACESETTINGS_H
#define TRACESETTINGS_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QString>
#include <QRegExp>
#include <QDataStream>


class TraceSettings
{
public:
    TraceSettings();
    ~TraceSettings();

    static QRegExp nameRegex;

    bool isValid();
    QString name;
    QString yParameter; // S11, S21, S12, S22, Pin, Pout, AMPM
    QString xParameter; // Frequency, Pin, Pout
    QString atParameter; // Frequency, Pin, Compression, Maximum Gain
    double atValue; // if atParameter != Compression or Maximum Gain

    // Name
    bool isValidName(); // Trims as well
    void generateNameFromSettings();

    // Y Parameter
    bool isYS11Trace() const;
    bool isYS22Trace() const;
    bool isYS21Trace() const;
    bool isYS12Trace() const;
    bool isYPin() const;
    bool isYPout() const;

    bool isYSParameter() const;
    bool isYReflection() const;
    bool isYInsertion() const;
    bool isYPower() const;
    bool isYAmPm() const;
    bool isValidYParameter() const;
    QStringList possibleYParameters() const;

    // X Parameter
    bool isXFrequency() const;
    bool isXPower() const;
    bool isXPin() const;
    bool isXPout() const;
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

bool operator==(const TraceSettings &trace1, const TraceSettings &trace2);
QDataStream &operator<<(QDataStream &stream, const TraceSettings &settings);
QDataStream &operator>>(QDataStream &stream, TraceSettings &settings);


#endif // TRACESETTINGS_H
