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

    int setRetrieveMeshData(QString partitionedMeshFolder, QString outputFile, QString outputFort13);

    int setPartitionMeshData(QString inputFile, QString nodalAttFile, int nPartitions, QString outputFile,  bool doNodalAttributes, QCryptographicHash::Algorithm hashType);

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

    QString output13File;

    QString inputFile;

    QString nodalAttFile;

    QCryptographicHash::Algorithm hashType;

    int nPartitions;

    bool doPartition,doWrite,doRetrieve,doNodalAttributes;

};

#endif // WORKER_H
