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
#include <QFileInfo>
#include "AdVersion.h"

using namespace std;

//...Function prototypes
int displayHelp();
int parseCommandLineOptions(int argc, char *argv[], QString &input, QString &output,
                            int &mode, int &nPartitions, bool &overrideNaming,
                            QCryptographicHash::Algorithm &hashType);
int writeAdv(QString input, QString output, int nPart, QCryptographicHash::Algorithm hashType);
int writeMesh(QString input, QString output, bool naming);


int main(int argc, char *argv[])
{
    int ierr,mode,npart;
    bool naming;
    QString input,output;
    QCryptographicHash::Algorithm hashType;

    QCoreApplication a(argc, argv);

    QTextStream out(stdout,QIODevice::WriteOnly);

    //...Parse the command line arguments
    ierr = parseCommandLineOptions(argc,argv,input,output,mode,npart,naming,hashType);

    if(ierr != 0)
        return ierr;

    //...Enter the appropriate routine
    if(mode==1)
        ierr = writeAdv(input,output,npart,hashType);
    else if(mode==2)
        ierr = writeMesh(input,output,naming);
    else
    {
        out << "ERROR: Invalid mode.\n";
        out.flush();
        return -1;
    }

    return 0;
}



int displayHelp()
{
    QTextStream out(stdout,QIODevice::WriteOnly);

    out << "AdVersion Command Line Interface \n";
    out << "\n";
    out << "Usage: ./adversion [OPTIONS]\n";
    out << "\n";
    out << "OPTIONS:\n";
    out << "   -n    #         Partition into # subdomains\n";
    out << "   -adv            Process input arguments into the\n";
    out << "                    .adv structure.\n";
    out << "   -grd            Process input arguments into the\n";
    out << "                     standard ADCIRC format\n";
    out << "   -I    [FILE]    Input file (.grd or .adv)\n";
    out << "   -O    [FILE]    Output file (.grd or .adv)\n";
    out << "   -V              Do not automatically name output file with\n";
    out << "                   version obtained from repository\n";
    out << "   -md5            Use MD5 hashes (default)\n";
    out << "   -sha1           Use SHA1 hashes\n";
    out << "\n";
    out << "Please report bugs to https://github.com/zcobell/AdVersion\n";
    out.flush();
    return 0;
}



int parseCommandLineOptions(int argc, char *argv[], QString &input, QString &output, int &mode, int &nPartitions, bool &overrideNaming, QCryptographicHash::Algorithm &hashType)
{
    QTextStream out(stdout,QIODevice::WriteOnly);

    int i;
    bool foundMode = false;
    QString argument;

    nPartitions = -1;
    overrideNaming = false;
    hashType = QCryptographicHash::Md5;

    if(argc<6)
    {
        out << "ERROR: Need to specify command line arguments\n\n";
        displayHelp();
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
                out << "ERROR: Cannot specify multiple operation modes.\n\n";
                displayHelp();
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
                out << "ERROR: Cannot specify multiple operation modes.\n\n";
                displayHelp();
                return -1;
            }
        }
        else if(argument=="-V")
        {
            overrideNaming = true;
        }
        else if(argument=="-md5")
        {
            hashType = QCryptographicHash::Md5;
        }
        else if(argument=="-sha1")
        {
            hashType = QCryptographicHash::Sha1;
        }
        i++;
    }

    return 0;
}



int writeAdv(QString input, QString output, int nPart, QCryptographicHash::Algorithm hashType)
{
    QTextStream out(stdout,QIODevice::WriteOnly);
    int ierr;
    AdVersion versioning;

    //...Set the hash type
    versioning.setHashAlgorithm(hashType);

    //...First, check if the output folder exists
    QFile partition(output+"/system/partition.control");
    if(!partition.exists() && nPart == -1)
    {
        out << "ERROR: You must specify a number of partitions during the first pass.\n";
        return -1;
    }

    //...Check the output extension for ".adv" and insert if necessary
    QFileInfo outputFileInfo(output);
    if(outputFileInfo.suffix()!="adv")
        output = output+".adv";

    if(nPart != -1)
    {
        out << "Creating the mesh partitions...";
        out.flush();
        ierr = versioning.createPartitions(input,output,nPart);

        if(ierr!=ERROR_NOERROR)
        {
            out << "ERROR!\n";
            return ierr;
        }
        else
        {
            out << "done!\n";
        }
        out.flush();
    }

    out << "Writing the partitioned mesh...";
    out.flush();
    ierr = versioning.writePartitionedMesh(input,output);
    if(ierr!=ERROR_NOERROR)
    {
        out << "ERROR!\n";
        return ierr;
    }
    out << "done!\n";
    out.flush();

    return 0;
}



int writeMesh(QString input, QString output, bool naming)
{
    QTextStream out(stdout,QIODevice::WriteOnly);
    int ierr;
    QString version;
    QFileInfo fileInfo(output);
    AdVersion versioning;

    //...Check if the ADV exists
    QFile inputAdv(input);
    if(!inputAdv.exists())
    {
        out << "ERROR: The specified ADV does not exist.\n";
        return -1;
    }

    //...Get the Git version
    AdVersion::getGitVersion(input,version);

    //...Check the suffix
    if(fileInfo.suffix()!="grd" || fileInfo.suffix()!="14")
        fileInfo.setFile(output+".grd");

    //...Create the output filename with the git version included
    if(version != "" && !naming)
        output = fileInfo.absoluteDir().path()+"/"+fileInfo.baseName()+"-"+version+"."+fileInfo.suffix();
    else
        output = fileInfo.absoluteDir().path()+"/"+fileInfo.baseName()+"."+fileInfo.suffix();

    //...Read the partitioned mesh
    out << "Reading the partitioned mesh...";
    ierr = versioning.readPartitionedMesh(input);
    if(ierr!=ERROR_NOERROR)
        out << "ERROR!\n";
    else
        out << "done!\n";
    out.flush();

    //...Write the mesh in standard format
    out << "Writing the mesh...";
    ierr = versioning.writeMesh(output);
    if(ierr!=ERROR_NOERROR)
        out << "ERROR!\n";
    else
        out << "done!\n";
    out.flush();

    return 0;
}

