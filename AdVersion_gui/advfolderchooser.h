#ifndef ADVFOLDERCHOOSER_H
#define ADVFOLDERCHOOSER_H
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
//  File: advfilechooser.h
//
//------------------------------------------------------------------------------

#include <QDialog>
#include "advqfilesystemmodel.h"

namespace Ui {
class AdvFolderChooser;
}

class AdvFolderChooser : public QDialog
{
    Q_OBJECT

public:
    explicit AdvFolderChooser(QWidget *parent = 0);
    ~AdvFolderChooser();

    int initialize(QString directory, bool allowCreate);

    QString getCurrentDirectory();

    int setModelPath(QString directory);

    QString selectedFile;

private slots:

    void on_listview_advfile_doubleClicked(const QModelIndex &index);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_listview_advfile_clicked(const QModelIndex &index);

    void on_button_upDirectory_clicked();

    void on_combo_driveSelect_currentIndexChanged(const QString &arg1);

    void on_text_newFile_returnPressed();

private:
    Ui::AdvFolderChooser    *ui;
    AdvQFileSystemModel     *dirModel;
    AdvQFileSystemModel     *fileModel;
    QString                  startDirectory;
    QString                  currentDirectory;
    bool                     create;
    bool                     returnPressed;
    bool                     shown;

};

#endif // ADVFOLDERCHOOSER_H
