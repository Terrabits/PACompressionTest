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
    void setProgressPlot(QCustomPlot *plot);

    bool isError() const;
    QString errorMessage() const;
    void start(Priority priority = InheritPriority);
    MeasurementData *takeResults();

signals:
    void progress(int percent);
    void finished();

protected:
    QString _appName;
    QString _appVersion;
    QCustomPlot *_plot;
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

private:
    //
};


#endif // MEASURETHREAD_H
