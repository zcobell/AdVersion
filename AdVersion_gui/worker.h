#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QProgressDialog>
#include "AdVersion.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);

    int setRetrieveMeshData(QString partitionedMeshFolder, QString outputFile);

    int setPartitionMeshData(QString inputFile, int nPartitions, QString outputFile, QCryptographicHash::Algorithm hashType);

    int setOperation(QThread *thread, bool doPartition, bool doWrite, bool doRetrieve);

signals:

    void finished();

    void processingStep(QString);

public slots:

    void writePartitionMesh();

    void retrievePartitionMesh();

    void emitProcessingStep(QString);

private:

    QString partitionedMeshFolder;

    QString outputFile;

    QString inputFile;

    QCryptographicHash::Algorithm hashType;

    int nPartitions;

    bool doPartition,doWrite,doRetrieve;

};

#endif // WORKER_H
