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
#include <QProgressDialog>
#include <QPointer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PreviousDirectory = QDir::homePath();
#ifdef Q_OS_WIN
    PreviousDirectory = PreviousDirectory+"/Desktop";
#endif

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
        QMessageBox::critical(NULL,"ERROR","An unknown error has occured.");
        break;
    }
    return;
}

//------------------------------------------------------------------------------
//...Function to process ADCIRC-->SHA1 using the adcirc2hash GUI interface
//------------------------------------------------------------------------------
int MainWindow::process_a2s(QString inputFile,QString outputFile)
{
    int ierr;
    int elapsedSeconds,elapsedMinutes;
    adcirc_mesh mesh;
    QTime timer;
    QString elapsedString;

    //...Set the starting time
    timer.start();

    //...Create a progress bar
    QProgressDialog progress;
    progress.setWindowTitle("Progress");
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(4);
    progress.show();
    QApplication::processEvents();

    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;

    progress.setLabelText("Reading the ADCIRC mesh...");
    QApplication::processEvents();
    ierr = adc->readAdcircMesh(inputFile,mesh);
    progress.setValue(1);

    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }

    progress.setLabelText("Creating the hashes...");
    QApplication::processEvents();
    ierr = adc->createAdcircHashes(mesh);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    progress.setValue(2);

    progress.setLabelText("Sorting the hashes...");
    QApplication::processEvents();
    ierr = adc->sortAdcircHashes(mesh);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    progress.setValue(3);

    progress.setLabelText("Writing the hashed mesh...");
    QApplication::processEvents();
    ierr = adc->writeAdcircHashMesh(outputFile,mesh);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    progress.setValue(4);
    QApplication::processEvents();

    elapsedSeconds = timer.elapsed() / 1000;
    elapsedMinutes = elapsedSeconds / 60;
    elapsedSeconds = elapsedSeconds % 60;
    elapsedString = QString::number(elapsedMinutes) + " min, " +
            QString::number(elapsedSeconds) + " sec";

    QMessageBox::information(NULL,"Complete",QString("The mesh was converted to SHA1 format. \n")+
                                             QString("Elapsed Time: "+elapsedString));

    return ERR_NOERR;
}

//------------------------------------------------------------------------------
//...Function to process SHA1-->ADCIRC using the GUI interface.
//------------------------------------------------------------------------------
int MainWindow::process_s2a(QString inputFile,QString outputFile)
{
    int ierr;
    int elapsedSeconds,elapsedMinutes;
    adcirc_mesh mesh;
    QTime timer;
    QString elapsedString;

    //...Set the starting time
    timer.start();

    //...Create a progress bar
    QProgressDialog progress;
    progress.setWindowTitle("Progress");
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(3);
    progress.show();

    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;

    progress.setLabelText("Reading the hashed mesh...");
    QApplication::processEvents();
    ierr = adc->readAdcircSha1Mesh(inputFile,mesh);
    progress.setValue(1);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }

    progress.setLabelText("Numbering the ADCIRC mesh...");
    QApplication::processEvents();
    ierr = adc->numberAdcircMesh(mesh);
    progress.setValue(2);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }

    progress.setLabelText("Writing the ADCIRC mesh...");
    QApplication::processEvents();
    ierr = adc->writeAdcircMesh(outputFile,mesh);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    progress.setValue(3);
    QApplication::processEvents();

    elapsedSeconds = timer.elapsed() / 1000;
    elapsedMinutes = elapsedSeconds / 60;
    elapsedSeconds = elapsedSeconds % 60;
    elapsedString = QString::number(elapsedMinutes) + " min, " +
            QString::number(elapsedSeconds) + " sec";

    QMessageBox::information(NULL,"Complete",QString("The mesh was converted to ADCIRC format. \n")+
                                             QString("Elapsed Time: "+elapsedString));

    return ERR_NOERR;
}
//------------------------------------------------------------------------------
