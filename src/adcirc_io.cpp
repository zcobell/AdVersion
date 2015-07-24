#include "adcirc_io.h"

QTime polling;
int progressUpdateInterval = 1;

adcirc_io::adcirc_io(QObject *parent) : QObject(parent)
{

}

//...Comparison operator used in the sorting of nodes
bool operator< (const adcirc_node &first, const adcirc_node &second)
{
    if(first.locationHash<second.locationHash)
        return true;
    else
        return false;
}

//...Comparison operator used in the sorting of elements
bool operator< (const adcirc_element &first, const adcirc_element &second)
{
    if(first.elementHash<second.elementHash)
        return true;
    else
        return false;
}

int adcirc_io::readAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{

    //Variables
    int i;

    QString readData;
    QStringList readDataList;

    QFile meshFile(fileName);

    dialog.setLabelText("Reading the ADCIRC mesh...");
    dialog.setValue(0);

    //----------------------------------------------------//

    //Check if we can open the file
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return ERR_NOFILE;
    }

    //Read the header line
    myMesh.header = meshFile.readLine().simplified();

    //Read the number of nodes/elements
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(1);
    myMesh.NumNodes = readData.toInt();
    readData = readDataList.value(0);
    myMesh.NumElements = readData.toInt();

    dialog.setMaximum(myMesh.NumNodes*4+myMesh.NumElements*5);

    //Begin sizing the arrays
    myMesh.node.resize(myMesh.NumNodes);
    myMesh.element.resize(myMesh.NumElements);

    //Begin reading the nodes
    for(i = 0; i < myMesh.NumNodes; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(1);
        myMesh.node[i].x = readData.toDouble();
        readData = readDataList.value(2);
        myMesh.node[i].y = readData.toDouble();
        readData = readDataList.value(3);
        myMesh.node[i].z = readData.toDouble();

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;

    }


    //Begin reading the elements
    for(i = 0; i< myMesh.NumElements; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(2);
        myMesh.element[i].c1 = readData.toInt();
        readData = readDataList.value(3);
        myMesh.element[i].c2 = readData.toInt();
        readData = readDataList.value(4);
        myMesh.element[i].c3 = readData.toInt();

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    meshFile.close();

    return ERR_NOERR;
}

