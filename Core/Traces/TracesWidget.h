#ifndef TRACESWIDGET_H
#define TRACESWIDGET_H


// Project
#include "TraceSettings.h"
#include "TraceSettingsModel.h"
#include "TraceSettingsDelegate.h"

// RsaToolbox
#include <Definitions.h>

// Qt
#include <QVector>
#include <QWidget>


namespace Ui {
class TracesWidget;
}

class TracesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TracesWidget(QWidget *parent = 0);
    ~TracesWidget();

    void setFrequencies(const RsaToolbox::QRowVector &frequencies_Hz);
    void setPowers(const RsaToolbox::QRowVector &powers_dBm);

    bool isTracesValid();
    bool isTracesValid(QString &message) const;
    QVector<TraceSettings> traces() const;
    void setTraces(const QVector<TraceSettings> &traces);

signals:
    void error(const QString &message);
    void tracesChanged();

private slots:
    void on_add_clicked();
    void on_remove_clicked();

private:
    Ui::TracesWidget *ui;

    TraceSettingsModel _model;
    TraceSettingsDelegate _delegate;

    void setFixedColumnWidths();
};

#endif // TRACESWIDGET_H
