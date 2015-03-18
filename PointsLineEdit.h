#ifndef POINTSLINEEDIT_H
#define POINTSLINEEDIT_H


// Qt
#include <QLineEdit>

class PointsLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit PointsLineEdit(QWidget *parent = 0);
    ~PointsLineEdit();

    uint points() const;

signals:
    void pointsChanged(uint points);

public slots:
    void setPoints(uint points);

private slots:
    void processTextChanged();

private:
    uint _points;

};

#endif // POINTSLINEEDIT_H
