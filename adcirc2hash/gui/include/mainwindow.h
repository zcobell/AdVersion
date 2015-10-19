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
//  File: mainwindow.h
//
//------------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <adcirc_hashlib.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString PreviousDirectory;
    explicit MainWindow(QWidget *parent = 0);
    void GetLeadingPath(QString Input);
    static void process_err(int ierr);
    ~MainWindow();

private slots:
    void on_browse_fromAdcirc_clicked();

    void on_browse_toSha_clicked();

    void on_browse_fromSha_clicked();

    void on_browse_toAdc_clicked();

    void on_button_toSha_clicked();

    void on_button_toAdc_clicked();

    int process_a2s(QString inputFile,QString outputFile);

    int process_s2a(QString inputFile,QString outputFile);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
