#ifndef TESTPLAN_H
#define TESTPLAN_H


// RsaToolbox
#include "General.h"
#include "Vna.h"

// Qt
#include <QObject>

class TestPlan : public QObject
{
    Q_OBJECT

public:
    explicit TestPlan(QObject *parent = 0);
    TestPlan(const TestPlan &other);
    ~TestPlan();
    
    // If Vna is provided,
    // error checking will occur
    bool isVna() const;
    RsaToolbox::Vna *vna() const;
    void setVna(RsaToolbox::Vna *vna);
    
    bool isValid();

    double startFrequency_Hz() const;
    double stopFrequency_Hz() const;
    uint frequencyPoints() const;

    double startPower_dBm() const;
    double powerStepSize_dBm() const;
    double absoluteStopPower_dBm() const;

    bool isGainExpansion() const;

    double compressionValue_dB() const;

    void operator=(const TestPlan &other);

signals:
    void vnaChanged(RsaToolbox::Vna *vna);
    
    void startFrequencyChanged(double frequency_Hz);
    void stopFrequencyChanged(double frequency_Hz);
    void frequencyPointsChanged(uint points);

    void startPowerChanged(double power_dBm);
    void absoluteStopPowerChanged(double power_dBm);
    void powerStepSizeChanged(double stepSize_dBm);

    void gainExpansionChanged(bool isOn);

    void compressionValueChanged(double value_dB);

    void reset();
    void error(const QString &message);

public slots:
    bool setStartFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);
    bool setStopFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);
    bool setFrequencyPoints(uint points);

    bool setStartPower(double power_dBm);
    bool setPowerStepSize(double stepSize_dBm);
    bool setAbsoluteStopPower(double power_dBm);

    bool setGainExpansion(bool on);

    bool setCompressionValue(double value_dB);

private:
    void initialize();
    RsaToolbox::Vna *_vna;
    
    bool isFrequencyValid(double hz);
    bool isPointsValid(uint points);
    double _startFrequency_Hz;
    double _stopFrequency_Hz;
    uint _frequencyPoints;

    bool isPowerValid(double dBm);
    bool isStepSizeValid(double dBm);
    double _startPower_dBm;
    double _powerStepSize_dBm;
    double _absoluteStopPower_dBm;

    bool isCompressionValueValid(double dB);
    double _compression_dB;

    bool _isGainExpansion;

//    double _maximumOvershoot_dBm;

    void setEqual(const TestPlan &other);

};

#endif // TESTPLAN_H
