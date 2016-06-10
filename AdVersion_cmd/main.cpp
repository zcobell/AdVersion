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
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <stdio.h>
#include <QFileInfo>
#include <QDebug>
#include "AdVersion.h"

using namespace std;

//...Function prototypes
int generateCommandLineParse(QCommandLineParser *p);

int writeAdv(QString inputMesh, QString inputFort13, QString outputAdv, int nPart, QCryptographicHash::Algorithm hashType);
int writeMesh(QString input, QString output, QString output13, bool naming);

#ifdef EBUG
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int ierr;

    QString inputMesh = "C:/Users/zcobell/Desktop/ms-riv.grd";
    QString inputFort13 = "C:/Users/zcobell/Desktop/ms-riv.13";
    QString outputMesh = "C:/Users/zcobell/Desktop/test_fort.14";
    QString output13 = "C:/Users/zcobell/Desktop/test_fort.13";
    QString outputAdv = "C:/Users/zcobell/Desktop/test.adv";
    int nPartitions = 2;
    QCryptographicHash::Algorithm hashType = QCryptographicHash::Md5;

    ierr = writeAdv(inputMesh,inputFort13,outputAdv,nPartitions,hashType);

    ierr = writeMesh(outputAdv,outputMesh,output13,false);

    return 0;
}
#else
int main(int argc, char *argv[])
{

    bool isMd5Set,isSha1Set;
    bool isInputMeshSet,isInputAdvSet;
    bool isModeSet,isInputFort13Set;
    bool isOutputMeshSet,isOutputAdvSet;
    bool isOutputFort13Set,isDisableAutonameSet;
    bool isNumPartitionsSet;

    QString modeString,inputMesh,outputMesh,inputAdv,outputAdv;
    QString inputFort13,outputFort13,nPartitionsString;

    int ierr,mode,nPartitions;

    QCryptographicHash::Algorithm hashType;

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("AdVersion");
    QCoreApplication::setApplicationVersion(GIT_VERSION);
    QCommandLineParser parser;

    QTextStream out(stdout,QIODevice::WriteOnly);

    //...Set up the command line parser
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.setApplicationDescription("Efficient versioning for ADCIRC models\n\nPlease report bugs to:\nhttps://github.com/zcobell/AdVersion");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption modeOption(QStringList() << "mode",QCoreApplication::translate("main","Operation mode. [required]"),QCoreApplication::translate("main","adv,grd"));
    parser.addOption(modeOption);

    QCommandLineOption inputMeshOption("input-mesh",
                                   QCoreApplication::translate("main","Input ADCIRC mesh."),
                                   QCoreApplication::translate("main","mesh"));
    parser.addOption(inputMeshOption);

    QCommandLineOption inputAdvOption("input-adv",
                                      QCoreApplication::translate("main","Input AdVersion folder."),
                                      QCoreApplication::translate("main","adv"));
    parser.addOption(inputAdvOption);

    QCommandLineOption inputFort13Option("input-13",
                                         QCoreApplication::translate("main","Input ADCIRC nodal attributes file."),
                                         QCoreApplication::translate("main","fort13"));
    parser.addOption(inputFort13Option);

    QCommandLineOption outputMeshOption("output-mesh",
                                       QCoreApplication::translate("main","Output ADCIRC mesh."),
                                       QCoreApplication::translate("main","mesh"));
    parser.addOption(outputMeshOption);

    QCommandLineOption outputAdvOption("output-adv",
                                       QCoreApplication::translate("main","Output AdVersion folder."),
                                       QCoreApplication::translate("main","adv"));
    parser.addOption(outputAdvOption);

    QCommandLineOption outputFort13Option("output-13",
                                      QCoreApplication::translate("main","Output ADCIRC nodal attributes file."),
                                      QCoreApplication::translate("main","fort13"));
    parser.addOption(outputFort13Option);

    QCommandLineOption md5HashOption("md5",QCoreApplication::translate("main","Use MD5 cryptographic hashes. [default]"));
    parser.addOption(md5HashOption);

    QCommandLineOption sha1HashOption("sha1",QCoreApplication::translate("main","Use SHA1 cryptographic hashes."));
    parser.addOption(sha1HashOption);

    QCommandLineOption nPartitionOption("np",QCoreApplication::translate("main","Number of partitions to create."),QCoreApplication::translate("main","#"));
    parser.addOption(nPartitionOption);

    QCommandLineOption overrideNamingOption("disable-autoname",QCoreApplication::translate("main","Do not attempt to name output ADCIRC files based upon Git repository revision"));
    parser.addOption(overrideNamingOption);

    //...Process the command line options
    parser.process(a);

    //...Check for no command line arguments. Exit here.
    if(argc==1)
    {
        out << "ERROR: No arguments detected!\n\n";
        out.flush();
        parser.showHelp(1);
    }


    //...Check which command line arguments have been set
    isModeSet            = parser.isSet(modeOption);
    isInputAdvSet        = parser.isSet(inputAdvOption);
    isInputMeshSet       = parser.isSet(inputMeshOption);
    isInputFort13Set     = parser.isSet(inputFort13Option);
    isOutputAdvSet       = parser.isSet(outputAdvOption);
    isOutputMeshSet      = parser.isSet(outputMeshOption);
    isOutputFort13Set    = parser.isSet(outputFort13Option);
    isNumPartitionsSet   = parser.isSet(nPartitionOption);
    isDisableAutonameSet = parser.isSet(overrideNamingOption);
    isMd5Set             = parser.isSet(md5HashOption);
    isSha1Set            = parser.isSet(sha1HashOption);

    //...Check for an operation mode
    if(isModeSet==false)
    {
        out << "You must select an operation mode using --mode <adv,grd>\n";
        out.flush();
        return 1;
    }
    modeString = parser.value(modeOption);

    //...Get variables based upon the mode selected
    if(modeString=="adv")
    {
        if(!isInputMeshSet)
        {
            out << "You must set an input ADCIRC mesh using --input-mesh.\n";
            out.flush();
            return 1;
        }

        if(!isOutputAdvSet)
        {
            out << "You must set an output ADV folder using --output-adv.\n";
            out.flush();
            return 1;
        }

        mode = 1;
        inputMesh = parser.value(inputMeshOption);
        outputAdv = parser.value(outputAdvOption);
        if(isInputFort13Set)
            inputFort13 = parser.value(inputFort13Option);
        if(isNumPartitionsSet)
        {
            nPartitionsString = parser.value(nPartitionOption);
            nPartitions       = nPartitionsString.toInt();
        }
        else
            nPartitions = -1;

        if(isMd5Set)
            hashType = QCryptographicHash::Md5;
        else if(isSha1Set)
            hashType = QCryptographicHash::Sha1;
        else
            hashType = QCryptographicHash::Md5;

    }
    else if(modeString=="grd")
    {
        if(!isInputAdvSet)
        {
            out << "You must set an input ADV folder using --input-adv.\n";
            out.flush();
            return 1;
        }

        if(!isOutputMeshSet)
        {
            out << "You must set an output ADCIRC mesh using --output-mesh.\n";
            out.flush();
            return 1;
        }

        mode = 2;
        inputAdv = parser.value(inputAdvOption);
        outputMesh = parser.value(outputMeshOption);
        if(isOutputFort13Set)
            outputFort13 = parser.value(outputFort13Option);

    }
    else
    {
        out << "Invalid operation mode. Must be either 'adv' or 'grd'.\n";
        out.flush();
        return 1;
    }


    //...Enter the appropriate routine
    if(mode==1)
        ierr = writeAdv(inputMesh,inputFort13,outputAdv,nPartitions,hashType);
    else if(mode==2)
        ierr = writeMesh(inputAdv,outputMesh,outputFort13,isDisableAutonameSet);

    return 0;
}
#endif

