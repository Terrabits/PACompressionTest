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
    ~TestPlan();
    
    // If Vna is provided,
    // error checking will occur
    bool isVna() const;
    RsaToolbox::Vna vna() const;
    void setVna(RsaToolbox::Vna *vna);
    
    bool isReady() const;

    double startFrequency_Hz() const;
    void setStartFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);

    double stopFrequency_Hz() const;
    void setStopFrequency(double frequency, RsaToolbox::SiPrefix prefix = RsaToolbox::SiPrefix::None);

    uint frequencyPoints() const;
    void setFrequencyPoints(uint points);

    double startPower_dBm() const;
    void setStartPower(double power_dBm);

    double powerStepSize_dBm() const;
    void setPowerStepSize(double stepSize_dBm);

    double absoluteStopPower_dBm() const;
    void setAbsoluteStopPower(double power_dBm);

    double compressionPoint_dB() const;
    void setCompressionPoint(double value_dB = 1.0);

signals:
    void vnaChanged(RsaToolbox::Vna *vna);
    
    void startFrequencyChanged(double frequency_Hz);
    void stopFrequencyChanged(double frequency_Hz);
    void frequencyPointsChanged(uint points);

    void startPowerChanged(double power_dBm);
    void absoluteStopPowerChanged(double power_dBm);
    void powerStepSizeChanged(double stepSize_dBm);

    void compressionPointChanged(double value_dB);

    void reset();
    
    void error(const QString &message);

public slots:

private:
    void initialize();
    RsaToolbox::Vna *_vna;
    
    double _startFrequency_Hz;
    double _stopFrequency_Hz;
    uint _frequencyPoints;

    double _startPower_dBm;
    double _powerStepSize_dBm;
    double _absoluteStopPower_dBm;

    double _compression_dB;

//    double _maximumOvershoot_dBm;

};

#endif // TESTPLAN_H
