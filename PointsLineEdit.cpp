#include "PointsLineEdit.h"


// Qt
#include <QRegExp>
#include <QRegExpValidator>


PointsLineEdit::PointsLineEdit(QWidget *parent) : QLineEdit(parent)
{
    _points = 0;

    QRegExp regex("[1-9]+\\d*");
    QRegExpValidator *validator = new QRegExpValidator(regex, this);
    this->setValidator(validator);

    connect(this, SIGNAL(textChanged(QString)),
            this, SLOT(processTextChanged()));
}

PointsLineEdit::~PointsLineEdit()
{

}

uint PointsLineEdit::points() const {
    return _points;
}

void PointsLineEdit::setPoints(uint points) {
    if (points == _points)
        return;
    if (points == 0)
        return;

    _points = points;
    setText(QVariant(_points).toString());
    emit pointsChanged(_points);
}

void PointsLineEdit::processTextChanged() {
    uint points = text().toUInt();
    if (points == _points)
        return;

    _points = points;
    emit pointsChanged(_points);
}
