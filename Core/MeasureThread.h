#ifndef MEASURETHREAD_H
#define MEASURETHREAD_H


// PA Compression Test
#include "MeasurementData.h"

// RsaToolbox
#include "Definitions.h"
#include "Vna.h"

// Qt
#include <QThread>


class MeasureThread : public QThread {
    Q_OBJECT

public:
    MeasureThread(QObject *parent = 0);
    ~MeasureThread();

    void setVna(RsaToolbox::Vna &vna);
    void setSettings(const MeasurementSettings &settings);

    MeasurementData &results();
    MeasurementData *takeResults();

signals:
    void progress(int percent);

protected:
    MeasurementData &data();

private slots:
    void run();

    static void flipPorts(QVector<RsaToolbox::NetworkData> &sweeps);

private:
    RsaToolbox::Vna *_vna;
    MeasurementSettings _settings;
    QScopedPointer<MeasurementData> _data;
    QVector<uint> _ports;
};


#endif // MEASURETHREAD_H
