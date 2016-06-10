#include "worker.h"

Worker::Worker(QObject *parent) : QObject(parent)
{
    this->hashType = QCryptographicHash::Md5;
    this->nPartitions = -1;
    this->doPartition = false;
    this->doWrite = false;
    this->doRetrieve = false;
    this->doNodalAttributes = false;
}


int Worker::setRetrieveMeshData(QString partitionedMeshFolder, QString outputFile)
{
    this->partitionedMeshFolder = partitionedMeshFolder;
    this->outputFile            = outputFile;
    return 0;
}


int Worker::setPartitionMeshData(QString inputFile, QString nodalAttFile, int nPartitions, QString outputFile,  bool doNodalAttributes, QCryptographicHash::Algorithm hashType)
{
    this->inputFile = inputFile;
    this->nodalAttFile = nodalAttFile;
    this->doNodalAttributes = doNodalAttributes;
    this->nPartitions = nPartitions;
    this->outputFile = outputFile;
    this->hashType = hashType;
    return 0;
}


int Worker::setOperation(QThread *thread, bool doPartition, bool doWrite, bool doRetrieve)
{
    this->doPartition = doPartition;
    this->doWrite = doWrite;
    this->doRetrieve = doRetrieve;

    if(this->doPartition || this->doWrite)
        connect(thread,SIGNAL(started()),this,SLOT(writePartitionMesh()));
    else if(this->doRetrieve)
        connect(thread,SIGNAL(started()),this,SLOT(retrievePartitionMesh()));

    connect(this,SIGNAL(finished()),thread,SLOT(quit()));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),this,SLOT(deleteLater()));

    return 0;
}


void Worker::writePartitionMesh()
{

    int ierr;
    AdVersion versioning;

    versioning.setHashAlgorithm(this->hashType);

    if(this->doPartition)
    {
        emit processingStep("Developing mesh partitions...");
        ierr = versioning.createPartitions(this->inputFile,this->outputFile,this->nPartitions);
    }
    emit processingStep("Writing partitioned mesh...");
    if(this->doNodalAttributes)
        ierr = versioning.writePartitionedMesh(this->inputFile,this->nodalAttFile,this->outputFile);
    else
        ierr = versioning.writePartitionedMesh(this->inputFile,this->outputFile);

    emit finished();

    return;
}


void Worker::retrievePartitionMesh()
{

    AdVersion versioning;
    int ierr;

    emit processingStep("Reading partitioned mesh...");
    ierr = versioning.readPartitionedMesh(this->partitionedMeshFolder);

    emit processingStep("Writing ADCIRC formatted mesh...");
    ierr = versioning.writeMesh(this->outputFile,this->output13File);

    emit finished();

    return;
}


void Worker::emitProcessingStep(QString string)
{
    emit processingStep(string);
    return;
}
