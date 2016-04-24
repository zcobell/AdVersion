#include "rectangle.h"
#include <QDebug>

Rectangle::Rectangle(QObject *parent)
{

}


int Rectangle::extend(QPointF point)
{
    qreal xmin,ymin,xmax,ymax;
    qreal x,y;
    qreal xmin_new,ymin_new;
    qreal xmax_new,ymax_new;

    //...Get coordinates
    xmin = this->bottomLeft().x();
    ymin = this->bottomLeft().y();
    xmax = this->topRight().x();
    ymax = this->topRight().y();
    x    = point.x();
    y    = point.y();

    if(!this->isEmpty())
    {
        //...Compare bounding box coordinates
        if(x>xmax)
            xmax_new = x;
        else
            xmax_new = xmax;

        if(x<xmin)
            xmin_new = x;
        else
            xmin_new = xmin;

        if(y>ymax)
            ymax_new = y;
        else
            ymax_new = ymax;

        if(y<ymin)
            ymin_new = y;
        else
            ymin_new = ymin;

        //...Set the new bounding box
        this->setBottomLeft(QPointF(xmin_new,ymin_new));
        this->setTopRight(QPointF(xmax_new,ymax_new));

        return 0;
    }
    else
        return -1;


}


qreal Rectangle::computeArea()
{
    this->area = qAbs(this->width() * this->height());
    return this->area;
}


bool Rectangle::containsPoint(const QPointF &p)
{

    qreal x,y;
    qreal xmin,xmax,ymin,ymax;

    x = p.x();
    y = p.y();

    xmin = this->bottomLeft().x();
    ymin = this->bottomLeft().y();
    xmax = this->topRight().x();
    ymax = this->topRight().y();

    if( x <= xmax && x >= xmin && y <= ymax && y >= ymin )
        return true;
    else
        return false;
}


int Rectangle::expand(qreal percent)
{
    qreal width_new,height_new,dx,dy;

    width_new  = this->width()*(1.0+(percent/100.0));
    height_new = this->height()*(1.0+(percent/100.0));
    dx         = (width_new-this->width())/2.0;
    dy         = (height_new-this->height())/2.0;
    this->adjust(-dx,-dy,dx,dy);
    return 0;
}

