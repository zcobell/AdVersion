#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>
#include <QPointer>

extern int process_a2s(QString inputFile,QString outputFile)
{
    int ierr;
    adcirc_mesh mesh;
    QPointer<adcirc_io> adc = new adcirc_io;
    mesh = adc->readAdcircMesh(inputFile);
    ierr = adc->createAdcircHashes(mesh);
    ierr = adc->sortAdcircHashes(mesh);
    ierr = adc->writeAdcircHashMesh(outputFile,mesh);
    return 0;
}

extern int process_s2a(QString inputFile,QString outputFile)
{
    int ierr;
    adcirc_mesh mesh;
    QPointer<adcirc_io> adc = new adcirc_io;
    mesh = adc->readAdcircSha1Mesh(inputFile);
    ierr = adc->numberAdcircMesh(mesh);
    ierr = adc->writeAdcircMesh(outputFile,mesh);
    return 0;
}
