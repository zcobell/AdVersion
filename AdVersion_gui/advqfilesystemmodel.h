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
//  File: advfilesystemmodel.h
//
//------------------------------------------------------------------------------
#ifndef ADVQFILESYSTEMMODEL_H
#define ADVQFILESYSTEMMODEL_H

#include <QObject>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

class AdvQFileSystemModel : public QFileSystemModel
{
public:
    AdvQFileSystemModel(QObject* parent = 0) : QFileSystemModel(parent){}

protected:
    QVariant data( const QModelIndex& index, int role ) const;

};


#endif // ADVQFILESYSTEMMODEL_H
