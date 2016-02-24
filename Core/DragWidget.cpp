#include "DragWidget.h"


// Qt
#include <QMouseEvent>


DragWidget::DragWidget(QWidget *parent) :
    QLabel(parent),
    _clickXCoordinate(0),
    _clickYCoordinate(0)
{

}
DragWidget::~DragWidget() {

}

void DragWidget::mousePressEvent(QMouseEvent *event) {
    _clickXCoordinate = event->x();
    _clickYCoordinate = event->y();
}
void DragWidget::mouseMoveEvent(QMouseEvent *event) {
    int x = event->globalPos().x() - _clickXCoordinate;
    int y = event->globalPos().y() - _clickYCoordinate;
    emit dragged(x, y);
}
