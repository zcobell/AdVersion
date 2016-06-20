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
#include "worker.h"
#include <QFileDialog>
#include <QMessageBox>

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

    //...Blank out text in info boxes
    ui->label_showMeshVersion->setText("");
    ui->label_showNumNodes->setText("");
    ui->label_showNumElements->setText("");
    ui->label_showNumOpenBC->setText("");
    ui->label_showNumLandBC->setText("");
    ui->label_showNumPartitions->setText("");

    this->hashAlgorithm = QCryptographicHash::Md5;
    this->thisWorker = NULL;
    this->workThread = NULL;
    this->progress = NULL;

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
    bool doPartition,doNodalAttributes;
    int ierr,nPartitions;
    QString meshFilename      = ui->text_inputMeshFile->text();
    QString nodalAttFile      = ui->text_inputNodalAttFile->text();
    QString meshFoldername    = ui->text_outputMeshFolder->text();

    if(meshFilename.isEmpty())
    {
        QMessageBox::critical(this,"ERROR","No input mesh specified");
        return;
    }

    if(meshFoldername.isEmpty())
    {
        QMessageBox::critical(this,"ERROR","No output file specified");
        return;
    }

    if(ui->check_inputNodalAtt->isChecked())
    {
        if(ui->text_inputNodalAttFile->text().isEmpty())
        {
            QMessageBox::critical(this,"ERROR","Select a nodal attributes (fort.13) file or deselect the option");
            return;
        }
        doNodalAttributes = true;
    }
    else
        doNodalAttributes = false;

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

    this->progress = new QProgressDialog(this);
    this->progress->setMinimum(0);
    this->progress->setMaximum(0);
    this->progress->setCancelButton(NULL);
    this->progress->setWindowTitle("Progress");
    this->progress->setWindowModality(Qt::WindowModal);

    this->workThread = new QThread(this);
    this->thisWorker = new Worker();
    this->thisWorker->setOperation(this->workThread,doPartition,true,false);
    this->thisWorker->setPartitionMeshData(meshFilename,nodalAttFile,nPartitions,meshFoldername,doNodalAttributes,this->hashAlgorithm);

    connect(this->thisWorker,SIGNAL(processingStep(QString)),this,SLOT(updateProgressText(QString)));
    connect(this->thisWorker,SIGNAL(finished()),this,SLOT(closeProgressBar()));
    connect(this->thisWorker,SIGNAL(finished()),this->thisWorker,SLOT(deleteLater()));

    this->thisWorker->moveToThread(this->workThread);
    this->workThread->start();
    this->progress->show();

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

    return;
}



void MainWindow::on_button_browseInputAdv_clicked()
{
    int ierr;
    QString directory,meshVersion,tempString;
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
            if(meshVersion==QString())
                meshVersion = "Not Versioned";
            else
                ui->text_outputMeshFile->setText("myMesh-"+meshVersion+".grd");
            ui->label_showMeshVersion->setText(meshVersion);
        }
        else
            ui->label_showMeshVersion->setText("Not Versioned");

        QFile metaFile(directory+"/system/mesh.header");
        if(!metaFile.open(QIODevice::ReadOnly))
            return;

        tempString = metaFile.readLine().simplified();
        tempString = metaFile.readLine().simplified();
        ui->label_showNumNodes->setText(QLocale().toString(tempString.toInt()));
        tempString = metaFile.readLine().simplified();
        ui->label_showNumElements->setText(QLocale().toString(tempString.toInt()));
        tempString = metaFile.readLine().simplified();
        ui->label_showNumOpenBC->setText(QLocale().toString(tempString.toInt()));
        tempString = metaFile.readLine().simplified();
        ui->label_showNumLandBC->setText(QLocale().toString(tempString.toInt()));

        metaFile.close();

        metaFile.setFileName(directory+"/system/partition.control");
        if(!metaFile.open(QIODevice::ReadOnly))
            return;

        tempString = metaFile.readLine().simplified();
        ui->label_showNumPartitions->setText(tempString);

        metaFile.close();

        QFile nodalAttFile(directory+"/system/nodalAttributes.control");
        if(!nodalAttFile.exists())
        {
            ui->label_outputFort13File->setEnabled(false);
            ui->text_outputFort13File->setEnabled(false);
            ui->text_outputFort13File->clear();
        }
        else
        {
            ui->label_outputFort13File->setEnabled(true);
            ui->text_outputFort13File->setEnabled(true);
            if(meshVersion!="Not Versioned")
                ui->text_outputFort13File->setText("myMesh-"+meshVersion+".13");
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
    QString inputFolder, outputFile, outputFolder, outputFort13;

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
    outputFort13 = ui->text_outputFort13File->text();

    outputFile   = outputFolder + "/" + outputFile;

    if(!outputFort13.isEmpty())
        outputFort13 = outputFolder + "/" + outputFort13;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    this->progress = new QProgressDialog(this);
    this->progress->setMinimum(0);
    this->progress->setMaximum(0);
    this->progress->setCancelButton(NULL);
    this->progress->setWindowTitle("Progress");
    this->progress->setWindowModality(Qt::WindowModal);

    this->workThread = new QThread(this);
    this->thisWorker = new Worker();
    this->thisWorker->setOperation(this->workThread,false,false,true);
    this->thisWorker->setRetrieveMeshData(inputFolder,outputFile,outputFort13);

    connect(this->thisWorker,SIGNAL(processingStep(QString)),this,SLOT(updateProgressText(QString)));
    connect(this->thisWorker,SIGNAL(finished()),this,SLOT(closeProgressBar()));
    connect(this->thisWorker,SIGNAL(finished()),this->thisWorker,SLOT(deleteLater()));

    this->thisWorker->moveToThread(this->workThread);
    this->workThread->start();
    this->progress->show();

    return;
}



void MainWindow::updateProgressBar(int pct)
{
    this->progress->setValue(pct);
    return;
}



void MainWindow::updateProgressText(QString string)
{
    this->progress->setLabelText(string);
    return;
}



void MainWindow::closeProgressBar()
{
    QApplication::restoreOverrideCursor();
    this->progress->close();
    return;
}


void MainWindow::on_check_inputNodalAtt_clicked(bool checked)
{
    ui->label_nodalAttFile->setEnabled(checked);
    ui->text_inputNodalAttFile->setEnabled(checked);
    ui->button_browseNodalAtt->setEnabled(checked);
    return;
}

void MainWindow::on_button_browseNodalAtt_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select Nodal Attributes File"),this->previousDirectory,tr("ADCIRC Nodal Attributes File (*.13)"));

    if(file.isNull())
        return;

    this->previousDirectory = QFileInfo(file).absoluteDir().path();
    ui->text_inputNodalAttFile->setText(file);
    return;
}
