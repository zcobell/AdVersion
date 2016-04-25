#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef _WIN32
    this->previousDirectory = QDir::homePath()+"/Desktop";
#else
    this->previousDirectory = QDir::homePath();
#endif


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_browseInMesh_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select Mesh File"),this->previousDirectory,tr("ADCIRC Mesh (*.grd, *.14)"));

    if(file.isNull())
        return;

    this->previousDirectory = QFileInfo(file).absoluteDir().path();
    ui->text_inputMeshFile->setText(file);
    return;
}

void MainWindow::on_button_browseInMesh_2_clicked()
{
    QString line;
    QDir dir;
    int n;
    QString directory = QFileDialog::getExistingDirectory(this,tr("Choose or Create Directory"),this->previousDirectory);

    if(directory.isNull())
        return;

    this->previousDirectory = QFileInfo(directory).absoluteDir().path();
    ui->text_outputMeshFolder->setText(directory);

    dir.setPath(directory);
    if(dir.exists())
    {
        QFile partition(dir.path()+"/system/partition.control");
        if(partition.exists())
        {
            if(!partition.open(QIODevice::ReadOnly))
                return;

            line = partition.readLine().simplified();
            n    = line.toInt();
            ui->spin_nPartitions->setValue(n);
            ui->groupBox_partition->setCheckable(true);
            ui->groupBox_partition->setChecked(false);
            partition.close();
        }
        else
        {
            ui->groupBox_partition->setCheckable(false);
            ui->groupBox_partition->setChecked(true);
            ui->spin_nPartitions->setEnabled(true);
            ui->label_nPartitions->setEnabled(true);
        }

    }

    return;
}

void MainWindow::on_groupBox_partition_clicked(bool checked)
{
    ui->label_nPartitions->setDisabled(!checked);
    ui->spin_nPartitions->setDisabled(!checked);
    return;
}

void MainWindow::on_pushButton_clicked()
{
    bool doPartition;
    int ierr,nPartitions;
    this->versioning          = new AdVersion(this);
    QString meshFilename      = ui->text_inputMeshFile->text();
    QString meshFoldername    = ui->text_outputMeshFolder->text();

    nPartitions = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //...Check to see if partitioning needs to be run...
    if(ui->groupBox_partition->isChecked() || !ui->groupBox_partition->isCheckable())
    {
        QFile meshFile(meshFilename);
        if(!meshFile.exists())
        {
            QMessageBox::critical(this,"ERROR","The mesh file was not found.");
            return;
        }
        nPartitions = ui->spin_nPartitions->value();
        doPartition = true;
    }
    else
    {
        doPartition = false;
    }

    meshFoldername = ui->text_outputMeshFolder->text();

    if(doPartition)
    {
        ierr = this->versioning->createPartitions(meshFilename,meshFoldername,nPartitions);
        if(ierr!=ERROR_NOERROR)
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this,"ERROR","There was an error while creating partitions.");
            return;
        }
    }

    ierr = this->versioning->writePartitionedMesh(meshFilename,meshFoldername);
    if(ierr!=ERROR_NOERROR)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this,"ERROR","There was an error while creating partitions.");
        return;
    }

    QApplication::restoreOverrideCursor();
    QMessageBox::information(this,"Success","The mesh was written successfully.");

    return;
}

