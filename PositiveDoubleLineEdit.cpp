#include "PositiveDoubleLineEdit.h"


// Qt
#include <QRegExp>
#include <QRegExpValidator>


PositiveDoubleLineEdit::PositiveDoubleLineEdit(QWidget *parent) : QLineEdit(parent)
{
    _value = 0;

    QRegExp regex("\\d+(?:\\.\\d*)?", Qt::CaseInsensitive);
    QRegExpValidator *validator = new QRegExpValidator(regex, this);
    setValidator(validator);

    connect(this, SIGNAL(textChanged(QString)),
            this, SLOT(processTextChanged()));
}

PositiveDoubleLineEdit::~PositiveDoubleLineEdit()
{

}

double PositiveDoubleLineEdit::value() const {
    return _value;
}

void PositiveDoubleLineEdit::setValue(double value) {
    if (value == _value)
        return;

    _value = value;
    setText(QVariant(_value).toString());
    emit valueChanged(_value);
}

void PositiveDoubleLineEdit::processTextChanged() {
    double value = text().toDouble();
    if (value == _value)
        return;

    _value = value;
    emit valueChanged(_value);
}
