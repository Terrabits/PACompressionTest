#ifndef TRACESETTINGSMODEL_H
#define TRACESETTINGSMODEL_H


// Project
#include "TraceSettings.h"

// Qt
#include <QVector>
#include <QAbstractTableModel>

class TraceSettingsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TraceSettingsModel(QObject *parent = 0);
    ~TraceSettingsModel();

    enum Column {
        yAxis = 0,
        yParameter = 1,
        yFormat = 2,
        xParameter = 3,
        atParameter = 4,
        atValue = 5
    };
    static const int COLUMNS = 6;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QVector<TraceSettings> traces() const;

signals:

public slots:

private:
    QVector<TraceSettings> _traces;

    void fixTraceSettings(int row);
};

#endif // TRACESETTINGSMODEL_H
