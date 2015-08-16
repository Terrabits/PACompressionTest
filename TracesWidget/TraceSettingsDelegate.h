#ifndef TRACESETTINGSDELEGATE_H
#define TRACESETTINGSDELEGATE_H


// Qt
#include <QStyledItemDelegate>

class TraceSettingsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TraceSettingsDelegate(QObject *parent = 0);
    ~TraceSettingsDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;


signals:

public slots:
};

#endif // TRACESETTINGSDELEGATE_H
