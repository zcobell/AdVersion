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
//  File: advfilechooser.cpp
//
//------------------------------------------------------------------------------
#include "advfolderchooser.h"
#include "ui_advfolderchooser.h"
#include <QMessageBox>
#include <QPushButton>

AdvFolderChooser::AdvFolderChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvFolderChooser)
{
    ui->setupUi(this);

}



AdvFolderChooser::~AdvFolderChooser()
{
    delete ui;
}



int AdvFolderChooser::initialize(QString directory, bool allowCreate)
{

    QPixmap pixmap(":/img/images/arrow_up.png");
    QIcon ButtonIcon(pixmap);
    ui->button_upDirectory->setIcon(ButtonIcon);
    ui->button_upDirectory->setIconSize(pixmap.rect().size());

    this->create = allowCreate;
    this->startDirectory = directory;
    this->fileModel = new AdvQFileSystemModel(this);
    this->fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->listview_advfile->setModel(this->fileModel);
    ui->listview_advfile->setRootIndex(this->fileModel->setRootPath(directory));
    ui->text_currentPath->setText(directory);

    ui->text_newFile->setReadOnly(!allowCreate);

    if(this->create)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Create File");
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Select File");

    this->currentDirectory = directory;

    return 0;
}



void AdvFolderChooser::on_listview_advfile_doubleClicked(const QModelIndex &index)
{

    QString currentFile = this->fileModel->fileInfo(index).absoluteFilePath();
    if(currentFile.contains(".adv"))
    {
        this->selectedFile = currentFile;
        this->accept();
    }
    else
    {
        ui->listview_advfile->setRootIndex(this->fileModel->setRootPath(this->fileModel->fileInfo(index).absoluteFilePath()));
        ui->text_currentPath->setText(this->fileModel->fileInfo(index).absoluteFilePath());
        this->currentDirectory = this->fileModel->fileInfo(index).absoluteFilePath();

        if(this->create)
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Create File");

    }
}



void AdvFolderChooser::on_buttonBox_rejected()
{
    this->reject();
}



void AdvFolderChooser::on_buttonBox_accepted()
{

    QString newFileText = ui->text_newFile->text();

    if(newFileText.isEmpty())
    {
        QMessageBox::critical(this,"ERROR","You must enter a filename");
        return;
    }

    if(this->create)
    {
        if(!newFileText.contains(".adv"))
            newFileText = newFileText+".adv";

        this->selectedFile = this->currentDirectory+"/"+newFileText;

        this->accept();
    }
    else
    {
        QFile file(this->currentDirectory+"/"+newFileText);
        if(file.exists())
        {
            this->selectedFile = this->currentDirectory+"/"+newFileText;
            this->accept();
        }
        else
            QMessageBox::information(this,"ERROR","File does not exist");
    }
}



void AdvFolderChooser::on_listview_advfile_clicked(const QModelIndex &index)
{
    QString currentFile = this->fileModel->fileInfo(index).absoluteFilePath();
    if(currentFile.contains(".adv"))
    {
        ui->text_newFile->setText(this->fileModel->fileName(index));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Select File");
    }
    else
    {
        if(this->create)
        {
            ui->text_newFile->setText("");
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Create File");
        }
    }
    return;
}



void AdvFolderChooser::on_button_upDirectory_clicked()
{

    QDir dir(this->currentDirectory);
    dir.cdUp();
    ui->listview_advfile->setRootIndex(this->fileModel->setRootPath(dir.path()));
    ui->text_currentPath->setText(dir.path());
    this->currentDirectory = dir.path();

    return;
}



QString AdvFolderChooser::getCurrentDirectory()
{
    return this->currentDirectory;
}
