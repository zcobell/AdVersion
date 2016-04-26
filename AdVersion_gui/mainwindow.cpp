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
//  File: mainwindow.cpp
//
//------------------------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "advfolderchooser.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef _WIN32
    this->previousDirectory = QDir::homePath()+"/Desktop";
#else
    this->previousDirectory = QDir::homePath();
#endif

    this->hashAlgorithm = QCryptographicHash::Md5;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_button_browseInMesh_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select Mesh File"),this->previousDirectory,tr("ADCIRC Mesh (*.grd *.14)"));

    if(file.isNull())
        return;

    this->previousDirectory = QFileInfo(file).absoluteDir().path();
    ui->text_inputMeshFile->setText(file);
    return;
}


void MainWindow::on_button_browseAdv_clicked()
{
    QString line,directory;
    QDir dir;
    int n,folderReturn;
    AdvFolderChooser *folderChooser = new AdvFolderChooser(this);
    folderChooser->initialize(this->previousDirectory);

    folderReturn = folderChooser->exec();
    if(folderReturn==QDialog::Accepted)
    {
        directory = folderChooser->selectedFile;
        this->previousDirectory = QFileInfo(directory).absoluteDir().path();
        ui->text_outputMeshFolder->setText(directory);

        dir.setPath(directory);
        if(dir.exists())
        {
            QFile partition(dir.path()+"/system/partition.control");
            if(partition.exists())
            {
                if(!partition.open(QIODevice::ReadOnly))
                    return;

                line = partition.readLine().simplified();
                n    = line.toInt();
                ui->spin_nPartitions->setValue(n);
                ui->groupBox_partition->setChecked(false);
                partition.close();
            }
            else
            {
                ui->groupBox_partition->setChecked(true);
                ui->spin_nPartitions->setEnabled(true);
                ui->label_nPartitions->setEnabled(true);
            }

            QFile hashStyle(dir.path()+"/system/hash.type");
            if(hashStyle.exists())
            {
                if(!hashStyle.open(QIODevice::ReadOnly))
                    return;

                line = hashStyle.readLine().simplified();
                if(line=="md5")
                {
                    ui->radio_hashMd5->setChecked(true);
                    ui->radio_hashSha1->setChecked(false);
                }
                else if(line=="sha1")
                {
                    ui->radio_hashMd5->setChecked(false);
                    ui->radio_hashSha1->setChecked(true);
                }
            }
        }
    }
    delete folderChooser;

    return;
}


void MainWindow::on_groupBox_partition_clicked(bool checked)
{
    ui->label_nPartitions->setDisabled(!checked);
    ui->spin_nPartitions->setDisabled(!checked);
    return;
}


void MainWindow::on_radio_hashSha1_toggled(bool checked)
{
    if(checked)
        this->hashAlgorithm = QCryptographicHash::Sha1;
    return;
}


void MainWindow::on_radio_hashMd5_toggled(bool checked)
{
    if(checked)
        this->hashAlgorithm = QCryptographicHash::Md5;
    return;
}


void MainWindow::on_button_processData_clicked()
{
    bool doPartition;
    int ierr,nPartitions;
    this->versioning          = new AdVersion(this);
    QString meshFilename      = ui->text_inputMeshFile->text();
    QString meshFoldername    = ui->text_outputMeshFolder->text();

    this->versioning->setHashAlgorithm(this->hashAlgorithm);

    nPartitions = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //...Check to see if partitioning needs to be run...
    if(ui->groupBox_partition->isChecked() || !ui->groupBox_partition->isCheckable())
    {
        QFile meshFile(meshFilename);
        if(!meshFile.exists())
        {
            QMessageBox::critical(this,"ERROR","The mesh file was not found.");
            return;
        }
        nPartitions = ui->spin_nPartitions->value();
        doPartition = true;
    }
    else
    {
        doPartition = false;
    }

    meshFoldername = ui->text_outputMeshFolder->text();

    if(doPartition)
    {
        ierr = this->versioning->createPartitions(meshFilename,meshFoldername,nPartitions);
        if(ierr!=ERROR_NOERROR)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this,"ERROR","There was an error while creating partitions.");
            return;
        }
    }

    ierr = this->versioning->writePartitionedMesh(meshFilename,meshFoldername);
    if(ierr!=ERROR_NOERROR)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this,"ERROR","There was an error while creating partitions.");
        return;
    }

    QApplication::restoreOverrideCursor();
    QMessageBox::information(this,"Success","The mesh was written successfully.");

    return;
}
