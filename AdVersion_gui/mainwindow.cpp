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
    QString version;
    bool doPartition;
    int ierr,nPartitions;
    AdVersion versioning;
    QString meshFilename      = ui->text_inputMeshFile->text();
    QString meshFoldername    = ui->text_outputMeshFolder->text();

    versioning.setHashAlgorithm(this->hashAlgorithm);

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
        ierr = versioning.createPartitions(meshFilename,meshFoldername,nPartitions);
        if(ierr!=ERROR_NOERROR)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this,"ERROR","There was an error while creating partitions.");
            return;
        }
    }

    ierr = versioning.writePartitionedMesh(meshFilename,meshFoldername);
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



void MainWindow::on_button_browseInputMesh_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select Mesh File"),this->previousDirectory,tr("ADCIRC Mesh (*.grd *.14)"));

    if(file.isNull())
        return;

    this->previousDirectory = QFileInfo(file).absoluteDir().path();
    ui->text_inputMeshFile->setText(file);
    return;
}



void MainWindow::on_button_browseOutputAdv_clicked()
{
    QString line,directory;
    QDir dir;
    int n,folderReturn;
    AdvFolderChooser *folderChooser = new AdvFolderChooser(this);
    folderChooser->initialize(this->previousDirectory,true);

    folderReturn = folderChooser->exec();
    if(folderReturn==QDialog::Accepted)
    {
        directory = folderChooser->selectedFile;
        this->previousDirectory = folderChooser->getCurrentDirectory();
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



void MainWindow::on_button_browseInputAdv_clicked()
{
    int ierr;
    QString directory,meshVersion;
    AdvFolderChooser *folderChooser = new AdvFolderChooser(this);
    folderChooser->initialize(this->previousDirectory,false);
    int folderReturn = folderChooser->exec();
    if(folderReturn==QDialog::Accepted)
    {
        directory = folderChooser->selectedFile;
        this->previousDirectory = folderChooser->getCurrentDirectory();
        ui->text_inputMeshFolder->setText(directory);

        QFile isGit(directory+"/.git");
        if(isGit.exists())
        {
            ierr = AdVersion::getGitVersion(directory,meshVersion);
            ui->text_outputMeshFile->setText("myMesh-"+meshVersion+".grd");
        }
    }
    return;
}



void MainWindow::on_button_browseOutputMesh_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr("Select Output Folder"),this->previousDirectory);

    if(file.isNull())
        return;

    this->previousDirectory = file;
    ui->text_outputMeshDirectory->setText(file);

    return;
}



void MainWindow::on_button_retrieveMesh_clicked()
{
    int ierr;
    AdVersion versioning;
    QString inputFolder, outputFile, outputFolder;

    if(ui->text_inputMeshFolder->text().isEmpty())
    {
        QMessageBox::critical(this,"ERROR","You must select an input mesh adv.");
        return;
    }

    if(ui->text_outputMeshDirectory->text().isEmpty())
    {
        QMessageBox::critical(this,"ERROR","You must select an output folder");
        return;
    }

    if(ui->text_outputMeshFile->text().isEmpty())
    {
        QMessageBox::critical(this,"ERROR","You must set an output filename");
        return;
    }

    inputFolder  = ui->text_inputMeshFolder->text();
    outputFolder = ui->text_outputMeshDirectory->text();
    outputFile   = ui->text_outputMeshFile->text();

    outputFile = outputFolder + "/" + outputFile;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    ierr = versioning.readPartitionedMesh(inputFolder);
    if(ierr!=0)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this,"ERROR","Error while reading the partitioned mesh.");
        return;
    }

    ierr = versioning.writeMesh(outputFile);
    if(ierr!=ERROR_NOERROR)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this,"ERROR","Error while writing the output mesh file.");
        return;
    }

    QApplication::restoreOverrideCursor();

    QMessageBox::information(this,"Success","The mesh was written successfully.");

    return;
}
