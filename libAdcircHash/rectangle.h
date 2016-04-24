#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QRectF>
#include <QObject>

class Rectangle : public QRectF
{
public:
    Rectangle(QObject *parent = 0);

    qreal area;

    qreal computeArea();
    int    extend(QPointF point);
    bool   containsPoint(const QPointF &p);
    int    expand(qreal percent);
};

#endif // RECTANGLE_H
