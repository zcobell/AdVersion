#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QProgressDialog>

void MainWindow::on_browse_fromAdcirc_clicked()
{
    QString Filename = QFileDialog::getOpenFileName(this,"Save as...",
                this->PreviousDirectory,"ADCIRC Mesh (*.grd *.14)");

    if(Filename==NULL)
        return;

    ui->text_fromAdcirc->setText(Filename);
    GetLeadingPath(Filename);
    return;
}

void MainWindow::on_browse_toSha_clicked()
{
    QString Filename = QFileDialog::getSaveFileName(this,"Save as...",
                this->PreviousDirectory,"SHA1 ADCIRC Mesh (*.adcsha1)");

    if(Filename==NULL)
        return;

    ui->text_toSha->setText(Filename);
    GetLeadingPath(Filename);
    return;
}

void MainWindow::on_browse_fromSha_clicked()
{
    QString Filename = QFileDialog::getOpenFileName(this,"Save as...",
                this->PreviousDirectory,"SHA1 ADCIRC Mesh (*.adcsha1)");

    if(Filename==NULL)
        return;

    ui->text_fromSha->setText(Filename);
    GetLeadingPath(Filename);
    return;
}

void MainWindow::on_browse_toAdc_clicked()
{
    QString Filename = QFileDialog::getSaveFileName(this,"Save as...",
                this->PreviousDirectory,"ADCIRC Mesh (*.grd *.14)");

    if(Filename==NULL)
        return;

    ui->text_toAdc->setText(Filename);
    GetLeadingPath(Filename);
    return;
}

void MainWindow::on_button_toSha_clicked()
{
    QString adcircFile,sha1File;
    adcirc_mesh mesh;
    int ierr;

    adcircFile = ui->text_fromAdcirc->text();
    sha1File = ui->text_toSha->text();

    if(adcircFile == NULL)
    {
        QMessageBox::critical(this,"ERROR","There was no ADCIRC mesh file specified.");
        return;
    }

    QFile adcirc(adcircFile);
    if(!adcirc.exists())
    {
        QMessageBox::critical(this,"ERROR","The ADCIRC mesh file specified does not exist.");
        return;
    }

    if(sha1File == NULL)
    {
        QMessageBox::critical(this,"ERROR","There was no SHA1 mesh file specified.");
        return;
    }

    QProgressDialog dialog;
    dialog.setLabelText("Converting ADCIRC --> SHA1");
    dialog.setWindowTitle("Processing...");
    dialog.setCancelButton(NULL);
    QFutureWatcher<int> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
    futureWatcher.setFuture(QtConcurrent::run(process_a2s,adcircFile,sha1File));
    dialog.exec();
    futureWatcher.waitForFinished();
    if(futureWatcher.result()==0)
        QMessageBox::information(this,"Success","The conversion to SHA1 format has finished.");
    else
        QMessageBox::information(this,"ERROR","The conversion was not performed successfully.");

    return;
}

void MainWindow::on_button_toAdc_clicked()
{
    QString adcircFile,sha1File;
    adcirc_mesh mesh;
    int ierr;

    adcircFile = ui->text_toAdc->text();
    sha1File = ui->text_fromSha->text();

    if(adcircFile == NULL)
    {
        QMessageBox::critical(this,"ERROR","There was no ADCIRC mesh file specified.");
        return;
    }

    if(sha1File == NULL)
    {
        QMessageBox::critical(this,"ERROR","There was no SHA1 mesh file specified.");
        return;
    }

    QFile sha1(sha1File);
    if(!sha1.exists())
    {
        QMessageBox::critical(this,"ERROR","The SHA1 ADCIRC mesh file specified does not exist.");
        return;
    }

    QProgressDialog dialog;
    dialog.setLabelText("Converting SHA1 --> ADCIRC");
    dialog.setWindowTitle("Processing...");
    dialog.setCancelButton(NULL);
    QFutureWatcher<int> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
    futureWatcher.setFuture(QtConcurrent::run(process_s2a,sha1File,adcircFile));
    dialog.exec();
    futureWatcher.waitForFinished();
    if(futureWatcher.result()==0)
        QMessageBox::information(this,"Success","The conversion to ADCIRC format has finished.");
    else
        QMessageBox::information(this,"ERROR","The conversion was not performed successfully.");

    return;
}

void MainWindow::GetLeadingPath(QString Input)
{
    QRegExp rx("[/\\\\]");
    QStringList parts = Input.split(rx);
    QString Directory = "";
    for(int i=0; i<parts.length()-1; i++)
    {
        if(i>0)
            Directory = Directory+"/"+parts.value(i);
        else
            Directory = parts.value(i);

    }
    this->PreviousDirectory = Directory;

    return;
}
