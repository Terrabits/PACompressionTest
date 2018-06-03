#ifndef MEASURETHREAD_H
#define MEASURETHREAD_H


// PA Compression Test
#include "MeasurementData.h"

// Dmm
#include <Dmm/stagesettings.h>
#include <Dmm/dmmcontroller.h>

// RsaToolbox
#include <Definitions.h>
#include <Vna.h>
#include <VnaUndo.h>

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
    //dmm
    void setDmmStages(const QVector<dmm::StageSettings> &stages, double delay_s);

    bool isError() const;
    QString errorMessage() const;
    void start(Priority priority = InheritPriority);
    MeasurementData *takeResults();

signals:
    void plotMaxGain(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &gain_dB);
    void plotPinAtCompression(const RsaToolbox::QRowVector &frequency_Hz, const RsaToolbox::QRowVector &pin_dB);

    void startingSweep(const QString &caption, uint sweepTime_ms);
    void finishedSweep();

    void progress(int percent);

protected:

    // Overwrite this:
    // virtual void run();


    QString _appName;
    QString _appVersion;
    MeasurementSettings _settings;

    // Initialized in start();
    QScopedPointer<MeasurementData> _results;

    RsaToolbox::Vna *_vna;
    dmm::DmmController _dmms;

    bool _isError;
    QString _error;
    void clearError();
    void setError(QString message = QString());

    void flipPorts();
    static void flipPorts(RsaToolbox::NetworkData &data);
    static void flipPorts(QVector<RsaToolbox::NetworkData> &data);

    uint _measurementChannel;
    QVector<uint> sourcesInChannel();
    bool prepareVna();
    void restoreVna();

private:
    QVector<uint> _continuousChannels;
};


#endif // MEASURETHREAD_H
