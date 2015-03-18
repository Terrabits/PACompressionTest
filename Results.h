#ifndef RESULTS_H
#define RESULTS_H


// RsaToolbox
#include "General.h"
#include "NetworkData.h"

// Qt
#include <QVector>
#include <QObject>


class Results : public QObject
{
    Q_OBJECT
public:
    explicit Results(QObject *parent = 0);
    ~Results();

signals:

public slots:

private:
    double _compressionValue;

    RsaToolbox::QRowVector _power_dBm;
    QVector<RsaToolbox::NetworkData> _data;
};

#endif // RESULTS_H