int writeAdv(QString inputMesh, QString inputFort13, QString outputAdv, int nPart, QCryptographicHash::Algorithm hashType)
{
    QTextStream out(stdout,QIODevice::WriteOnly);
    int ierr;
    AdVersion versioning;

    //...Set the hash type
    versioning.setHashAlgorithm(hashType);

    //...First, check if the output folder exists
    QFile partition(outputAdv+"/system/partition.control");
    if(!partition.exists() && nPart == -1)
    {
        out << "ERROR: You must specify a number of partitions during the first pass.\n";
        return -1;
    }

    //...Check the output extension for ".adv" and insert if necessary
    QFileInfo outputFileInfo(outputAdv);
    if(outputFileInfo.suffix()!="adv")
        outputAdv = outputAdv+".adv";

    if(nPart != -1)
    {
        out << "Creating the mesh partitions...";
        out.flush();

        ierr = versioning.createPartitions(inputMesh,outputAdv,nPart);

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
    else
    {
        //...If we aren't going to partition, ensure mesh was previously
        //   partitioned
        QFile partFile(outputAdv+"/system/partition.control");
        if(!partFile.exists())
        {
            out << "ERROR: The mesh does not appear to have been previously partitioned.\n\n";
            out.flush();
            return -1;
        }
    }

    out << "Writing the partitioned mesh...";
    out.flush();
    if(inputFort13!=QString())
        ierr = versioning.writePartitionedMesh(inputMesh,inputFort13,outputAdv);
    else
        ierr = versioning.writePartitionedMesh(inputMesh,outputAdv);

    if(ierr!=ERROR_NOERROR)
    {
        out << "ERROR!\n";
        return ierr;
    }
    out << "done!\n";
    out.flush();

    return 0;
}


#include <QDebug>
int writeMesh(QString input, QString output, QString output13, bool naming)
{
    QTextStream out(stdout,QIODevice::WriteOnly);
    int ierr;
    QString version;
    QFileInfo fileInfo(output);
    QFileInfo fort13fileInfo(output13);
    AdVersion versioning;
    bool do13 = false;

    //...Check if the ADV exists
    QFile inputAdv(input);
    if(!inputAdv.exists())
    {
        out << "ERROR: The specified ADV does not exist.\n";
        return -1;
    }

    //...If retrieving the fort13, make sure it has been written
    inputAdv.setFileName(input+"/system/nodalAttributes.control");
    if(!inputAdv.exists() && !output13.isEmpty())
    {
        out << "ERROR: The nodal attributes are not available from this repository.\n";
        return -1;
    }
    else
        do13 = true;

    //...Get the Git version
    AdVersion::getGitVersion(input,version);

    //...Check the suffix
    if(fileInfo.suffix()!="grd" || fileInfo.suffix()!="14")
        fileInfo.setFile(output+".grd");

    if(do13 && fort13fileInfo.suffix()!=".13")
        fort13fileInfo.setFile(output13+".13");

    //...Create the output filename with the git version included
    if(version != "" && !naming)
    {
        output = fileInfo.absoluteDir().path()+"/"+fileInfo.baseName()+"-"+version+"."+fileInfo.suffix();
        if(do13)
            output13 = fort13fileInfo.absoluteDir().path()+"/"+fort13fileInfo.baseName()+"-"+version+"."+fileInfo.suffix();
    }
    else
    {
        output = fileInfo.absoluteDir().path()+"/"+fileInfo.baseName()+"."+fileInfo.suffix();
        if(do13)
            output13 = fileInfo.absoluteDir().path()+"/"+fort13fileInfo.baseName()+"."+fort13fileInfo.suffix();
    }

    //...Read the partitioned mesh
    out << "Reading the partitioned mesh...";
    ierr = versioning.readPartitionedMesh(input,do13);
    if(ierr!=ERROR_NOERROR)
        out << "ERROR!\n";
    else
        out << "done!\n";
    out.flush();

    //...Write the mesh in standard format
    out << "Writing the mesh...";
    ierr = versioning.writeMesh(output,output13);
    if(ierr!=ERROR_NOERROR)
        out << "ERROR!\n";
    else
        out << "done!\n";
    out.flush();

    return 0;
}

