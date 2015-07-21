#include <adcirc_io.h>

adcirc_mesh readAdcircMesh(QString fileName)
{

    //Variables
    int i;

    QString readData;
    QStringList readDataList;

    QTextStream cout(stdout);
    QFile meshFile(fileName);

    adcirc_mesh myMesh;

    //----------------------------------------------------//

    //Check if we can open the file
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        cout << "Error: Could not open the mesh file.";
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
    myMesh.x_location.resize(myMesh.NumNodes);
    myMesh.y_location.resize(myMesh.NumNodes);
    myMesh.z_elevation.resize(myMesh.NumNodes);
    myMesh.connectivity.resize(3);
    for(i=0;i<3;i++)
        myMesh.connectivity[i].resize(myMesh.NumElements);
    myMesh.location_hash.resize(myMesh.NumNodes);
    myMesh.connectivity_hash.resize(myMesh.NumElements);

    //Begin reading the nodes
    for(i = 0; i < myMesh.NumNodes; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(1);
        myMesh.x_location[i] = readData.toDouble();
        readData = readDataList.value(2);
        myMesh.y_location[i] = readData.toDouble();
        readData = readDataList.value(3);
        myMesh.z_elevation[i] = readData.toDouble();
    }

    //Begin reading the elements
    for(i = 0; i< myMesh.NumElements; i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(2);
        myMesh.connectivity[0][i] = readData.toInt();
        readData = readDataList.value(3);
        myMesh.connectivity[1][i] = readData.toInt();
        readData = readDataList.value(4);
        myMesh.connectivity[2][i] = readData.toInt();
    }

    meshFile.close();

    return myMesh;
}

int createAdcircHashes(adcirc_mesh &myMesh)
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
        hashSeed1.sprintf("%+018.12e",myMesh.x_location[i]);
        hashSeed2.sprintf("%+018.12e",myMesh.y_location[i]);

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
        myMesh.location_hash[i] = localHash.result().toHex();
    }

    //...Now create the hash for each element which is based upon the
    //   locations of each hash
    for(i=0;i<myMesh.NumElements;i++)
    {
        //...Create a formatted string for each vertex as a product
        //   of each location hash
        hashSeed1 = myMesh.location_hash[myMesh.connectivity[0][i]-1];
        hashSeed2 = myMesh.location_hash[myMesh.connectivity[1][i]-1];
        hashSeed3 = myMesh.location_hash[myMesh.connectivity[2][i]-1];

        //...Concatenate the formatted strings together
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        hashCSeed = hashSeed.toStdString().c_str();

        //...Create the hash for the local node
        localHash.reset();
        localHash.addData(hashCSeed,120);

        //...Save the local hash for this node into the array
        myMesh.connectivity_hash[i] = localHash.result().toHex();
    }

    return 0;
}

int sortAdcircHashes(adcirc_mesh &myMeshIn, adcirc_mesh &myMeshOut)
{
    int i;
    QVector<QString> dummyNodeList,dummyElementList;
    QString dummyString, tempString;
    QStringList dummy;

    //...Make some dummy arrays that we
    //   can decompose later
    dummyNodeList.resize(myMeshIn.NumNodes);
    dummyElementList.resize(myMeshIn.NumElements);

    //...Build the dummy node array for decomposition later
    for(i=0;i<myMeshIn.NumNodes;i++)
    {
        dummyString = "";
        tempString = myMeshIn.location_hash[i];
        dummyString = tempString;
        tempString.sprintf("%10.10i,%+018.12e,%+018.12e,%+018.12e",
                           i,
                           myMeshIn.x_location[i],
                           myMeshIn.y_location[i],
                           myMeshIn.z_elevation[i]);
        dummyString = dummyString + "," + tempString;
        dummyNodeList[i] = dummyString;
    }


    //...Build the dummy element array for decomposition later
    for(i=0;i<myMeshIn.NumElements;i++)
    {
        dummyString = "";
        tempString = myMeshIn.connectivity_hash[i];
        dummyString = tempString;
        tempString.sprintf("%10.10i",i);
        dummyString = dummyString + "," + tempString;
        tempString  = myMeshIn.location_hash[myMeshIn.connectivity[0][i]-1] + "," +
                      myMeshIn.location_hash[myMeshIn.connectivity[1][i]-1] + "," +
                      myMeshIn.location_hash[myMeshIn.connectivity[2][i]-1];
        dummyString = dummyString + "," + tempString;
        dummyElementList[i] = dummyString;
    }

    //...Sort the hashes
    qSort(dummyNodeList);
    qSort(dummyElementList);

    //...Build the new sorted mesh
    myMeshOut.NumNodes = myMeshIn.NumNodes;
    myMeshOut.NumElements = myMeshIn.NumElements;
    myMeshOut.x_location.resize(myMeshIn.NumNodes);
    myMeshOut.y_location.resize(myMeshIn.NumNodes);
    myMeshOut.z_elevation.resize(myMeshIn.NumNodes);
    myMeshOut.conn_node_hash.resize(3);
    myMeshOut.conn_node_hash[0].resize(myMeshIn.NumElements);
    myMeshOut.conn_node_hash[1].resize(myMeshIn.NumElements);
    myMeshOut.conn_node_hash[2].resize(myMeshIn.NumElements);
    myMeshOut.connectivity_hash.resize(myMeshIn.NumElements);
    myMeshOut.location_hash.resize(myMeshIn.NumNodes);

    //...Sorted nodes
    for(i=0;i<myMeshOut.NumNodes;i++)
    {
        dummy = dummyNodeList[i].split(",");
        myMeshOut.location_hash[i] = dummy.value(0);
        tempString = dummy.value(2);
        myMeshOut.x_location[i] = tempString.toDouble();
        tempString = dummy.value(3);
        myMeshOut.y_location[i] = tempString.toDouble();
        tempString = dummy.value(4);
        myMeshOut.z_elevation[i] = tempString.toDouble();
    }

    //...Sorted elements
    for(i=0;i<myMeshOut.NumElements;i++)
    {
        dummy = dummyElementList[i].split(",");
        myMeshOut.connectivity_hash[i] = dummy.value(0);
        myMeshOut.conn_node_hash[0][i] = dummy.value(2);
        myMeshOut.conn_node_hash[1][i] = dummy.value(3);
        myMeshOut.conn_node_hash[2][i] = dummy.value(4);
    }

    myMeshOut.header = myMeshIn.header;

    //...Boundaries not part of code yet

    return 0;
}

int writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh)
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
        hashSeed1.sprintf("%+018.12e",myMesh.x_location[i]);
        hashSeed2.sprintf("%+018.12e",myMesh.y_location[i]);
        hashSeed3.sprintf("%+018.12e",myMesh.z_elevation[i]);
        hashSeed = hashSeed1+hashSeed2+hashSeed3;
        hashCSeed = hashSeed.toStdString().c_str();
        fullHash.addData(hashCSeed,57);
    }
    for(i=0;i<myMesh.NumElements;i++)
    {
        hashSeed1 = myMesh.connectivity_hash[i];
        hashSeed2 = myMesh.conn_node_hash[0][i];
        hashSeed3 = myMesh.conn_node_hash[1][i];
        hashSeed4 = myMesh.conn_node_hash[2][i];
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
        line = myMesh.location_hash[i];
        line = line + " " + tempString.sprintf("%+18.12e %+18.12e %+18.12e \n",
                                         myMesh.x_location[i],
                                         myMesh.y_location[i],
                                         myMesh.z_elevation[i]);
        output << line;
    }

    //...Write the element connectivity
    for(i=0;i<myMesh.NumElements;i++)
    {
        line = "";
        line = myMesh.connectivity_hash[i];
        line = line + " " + myMesh.conn_node_hash[0][i] + " " +
                myMesh.conn_node_hash[1][i] + " " +
                myMesh.conn_node_hash[2][i] + "\n";
        output << line;
    }

    outputFile.close();

    return 0;
}

adcirc_mesh readAdcircSha1Mesh(QString fileName)
{
    QTextStream cout(stdout);
    QString tempString;
    QStringList tempList;
    int i;
    adcirc_mesh myMesh;

    QFile meshFile(fileName);
    if(!meshFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        cout << "Error: Could not open the mesh file.";
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

    myMesh.x_location.resize(myMesh.NumNodes);
    myMesh.y_location.resize(myMesh.NumNodes);
    myMesh.z_elevation.resize(myMesh.NumNodes);
    myMesh.location_hash.resize(myMesh.NumNodes);
    myMesh.connectivity_hash.resize(myMesh.NumElements);
    myMesh.connectivity.resize(3);
    myMesh.connectivity[0].resize(myMesh.NumElements);
    myMesh.connectivity[1].resize(myMesh.NumElements);
    myMesh.connectivity[2].resize(myMesh.NumElements);
    myMesh.conn_node_hash.resize(3);
    myMesh.conn_node_hash[0].resize(myMesh.NumElements);
    myMesh.conn_node_hash[1].resize(myMesh.NumElements);
    myMesh.conn_node_hash[2].resize(myMesh.NumElements);


    //...Reading the nodes
    for(i=0;i<myMesh.NumNodes;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.location_hash[i] = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.x_location[i] = tempString.toDouble();
        tempString = tempList.value(2);
        myMesh.y_location[i] = tempString.toDouble();
        tempString = tempList.value(3);
        myMesh.z_elevation[i] = tempString.toDouble();
    }

    //...Reading the elements
    for(i=0;i<myMesh.NumElements;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.connectivity_hash[i] = tempList.value(0);
        myMesh.conn_node_hash[0][i] = tempList.value(1);
        myMesh.conn_node_hash[1][i] = tempList.value(2);
        myMesh.conn_node_hash[2][i] = tempList.value(3);
    }

    meshFile.close();
    return myMesh;
}

int numberAdcircMesh(adcirc_mesh &myMesh)
{
    QMap<QString,int> mapping_s2a;
    QMap<int,QString> mapping_a2s;
    int i;

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
    {
        mapping_s2a[myMesh.location_hash[i]] = i;
        mapping_a2s[i] = myMesh.location_hash[i];
    }

    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.connectivity[0][i] = mapping_s2a[myMesh.conn_node_hash[0][i]];
        myMesh.connectivity[1][i] = mapping_s2a[myMesh.conn_node_hash[1][i]];
        myMesh.connectivity[2][i] = mapping_s2a[myMesh.conn_node_hash[2][i]];
    }

    return 0;
}

int writeAdcircMesh(QString fileName,adcirc_mesh &myMesh)
{
    QTextStream cout(stdout);
    QString line;
    int i;
    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

    output << myMesh.header << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line.sprintf("%10i %+18.12e %+18.12e %+18.12e \n",i+1,myMesh.x_location[i],
                     myMesh.y_location[i],myMesh.z_elevation[i]);
        output << line;
    }
    for(i=0;i<myMesh.NumElements;i++)
    {
        line.sprintf("%10i %5i %10i %10i %10i \n",i+1,3,myMesh.connectivity[0][i]+1,
                myMesh.connectivity[1][i]+1,myMesh.connectivity[2][i]+1);
        output << line;
    }
    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    output << 0 << "\n";
    outputFile.close();
    return 0;
}
