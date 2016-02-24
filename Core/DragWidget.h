#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QLabel>

class DragWidget : public QLabel
{
    Q_OBJECT
public:
    explicit DragWidget(QWidget *parent = 0);
    ~DragWidget();

signals:
    void dragged(const int &x, const int &y);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    int _clickXCoordinate;
    int _clickYCoordinate;

};

#endif // DRAGWIDGET_H
