#ifndef TRACESWIDGET_H
#define TRACESWIDGET_H


// Project
#include "TraceSettings.h"
#include "TraceSettingsModel.h"
#include "TraceSettingsDelegate.h"

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

    QVector<TraceSettings> traces() const;

signals:
    void tracesChanged();

private slots:
    void on_add_clicked();

    void on_remove_clicked();

private:
    Ui::TracesWidget *ui;

    TraceSettingsModel _model;
    TraceSettingsDelegate _delegate;
};

#endif // TRACESWIDGET_H
