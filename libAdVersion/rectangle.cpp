//-----GPL----------------------------------------------------------------------
//
// This file is part of AdVersion
// Copyright (C) 2015  Zach Cobell
//
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: rectangle.cpp
//
//------------------------------------------------------------------------------
#include "rectangle.h"


//-----------------------------------------------------------------------------------------//
//...Constructor
//-----------------------------------------------------------------------------------------//
/**
 * \fn Rectangle::Rectangle
 * \brief Constructor with reference to QObject
 *
 * Constructor with reference to QObject
 *
 **/
//-----------------------------------------------------------------------------------------//
Rectangle::Rectangle(QObject *parent)
{
    this->area = 0.0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to enlarge the boundary of a rectangle
//-----------------------------------------------------------------------------------------//
/**
 * \fn Rectangle::extend
 * \brief Method to enlarge the boundary of a rectangle
 *
 * Method to enlarge the boundary of a rectangle
 *
 **/
//-----------------------------------------------------------------------------------------//
int Rectangle::extend(QPointF &point)
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
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to compute the area of a rectangle
//-----------------------------------------------------------------------------------------//
/**
 * \fn Rectangle::computeArea
 * \brief Method to compute the area of a rectangle
 *
 * Method to compute the area of a rectangle
 *
 **/
//-----------------------------------------------------------------------------------------//
qreal Rectangle::computeArea()
{
    this->area = qAbs(this->width() * this->height());
    return this->area;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to decide if a point resides within a rectangle
//-----------------------------------------------------------------------------------------//
/**
 * \fn Rectangle::containsPoint
 * \brief Method to decide if a point resides within a rectangle
 *
 * Method to decide if a point resides within a rectangle
 *
 **/
//-----------------------------------------------------------------------------------------//
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
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Method to enlarge a rectangle by a specified percentage
//-----------------------------------------------------------------------------------------//
/**
 * \fn Rectangle::expand
 * \brief Method to enlarge a rectangle by a specified percentage
 *
 * Method to enlarge a rectangle by a specified percentage
 *
 **/
//-----------------------------------------------------------------------------------------//
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
//-----------------------------------------------------------------------------------------//
