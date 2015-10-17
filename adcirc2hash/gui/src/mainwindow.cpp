//-----GPL----------------------------------------------------------------------
//
// This file is part of adcirc2hash
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
//  File: mainwindow.cpp
//
//------------------------------------------------------------------------------

#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PreviousDirectory = QDir::homePath();
#ifdef Q_OS_WIN
    PreviousDirectory = PreviousDirectory+"/Desktop";
#endif

    //...Set up the file formatting options


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::process_err(int ierr)
{
    switch(ierr){
    case ERR_NOFILE:
        QMessageBox::critical(NULL,"ERROR","The file could not be opened.");
        break;
    case ERR_CANCELED:
        QMessageBox::critical(NULL,"ERROR","The operation was cancelled.");
        break;
    default:
        QMessageBox::critical(NULL,"ERROR","An unknown error hsa occured.");
        break;
    }
    return;
}
