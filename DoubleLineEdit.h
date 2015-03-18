#ifndef DOUBLELINEEDIT_H
#define DOUBLELINEEDIT_H

#include <QLineEdit>

class DoubleLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit DoubleLineEdit(QWidget *parent = 0);
    ~DoubleLineEdit();

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

#endif // DOUBLELINEEDIT_H
