#ifndef TRACESWIDGET_H
#define TRACESWIDGET_H


// Project
#include "TraceSettings.h"
#include "TraceSettingsModel.h"
#include "TraceSettingsDelegate.h"

// RsaToolbox
#include <Definitions.h>
#include <Keys.h>
#include <ErrorLabel.h>

// Qt
#include <QVector>
#include <QWidget>


namespace Ui {
class TracesWidget;
}

class TracesWidget : public QWidget
{
    Q_OBJECT
    friend class TraceSettingsDelegate;

public:
    explicit TracesWidget(QWidget *parent = 0);
    ~TracesWidget();

    void setKeys(RsaToolbox::Keys *keys);
    void loadKeys();
    void saveKeys() const;

    void setFrequencies(const RsaToolbox::QRowVector &frequencies_Hz);
    void setPowers(const RsaToolbox::QRowVector &powers_dBm);

    bool isTracesValid();
    QVector<TraceSettings> traces() const;
    void setTraces(const QVector<TraceSettings> &traces);

    RsaToolbox::ErrorLabel *errorLabel();

signals:
    void exportClicked();
    void closeClicked();
    void plotClicked();

    void inputError(const QString &message);
    void tracesChanged();

public slots:
    void enableExportAndPlot();
    void disableExportAndPlot();

protected slots:
    void delegateError(const QString &message);

private slots:
    void on_add_clicked();
    void on_remove_clicked();

private:
    Ui::TracesWidget *ui;

    mutable RsaToolbox::Keys *_keys;

    TraceSettingsModel _model;
    TraceSettingsDelegate _delegate;

    void setFixedColumnWidths();
};

#endif // TRACESWIDGET_H
