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
//  File: main.cpp
//
//------------------------------------------------------------------------------
#include <QCoreApplication>
#include <stdio.h>
#include <QDebug>
#include "AdVersion.h"

using namespace std;

int displayHelp();
int parseCommandLineOptions(int argc, char *argv[], QString &input, QString &output, int &mode, int &nPartitions);
int partitioning();
int writePartitionedMesh();
int retrieveMesh();

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

QTextStream& qStdErr()
{
    static QTextStream ts( stderr );
    return ts;
}

int main(int argc, char *argv[])
{
    int ierr,mode,npart;
    QString input,output;

    QCoreApplication a(argc, argv);


    ierr = parseCommandLineOptions(argc,argv,input,output,mode,npart);

    if(ierr != 0)
        return ierr;


    return 0;
}


int displayHelp()
{
    qStdOut() << "AdVersion Command Line Interface \n";
    qStdOut() << "\n";
    qStdOut() << "Usage: ./adversion [OPTIONS]\n";
    qStdOut() << "\n";
    qStdOut() << "OPTIONS:\n";
    qStdOut() << "   -n    #         Partition into # subdomains\n";
    qStdOut() << "   -adv            Process input arguments into the\n";
    qStdOut() << "                    .adv structure.\n";
    qStdOut() << "   -grd            Process input arguments into the\n";
    qStdOut() << "                     standard ADCIRC format\n";
    qStdOut() << "   -I    [FILE]    Input file (.grd or .adv)\n";
    qStdOut() << "   -O    [FILE]    Output file (.grd or .adv)\n\n";
    qStdOut() << "Please report bugs to https://github.com/zcobell/AdVersion\n";
    return 0;
}

int parseCommandLineOptions(int argc, char *argv[], QString &input, QString &output, int &mode, int &nPartitions)
{
    int i,ierr;
    bool foundMode = false;
    QString argument;
    nPartitions = -1;

    if(argc<6)
    {
        qStdOut() << "ERROR: Need to specify command line arguments\n\n";
        ierr = displayHelp();
        return -1;
    }

    i = 1;
    while(i<argc)
    {
        argument = QString(argv[i]);
        if(argument=="-I")
        {
            i = i + 1;
            input = QString(argv[i]);
        }
        else if(argument=="-O")
        {
            i = i + 1;
            output = QString(argv[i]);
        }
        else if(argument=="-n")
        {
            i = i + 1;
            nPartitions = QString(argv[i]).toInt();
        }
        else if(argument=="-adv")
        {
            if(foundMode==false)
            {
                mode = 1;
                foundMode = true;
            }
            else
            {
                qStdOut() << "ERROR: Cannot specify multiple operation modes.\n\n";
                ierr = displayHelp();
                return -1;
            }
        }
        else if(argument=="-grd")
        {
            if(foundMode==false)
            {
                mode = 2;
                foundMode = true;
            }
            else
            {
                qStdOut() << "ERROR: Cannot specify multiple operation modes.\n\n";
                ierr = displayHelp();
                return -1;
            }
        }
        i++;
    }

    return 0;
}
