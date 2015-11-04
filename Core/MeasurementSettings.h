#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H


// RsaToolbox
#include <Vna.h>

// Qt
#include <Qt>
#include <QDataStream>


class MeasurementSettings
{
public:
    MeasurementSettings();
    ~MeasurementSettings();

    double startFrequency_Hz() const;
    double stopFrequency_Hz() const;
    uint frequencyPoints() const;
    double ifBw_Hz() const;
    void setStartFrequency(double frequency_Hz);
    void setStopFrequency(double frequency_Hz);
    void setFrequencyPoints(uint points);
    void setIfBw(double bandwidth_Hz);

    double startPower_dBm() const;
    double stopPower_dBm() const;
    uint powerPoints() const;
    double powerStepSize() const; // Pseudo!!!
    double compressionLevel_dB() const;
    bool isGainExpansion() const;
    bool isStopAtCompression() const;
    bool isRfOffPostCondition() const;
    void setStartPower(double power_dBm);
    void setStopPower(double power_dBm);
    void setPowerPoints(uint points);
    void setCompressionLevel(double level_dB);
    void setGainExpansion(bool includeGainExpansion);
    void setStopAtCompression(bool stopAtCompression);
    void setRfOffPostCondition(bool rfOff);

    uint channel() const;
    uint outputPort() const;
    uint inputPort() const;
    void setChannel(uint index);
    void setOutputPort(uint port);
    void setInputPort(uint port);

    bool isValid(RsaToolbox::Vna &vna) const;
    bool isValid(RsaToolbox::Vna &vna, QString &errorMessage) const;

    void reset();

private:
    double _startFreq_Hz;
    double _stopFreq_Hz;
    uint _frequencyPoints;
    double _ifBw_Hz;

    double _startPower_dBm;
    double _stopPower_dBm;
    uint _powerPoints;
    double _compressionLevel_dB;
    bool _isGainExpansion;
    bool _isStopAtCompression;
    bool _isRfOffPostCondition;

    uint _channel;
    uint _inputPort;
    uint _outputPort;
};

QDataStream &operator>>(QDataStream &stream, MeasurementSettings &settings);
QDataStream &operator<<(QDataStream &stream, const MeasurementSettings &settings);

#endif // MEASUREMENTSETTINGS_H
