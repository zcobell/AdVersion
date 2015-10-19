//-----GPL----------------------------------------------------------------------
//
// This file is part of adcirc2hash
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
//  File: pushbutton.cpp
//
//------------------------------------------------------------------------------

#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
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


    ierr = process_a2s(adcircFile,sha1File);
    if(ierr!=ERR_NOERR)
        process_err(ierr);

    return;
}

void MainWindow::on_button_toAdc_clicked()
{
    QString adcircFile,sha1File;
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

    ierr = process_s2a(sha1File,adcircFile);
    if(ierr!=ERR_NOERR)
        process_err(ierr);

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
