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
int parseCommandLineOptions(int argc, char *argv[], QString input, QString output, int mode, int nPartitions);
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



    return 0;
}


int displayHelp()
{
    qStdOut() << "AdVersion Command Line Interface \n";
    qStdOut() << "\n";
    qStdOut() << "Usage: ./adversion [OPTIONS]";
    qStdOut() << "\n";
    qStdOut() << "OPTIONS:\n";
    qStdOut() << "   -n    #             Partition into # subdomains\n";
    qStdOut() << "   -adv                Process input arguments into the\n";
    qStdOut() << "                       .adv structure.\n";
    qStdOut() << "   -grd                Process input arguments into the\n";
    qStdOut() << "                       standard ADCIRC format\n";
    qStdOut() << "   -I    [FILE]        Input file (.grd or .adv)\n";
    qStdOut() << "   -O    [DIRECTORY]   Output file (.grd or .adv)\n";
    qStdOut() << "Please report bugs to https://github.com/zcobell/AdVersion\n";
    return 0;
}

int parseCommandLineOptions(int argc, char *argv[], QString input, QString output, int mode, int nPartitions)
{
    int i,ierr;

    if(argc<4)
    {
        qStdOut() << "ERROR: Need to specify command line arguments\n\n";
        ierr = displayHelp();
        return -1;
    }
    for(i=1;i<argc;i++)
    {
        qDebug() << argv[i];
    }


    return 0;
}
