//-----GPL----------------------------------------------------------------------
//
// This file is part of libAdVersion
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
//  File: rectangle.h
//
//------------------------------------------------------------------------------
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
    int    extend(QPointF &point);
    int    expand(qreal percent);
    bool   containsPoint(const QPointF &p);
};

#endif // RECTANGLE_H
