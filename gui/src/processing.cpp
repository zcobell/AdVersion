#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>
#include <QPointer>

int adcirc_io::process_a2s(QString inputFile,QString outputFile)
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

    QPointer<adcirc_io> adc = new adcirc_io;
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

int adcirc_io::process_s2a(QString inputFile,QString outputFile)
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

    QPointer<adcirc_io> adc = new adcirc_io;
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
