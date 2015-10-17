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

using namespace std;

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


        ierr = adcirc_hashlib::process_a2s(adcircFile,sha1File);
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

        ierr = adcirc_hashlib::process_s2a(sha1File,adcircFile);
    }


    return 0;
}

