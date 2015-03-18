#ifndef POSITIVEDOUBLELINEEDIT_H
#define POSITIVEDOUBLELINEEDIT_H

#include <QLineEdit>

class PositiveDoubleLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit PositiveDoubleLineEdit(QWidget *parent = 0);
    ~PositiveDoubleLineEdit();

    double value() const;

signals:
    void valueChanged(double value);

public slots:
    void setValue(double value);

private slots:
    void processTextChanged();

private:
    double _value;
};

#endif // POSITIVEDOUBLELINEEDIT_H
