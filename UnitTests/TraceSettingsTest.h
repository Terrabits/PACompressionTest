#ifndef TRACESETTINGSTEST_H
#define TRACESETTINGSTEST_H

#include <QObject>
#include <QDir>

class TraceSettingsTest : public QObject
{
    Q_OBJECT
public:
    explicit TraceSettingsTest(QObject *parent = 0);
    ~TraceSettingsTest();

private slots:

    void setNameFromSettings_data();
    void setNameFromSettings();

    void save();

private:
    QDir _dir;
};

#endif // TRACESETTINGSTEST_H
