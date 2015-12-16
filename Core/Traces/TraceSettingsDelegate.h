#ifndef TRACESETTINGSDELEGATE_H
#define TRACESETTINGSDELEGATE_H


// RsaToolbox
#include <Definitions.h>

// Qt
#include <QStyledItemDelegate>


class TraceSettingsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TraceSettingsDelegate(QObject *parent = 0);
    ~TraceSettingsDelegate();

    void setFrequencies(const RsaToolbox::QRowVector &frequencies_Hz);
    void setPowers(const RsaToolbox::QRowVector &powers_dBm);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;


private:
    RsaToolbox::QRowVector _frequencies_Hz;
    RsaToolbox::QRowVector _powers_dBm;
};

#endif // TRACESETTINGSDELEGATE_H
