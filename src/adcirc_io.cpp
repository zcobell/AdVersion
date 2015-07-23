#include "adcirc_io.h"

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

adcirc_mesh adcirc_io::readAdcircMesh(QString fileName)
{

    //Variables
    int i;

    QString readData;
    QStringList readDataList;

    QFile meshFile(fileName);

    adcirc_mesh myMesh;

    //----------------------------------------------------//

    //Check if we can open the file
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return myMesh;
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
    }

    meshFile.close();

    return myMesh;
}

int adcirc_io::createAdcircHashes(adcirc_mesh &myMesh)
{

    //...variables
    int i;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;
    const char* hashCSeed;

    //...initialize the sha1 hash
    QCryptographicHash localHash(QCryptographicHash::Sha1);

    //...zero out the hash
    localHash.reset();

    //...loop over nodes to generate a unique hash
    //   for each node
    for(i=0;i<myMesh.NumNodes;i++)
    {
        //...Create a formatted string for each x, y and z;
        hashSeed1.sprintf("%+018.12e",myMesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",myMesh.node[i].y);

        //Currently, neglect the z-value so it does not alter
        //the connectivity hashes
        //hashSeed3.sprintf("%+018.12e",myMesh.z_elevation[i]);

        //...Concatenate the formatted strings together
        hashSeed = hashSeed1+hashSeed2; //+hashSeed3;
        hashCSeed = hashSeed.toStdString().c_str();

        //...Create the hash for the local node
        localHash.reset();
        localHash.addData(hashCSeed,38);

        //...Save the local hash for this node into the array
        myMesh.node[i].locationHash = localHash.result().toHex();
    }

    //...Now create the hash for each element which is based upon the
    //   locations of each hash
    for(i=0;i<myMesh.NumElements;i++)
    {
        //...Create a formatted string for each vertex as a product
        //   of each location hash
        hashSeed1 = myMesh.node[myMesh.element[i].c1-1].locationHash;
        hashSeed2 = myMesh.node[myMesh.element[i].c2-1].locationHash;
        hashSeed3 = myMesh.node[myMesh.element[i].c3-1].locationHash;

        //...Concatenate the formatted strings together
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        hashCSeed = hashSeed.toStdString().c_str();

        //...Create the hash for the local node
        localHash.reset();
        localHash.addData(hashCSeed,120);

        //...Save the local hash for this node into the array
        myMesh.element[i].elementHash = localHash.result().toHex();
    }

    return 0;
}

int adcirc_io::sortAdcircHashes(adcirc_mesh &myMesh)
{
    //...Sort the nodes
    QVector<adcirc_node> nodeList;
    nodeList.resize(myMesh.NumNodes);
    nodeList = myMesh.node;
    qSort(nodeList);
    myMesh.node = nodeList;
    nodeList.clear();

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
    }

    return 0;
}

int adcirc_io::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh)
{
    QString line,tempString;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3,hashSeed4;
    const char* hashCSeed;
    int i;

    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

    //...Compute the full mesh hash by iterating over all the data
    //   contained
    QCryptographicHash fullHash(QCryptographicHash::Sha1);

    //...The full mesh hash is computed with the z-elevations
    //   so we can easily see if two meshes are different in
    //   any way by checking the hash in the header
    for(i=0;i<myMesh.NumNodes;i++)
    {
        hashSeed1.sprintf("%+018.12e",myMesh.node[i].x);
        hashSeed2.sprintf("%+018.12e",myMesh.node[i].y);
        hashSeed3.sprintf("%+018.12e",myMesh.node[i].z);
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        hashCSeed = hashSeed.toStdString().c_str();
        fullHash.addData(hashCSeed,57);
    }
    for(i=0;i<myMesh.NumElements;i++)
    {
        hashSeed1 = myMesh.element[i].elementHash;
        hashSeed2 = myMesh.element[i].h1;
        hashSeed3 = myMesh.element[i].h2;
        hashSeed4 = myMesh.element[i].h3;
        hashSeed = hashSeed1+hashSeed2+hashSeed3+hashSeed4;
        hashCSeed = hashSeed.toStdString().c_str();
        fullHash.addData(hashCSeed,160);
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
    }

    outputFile.close();

    return 0;
}

adcirc_mesh adcirc_io::readAdcircSha1Mesh(QString fileName)
{
    QString tempString;
    QStringList tempList;
    int i;
    adcirc_mesh myMesh;

    QFile meshFile(fileName);
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        myMesh.status = -1;
        return myMesh;
    }

    myMesh.header = meshFile.readLine().simplified();
    myMesh.mesh_hash = meshFile.readLine().simplified();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(1);
    myMesh.NumNodes = tempString.toInt();
    tempString = tempList.value(0);
    myMesh.NumElements = tempString.toInt();

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
    }

    meshFile.close();
    return myMesh;
}

int adcirc_io::numberAdcircMesh(adcirc_mesh &myMesh)
{
    QMap<QString,int> mapping_s2a;
    int i;

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
        mapping_s2a[myMesh.node[i].locationHash] = i;

    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].c1 = mapping_s2a[myMesh.element[i].h1];
        myMesh.element[i].c2 = mapping_s2a[myMesh.element[i].h2];
        myMesh.element[i].c3 = mapping_s2a[myMesh.element[i].h3];
    }

    return 0;
}

int adcirc_io::writeAdcircMesh(QString fileName,adcirc_mesh &myMesh)
{
    QString line;
    int i;
    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

    output << myMesh.header << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line.sprintf("%10i %+18.12e %+18.12e %+18.12e \n",i+1,myMesh.node[i].x,
                     myMesh.node[i].y,myMesh.node[i].z);
        output << line;
    }
    for(i=0;i<myMesh.NumElements;i++)
    {
        line.sprintf("%10i %5i %10i %10i %10i \n",i+1,3,myMesh.element[i].c1+1,
                myMesh.element[i].c2+1,myMesh.element[i].c3+1);
        output << line;
    }
    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    outputFile.close();
    return 0;
}
