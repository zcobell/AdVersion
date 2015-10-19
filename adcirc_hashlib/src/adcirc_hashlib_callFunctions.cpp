//-----GPL----------------------------------------------------------------------
//
// This file is part of adcirc_hashlib
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
//  File: adcirc_hashlib_callFunctions.cpp
//
//------------------------------------------------------------------------------

#include "adcirc_hashlib.h"
#include <QPointer>

//------------------------------------------------------------------------------
//...Function to update the progress bar when called from the adcirc2hash gui
//------------------------------------------------------------------------------
#ifdef GUI
void adcirc_hashlib::updateProgress(int &count,QProgressDialog &dialog)
{
    //...Progress Bar updates every 100ms
    count++;
    if(QTime::currentTime()>polling)
    {
        polling = QTime::currentTime().addMSecs(progressUpdateInterval);
        dialog.setValue(count);
        QApplication::processEvents();
    }
    return;
}
#endif
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Function to process ADCIRC-->SHA1 using the adcirc2hash GUI interface
//   This should probably be moved out of the library, but for now it stays
//------------------------------------------------------------------------------
#ifdef GUI
int adcirc_hashlib::process_a2s(QString inputFile,QString outputFile)
{
    int ierr,counter;
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
    progress.setMaximum(9);
    progress.setValue(0);
    counter = 0;
    progress.show();

    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;
    ierr = adc->readAdcircMesh(inputFile,mesh,progress,counter);

    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    ierr = adc->createAdcircHashes(mesh,progress,counter);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    ierr = adc->sortAdcircHashes(mesh,progress,counter);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    ierr = adc->writeAdcircHashMesh(outputFile,mesh,progress,counter);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }

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
//   This should probably be moved out of the library, but for now it stays
//------------------------------------------------------------------------------
int adcirc_hashlib::process_s2a(QString inputFile,QString outputFile)
{
    int ierr,counter;
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
    progress.setMaximum(6);
    progress.setValue(0);
    counter = 0;
    progress.show();

    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;
    ierr = adc->readAdcircSha1Mesh(inputFile,mesh,progress,counter);

    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    ierr = adc->numberAdcircMesh(mesh,progress,counter);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }
    ierr = adc->writeAdcircMesh(outputFile,mesh,progress,counter);
    if(ierr!=ERR_NOERR)
    {
        MainWindow::process_err(ierr);
        return ERR_NOERR;
    }

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
#else


//------------------------------------------------------------------------------
//...Function to process ADCIRC-->SHA1 using the adcirc2hash command line interface
//   This should probably be moved out of the library, but for now it stays
//------------------------------------------------------------------------------
int adcirc_hashlib::process_a2s(QString inputFile,QString outputFile)
{
    int ierr,counter;
    int elapsedSeconds,elapsedMinutes;
    adcirc_mesh mesh;
    QTime timer;
    QString elapsedString;

    //...Set the starting time
    timer.start();

    cout << "Reading ADCIRC mesh...";
    cout.flush();
    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;
    ierr = adc->readAdcircMesh(inputFile,mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    cout << "Hashing ADCIRC mesh...";
    cout.flush();
    ierr = adc->createAdcircHashes(mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    cout << "Sorting ADCIRC hashes...";
    cout.flush();
    ierr = adc->sortAdcircHashes(mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    cout << "Writing hashed mesh...";
    cout.flush();
    ierr = adc->writeAdcircHashMesh(outputFile,mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    elapsedSeconds = timer.elapsed() / 1000;
    elapsedMinutes = elapsedSeconds / 60;
    elapsedSeconds = elapsedSeconds % 60;
    elapsedString = QString::number(elapsedMinutes) + " min, " +
            QString::number(elapsedSeconds) + " sec";

    cout << "Elapsed time: " << elapsedString.toStdString() << "\n";

    return ERR_NOERR;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Function to process SHA1-->ADCIRC using the command line interface.
//   This should probably be moved out of the library, but for now it stays
//------------------------------------------------------------------------------
int adcirc_hashlib::process_s2a(QString inputFile,QString outputFile)
{
    int ierr,counter;
    int elapsedSeconds,elapsedMinutes;
    adcirc_mesh mesh;
    QTime timer;
    QString elapsedString;

    //...Set the starting time
    timer.start();

    QPointer<adcirc_hashlib> adc = new adcirc_hashlib;

    cout << "Reading hashed mesh...";
    cout.flush();
    ierr = adc->readAdcircSha1Mesh(inputFile,mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    cout << "Numbering the ADCIRC mesh...";
    cout.flush();
    ierr = adc->numberAdcircMesh(mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    cout << "Writing the ADCIRC mesh...";
    cout.flush();
    ierr = adc->writeAdcircMesh(outputFile,mesh);
    if(ierr!=ERR_NOERR)
        return ierr;
    cout << "done!\n";

    elapsedSeconds = timer.elapsed() / 1000;
    elapsedMinutes = elapsedSeconds / 60;
    elapsedSeconds = elapsedSeconds % 60;
    elapsedString = QString::number(elapsedMinutes) + " min, " +
            QString::number(elapsedSeconds) + " sec";

    cout << "Elapsed time: " << elapsedString.toStdString();

    return ERR_NOERR;
}

#endif
//------------------------------------------------------------------------------
