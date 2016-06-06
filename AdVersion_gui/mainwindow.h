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
//  File: mainwindow.h
//
//------------------------------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QThread>
#include "AdVersion.h"
#include "worker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_groupBox_partition_clicked(bool checked);

    void on_radio_hashSha1_toggled(bool checked);

    void on_radio_hashMd5_toggled(bool checked);

    void on_button_processData_clicked();

    void on_button_browseInputMesh_clicked();

    void on_button_browseOutputAdv_clicked();

    void on_button_browseInputAdv_clicked();

    void on_button_browseOutputMesh_clicked();

    void on_button_retrieveMesh_clicked();

    void updateProgressBar(int);

    void updateProgressText(QString);

    void closeProgressBar();

    void on_check_inputNodalAtt_clicked(bool checked);

    void on_button_browseNodalAtt_clicked();

private:
    Ui::MainWindow *ui;
    QString previousDirectory;
    QCryptographicHash::Algorithm hashAlgorithm;
    Worker  *thisWorker;
    QThread *workThread;
    QProgressDialog *progress;

};

#endif // MAINWINDOW_H
