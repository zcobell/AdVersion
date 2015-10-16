#include <QCoreApplication>
#include "adcirc_hashlib.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString adcircFile,sha1File;
    int conversion,ierr;
    char* adcircFile_std,sha1File_std;

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
        cin >> adcircFile_std;
        cout << "Name of output ADCSHA1 file: ";
        cin >> sha1File_std;

        adcircFile = QString(adcircFile_std);
        sha1File = QString(sha1File_std);

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
        cin >> sha1File_std;
        cout << "Name of output ADCIRC mesh: ";
        cin >> adcircFile_std;

        adcircFile = QString(adcircFile_std);
        sha1File = QString(sha1File_std);

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

