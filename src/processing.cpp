#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>
#include <QPointer>

int adcirc_io::process_a2s(QString inputFile,QString outputFile)
{
    int ierr,counter;
    adcirc_mesh mesh;

    //...Create a progress bar
    QProgressDialog progress;
    progress.setWindowTitle("Converting ADCIRC --> SHA1");
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(14);
    progress.setValue(0);
    counter = 0;
    progress.show();

    QPointer<adcirc_io> adc = new adcirc_io;
    mesh = adc->readAdcircMesh(inputFile,progress,counter);
    ierr = adc->createAdcircHashes(mesh,progress,counter);
    ierr = adc->sortAdcircHashes(mesh,progress,counter);
    ierr = adc->writeAdcircHashMesh(outputFile,mesh,progress,counter);
    return 0;
}

int adcirc_io::process_s2a(QString inputFile,QString outputFile)
{
    int ierr,counter;
    adcirc_mesh mesh;

    //...Create a progress bar
    QProgressDialog progress;
    progress.setWindowTitle("Converting SHA1 --> ADCIRC");
    progress.setWindowModality(Qt::WindowModal);
    progress.setMaximum(6);
    progress.setValue(0);
    counter = 0;

    progress.show();

    QPointer<adcirc_io> adc = new adcirc_io;
    mesh = adc->readAdcircSha1Mesh(inputFile,progress,counter);
    ierr = adc->numberAdcircMesh(mesh,progress,counter);
    ierr = adc->writeAdcircMesh(outputFile,mesh,progress,counter);
    return 0;
}