int adcirc_io::createAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{

    //...variables
    int i;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;

    dialog.setLabelText("Hashing the ADCIRC mesh...");

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);

    //...zero out the hash
    localHash.reset();

    //...loop over nodes to generate a unique hash
    //   for each node
    for(i=0;i<myMesh.NumNodes;i++)
    {
        //...Create a formatted string for each x, y and z;
        hashSeed1 = QString();
        hashSeed2 = QString();
        hashSeed1.sprintf("%+018.12e",myMesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",myMesh.node[i].y);

        //...Concatenate the formatted strings together
        hashSeed = QString();
        hashSeed = hashSeed1+hashSeed2; //+hashSeed3;

        //...Create the hash for the local node
        localHash.reset();
        localHash.addData(hashSeed.toUtf8(),38);

        //...Save the local hash for this node into the array
        myMesh.node[i].locationHash = localHash.result().toHex();

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;

    }

    //...Now create the hash for each element which is based upon the
    //   locations of each hash
    for(i=0;i<myMesh.NumElements;i++)
    {
        //...Create a formatted string for each vertex as a product
        //   of each location hash
        hashSeed1 = QString();
        hashSeed2 = QString();
        hashSeed3 = QString();
        hashSeed1 = myMesh.node[myMesh.element[i].c1-1].locationHash;
        hashSeed2 = myMesh.node[myMesh.element[i].c2-1].locationHash;
        hashSeed3 = myMesh.node[myMesh.element[i].c3-1].locationHash;

        //...Concatenate the formatted strings together
        hashSeed = QString();
        hashSeed = hashSeed1+hashSeed2+hashSeed3;

        //...Create the hash for the local node
        localHash.reset();
        localHash.addData(hashSeed.toUtf8(),120);

        //...Save the local hash for this node into the array
        myMesh.element[i].elementHash = localHash.result().toHex();

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    return ERR_NOERR;
}

int adcirc_io::sortAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{

    dialog.setLabelText("Sorting the hashes...");

    //...Sort the nodes, but don't transfer yet
    QVector<adcirc_node> nodeList;
    nodeList.resize(myMesh.NumNodes);
    nodeList = myMesh.node;
    qSort(nodeList);

    //...Sort the elements
    QVector<adcirc_element> elementList;
    elementList.resize(myMesh.NumElements);
    elementList = myMesh.element;
    qSort(elementList);
    myMesh.element = elementList;
    elementList.clear();

    //...Hash the connectivity table
    for(int i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].h1 = myMesh.node[myMesh.element[i].c1-1].locationHash;
        myMesh.element[i].h2 = myMesh.node[myMesh.element[i].c2-1].locationHash;
        myMesh.element[i].h3 = myMesh.node[myMesh.element[i].c3-1].locationHash;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    //...Transfer the sorted node list
    myMesh.node = nodeList;

    return ERR_NOERR;
}

int adcirc_io::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{
    QString line,tempString;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3,hashSeed4;
    int i;

    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

    dialog.setLabelText("Writing the hashed ADCIRC mesh...");

    //...Compute the full mesh hash by iterating over all the data
    //   contained
    QCryptographicHash fullHash(QCryptographicHash::Sha1);

    //...The full mesh hash is computed with the z-elevations
    //   so we can easily see if two meshes are different in
    //   any way by checking the hash in the header
    for(i=0;i<myMesh.NumNodes;i++)
    {
        hashSeed1 = QString();
        hashSeed2 = QString();
        hashSeed3 = QString();
        hashSeed  = QString();
        hashSeed1.sprintf("%+018.12e",myMesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",myMesh.node[i].y);
        hashSeed3.sprintf("%+018.12e",myMesh.node[i].z);
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        fullHash.addData(hashSeed.toUtf8(),57);

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }


    for(i=0;i<myMesh.NumElements;i++)
    {
        hashSeed1 = QString();
        hashSeed2 = QString();
        hashSeed3 = QString();
        hashSeed4 = QString();
        hashSeed  = QString();
        hashSeed1 = myMesh.element[i].elementHash;
        hashSeed2 = myMesh.element[i].h1;
        hashSeed3 = myMesh.element[i].h2;
        hashSeed4 = myMesh.element[i].h3;
        hashSeed = hashSeed1+hashSeed2+hashSeed3+hashSeed4;
        fullHash.addData(hashSeed.toUtf8(),160);

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    myMesh.mesh_hash = fullHash.result().toHex();

    output << myMesh.header << "\n";
    output << myMesh.mesh_hash << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";

    //...Write the node positions and elevations
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line = "";
        line = myMesh.node[i].locationHash;
        line = line + " " + tempString.sprintf("%+18.12e %+18.12e %+18.12e \n",
                                         myMesh.node[i].x,
                                         myMesh.node[i].y,
                                         myMesh.node[i].z);
        output << line;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    //...Write the element connectivity
    for(i=0;i<myMesh.NumElements;i++)
    {
        line = "";
        line = myMesh.element[i].elementHash;
        line = line + " " +
               myMesh.element[i].h1 + " " +
               myMesh.element[i].h2 + " " +
               myMesh.element[i].h3 + "\n";
        output << line;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    outputFile.close();

    return ERR_NOERR;
}

int adcirc_io::readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{
    QString tempString;
    QStringList tempList;
    int i;

    dialog.setLabelText("Reading the hashed ADCIRC mesh...");

    QFile meshFile(fileName);
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return ERR_NOFILE;
    }

    myMesh.header = meshFile.readLine().simplified();
    myMesh.mesh_hash = meshFile.readLine().simplified();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(1);
    myMesh.NumNodes = tempString.toInt();
    tempString = tempList.value(0);
    myMesh.NumElements = tempString.toInt();

    dialog.setMaximum((myMesh.NumNodes+myMesh.NumElements)*3);

    myMesh.node.resize(myMesh.NumNodes);
    myMesh.element.resize(myMesh.NumElements);

    //...Reading the nodes
    for(i=0;i<myMesh.NumNodes;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.node[i].locationHash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.node[i].x = tempString.toDouble();
        tempString = tempList.value(2);
        myMesh.node[i].y = tempString.toDouble();
        tempString = tempList.value(3);
        myMesh.node[i].z = tempString.toDouble();

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    //...Reading the elements
    for(i=0;i<myMesh.NumElements;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.element[i].elementHash = tempList.value(0);
        myMesh.element[i].h1 = tempList.value(1);
        myMesh.element[i].h2 = tempList.value(2);
        myMesh.element[i].h3 = tempList.value(3);

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    meshFile.close();
    return ERR_NOERR;
}

int adcirc_io::numberAdcircMesh(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{
    QMap<QString,int> mapping_s2a;
    int i;

    dialog.setLabelText("Numbering the ADCIRC mesh...");

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
    {
        mapping_s2a[myMesh.node[i].locationHash] = i;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }


    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].c1 = mapping_s2a[myMesh.element[i].h1];
        myMesh.element[i].c2 = mapping_s2a[myMesh.element[i].h2];
        myMesh.element[i].c3 = mapping_s2a[myMesh.element[i].h3];

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    return ERR_NOERR;
}

int adcirc_io::writeAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
{
    QString line;
    int i;
    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

    //...Set the progress bar update time
    polling = QTime::currentTime().addMSecs(100);

    dialog.setLabelText("Writing the ADCIRC mesh...");

    output << myMesh.header << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line.sprintf("%10i %+18.12e %+18.12e %+18.12e \n",i+1,myMesh.node[i].x,
                     myMesh.node[i].y,myMesh.node[i].z);
        output << line;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    for(i=0;i<myMesh.NumElements;i++)
    {
        line.sprintf("%10i %5i %10i %10i %10i \n",i+1,3,myMesh.element[i].c1+1,
                myMesh.element[i].c2+1,myMesh.element[i].c3+1);
        output << line;

        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
    }

    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    outputFile.close();
    return ERR_NOERR;
}

void adcirc_io::updateProgress(int &count,QProgressDialog &dialog)
{
    //...Progress Bar updates every 100ms
    count++;
    if(QTime::currentTime()>polling)
    {
        polling = QTime::currentTime().addMSecs(progressUpdateInterval);
        dialog.setValue(count);
        QApplication::processEvents();
    }
    return;
}

