#ifndef MEASURETHREAD_H
#define MEASURETHREAD_H


// PA Compression Test
#include "MeasurementData.h"

// RsaToolbox
#include "Definitions.h"
#include "Vna.h"

// QCustomPlot
#include <qcustomplot.h>

// Qt
#include <QThread>


class MeasureThread : public QThread {
    Q_OBJECT

public:
    MeasureThread(QObject *parent = 0);
    ~MeasureThread();

    void setAppInfo(const QString &name, const QString &version);
    void setVna(RsaToolbox::Vna *vna);
    void setSettings(const MeasurementSettings &settings);

    bool isError() const;
    QString errorMessage() const;
    void start(Priority priority = InheritPriority);
    MeasurementData *takeResults();

signals:
    void plotMaxGain(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &pin_dB);
    void progress(int percent);

protected:
    QString _appName;
    QString _appVersion;
    RsaToolbox::Vna *_vna;
    MeasurementSettings _settings;

    // Overwrite this:
    // virtual void run();

    bool _isError;
    QString _error;

    // Initialized in start();
    QScopedPointer<MeasurementData> _results;

    void clearError();
    void setError(QString message = QString());

    void flipPorts();
    static void flipPorts(RsaToolbox::NetworkData &data);
    static void flipPorts(QVector<RsaToolbox::NetworkData> &data);

    void freezeChannels();
    void unfreezeChannels();

    void displayResultsOnInstrument();

    void setupChannel(uint channelIndex, const RsaToolbox::QRowVector &frequencies_Hz, const RsaToolbox::QRowVector &powers_dBm);
    void resizeChannel(uint channelIndex, uint points);

    void createMaxGainTrace(uint diagram);
    void createCompressedGainTrace(uint diagram);
    void createPinCompressionTrace(uint diagram);
    void createPoutCompressionTrace(uint diagram);
    void configureDiagram(uint diagram);

private:
    QVector<uint> _channels;
    QBitArray _isContinuous;
};


#endif // MEASURETHREAD_H
