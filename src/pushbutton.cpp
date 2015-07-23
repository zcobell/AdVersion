#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QPointer>

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

    QPointer<adcirc_io> adc = new adcirc_io;

    qDebug() << "Reading mesh...";
    mesh = adc->readAdcircMesh(adcircFile);
    qDebug() << "Making hashes...";
    ierr = adc->createAdcircHashes(mesh);
    qDebug() << "Sorting hashes...";
    ierr = adc->sortAdcircHashes(mesh);
    qDebug() << "Writing mesh...";
    ierr = adc->writeAdcircHashMesh(sha1File,mesh);

    QMessageBox::information(this,"Success","The conversion to SHA1 is finished.");

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

    QPointer<adcirc_io> adc = new adcirc_io;
    mesh = adc->readAdcircSha1Mesh(sha1File);
    ierr = adc->numberAdcircMesh(mesh);
    ierr = adc->writeAdcircMesh(adcircFile,mesh);

    QMessageBox::information(this,"Success","The conversion to ADCIRC format has finished.");

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
