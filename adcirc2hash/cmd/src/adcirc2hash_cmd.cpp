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
//  File: adcirc2hash_cmd.cpp
//
//------------------------------------------------------------------------------


#include <QCoreApplication>
#include "adcirc_hashlib.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <QTextStream>
#include <QPointer>

int process_a2s(QString inputFile,QString outputFile);
int process_s2a(QString inputFile,QString outputFile);

using namespace std;


//------------------------------------------------------------------------------
//...Main function for conversion
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString adcircFile,sha1File;
    int conversion,ierr;

    QTextStream sin(stdin);

    cout << "\n";
    cout << "    ADCIRC mesh hashing utility     \n";
    cout << "      (c) Zach Cobell 2015          \n";
    cout << "\n";
    cout << "Choose conversion direction:\n";
    cout << "(1) ADCIRC .grd --> ADCIRC .adcsha1 \n";
    cout << "(2) ADCIRC .adcsha1 --> ADCIRC .grd \n";
    cout << "==> ";
    cin >> conversion;


    if(conversion==1)
    {

        cout << "Name of ADCIRC mesh: ";
        sin >> adcircFile;
        cout << "Name of output ADCSHA1 file: ";
        sin >> sha1File;

        if(adcircFile == NULL)
        {
            cout << "ERROR: There was no ADCIRC mesh file specified.\n";
            return -1;
        }

        QFile adcirc(adcircFile);
        if(!adcirc.exists())
        {
            cout << "ERROR: The ADCIRC mesh file specified does not exist.\n";
            return -1;
        }

        if(sha1File == NULL)
        {
            cout << "ERROR: There was no SHA1 mesh file specified.\n";
            return -1;
        }


        ierr = process_a2s(adcircFile,sha1File);
    }
    else if(conversion==2)
    {
        cout << "Name of ADCSHA1 file: ";
        sin >> sha1File;
        cout << "Name of output ADCIRC mesh: ";
        sin >> adcircFile;

        if(adcircFile == NULL)
        {
            cout << "ERROR: There was no ADCIRC mesh file specified.\n";
            return -1;
        }

        if(sha1File == NULL)
        {
            cout << "ERROR: There was no SHA1 mesh file specified.\n";
            return -1;
        }

        QFile sha1(sha1File);
        if(!sha1.exists())
        {
            cout << "ERROR: The SHA1 ADCIRC mesh file specified does not exist.\n";
            return -1;
        }

        ierr = process_s2a(sha1File,adcircFile);
    }


    return 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//...Function to process ADCIRC-->SHA1 using the adcirc2hash command line interface
//------------------------------------------------------------------------------
int process_a2s(QString inputFile,QString outputFile)
{
    int ierr;
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
//------------------------------------------------------------------------------
int process_s2a(QString inputFile,QString outputFile)
{
    int ierr;
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

