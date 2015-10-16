#include "adcirc_io.h"

#ifdef GUI
//...Some checks for the progress bar
QTime polling;
int progressUpdateInterval = 10;
#endif

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

//...Comparison operator used in the sorting of boundaries
bool operator< (const adcirc_boundary_hash &first, const adcirc_boundary_hash &second)
{
    if(first.boundary_hash<second.boundary_hash)
        return true;
    else
        return false;
}

//...Comparison operator used in the sorting of boundaries
bool operator< (const adcirc_boundary &first, const adcirc_boundary &second)
{
    if(first.averageLongitude<second.averageLongitude)
        return false;
    else
        return true;
}

#ifdef GUI
int adcirc_io::readAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::readAdcircMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    //Variables
    int i,j;

    QString readData;
    QStringList readDataList;

    QFile meshFile(fileName);

#ifdef GUI
    dialog.setLabelText("Reading the ADCIRC mesh...");
    dialog.setValue(0);
#endif

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

#ifdef GUI
    dialog.setMaximum(myMesh.NumNodes*4+myMesh.NumElements*5);
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //Begin reading the boundaries

    //Open Boundaries
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumOpenBoundaries = readData.toInt();
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumOpenBoundaryNodes = readData.toInt();
    myMesh.openBoundaryH.resize(myMesh.NumOpenBoundaries);
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(0);
        myMesh.openBoundaryH[i].NumNodes = readData.toInt();
        myMesh.openBoundaryH[i].code = -1;
        myMesh.openBoundaryH[i].node1.resize(myMesh.openBoundaryH[i].NumNodes);
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
        {
            readData = meshFile.readLine().simplified();
            myMesh.openBoundaryH[i].node1[j] = readData.toInt();
        }
    }

    //Land Boundaries
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumLandBoundaries = readData.toInt();
    readData = meshFile.readLine().simplified();
    readDataList = readData.split(" ");
    readData = readDataList.value(0);
    myMesh.NumLandBoundaryNodes = readData.toInt();
    myMesh.landBoundaryH.resize(myMesh.NumLandBoundaries);

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        readData = meshFile.readLine().simplified();
        readDataList = readData.split(" ");
        readData = readDataList.value(0);
        myMesh.landBoundaryH[i].NumNodes = readData.toInt();
        readData = readDataList.value(1);
        myMesh.landBoundaryH[i].code = readData.toInt();

        //Read the boundary string appropriately
        if(myMesh.landBoundaryH[i].code  == 0   ||
            myMesh.landBoundaryH[i].code == 1   ||
            myMesh.landBoundaryH[i].code == 2   ||
            myMesh.landBoundaryH[i].code == 10  ||
            myMesh.landBoundaryH[i].code == 11  ||
            myMesh.landBoundaryH[i].code == 12  ||
            myMesh.landBoundaryH[i].code == 20  ||
            myMesh.landBoundaryH[i].code == 21  ||
            myMesh.landBoundaryH[i].code == 22  ||
            myMesh.landBoundaryH[i].code == 30  ||
            myMesh.landBoundaryH[i].code == 52  ||
            myMesh.landBoundaryH[i].code == 102 ||
            myMesh.landBoundaryH[i].code == 112 ||
            myMesh.landBoundaryH[i].code == 122 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].subcritical.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].node2[j] = readData.toInt();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(3);
                myMesh.landBoundaryH[i].subcritical[j] = readData.toDouble();
                readData = readDataList.value(4);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].elevation.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].supercritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].subcritical.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_ht.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_coef.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].pipe_diam.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                readDataList = readData.split(" ");
                readData = readDataList.value(0);
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
                readData = readDataList.value(1);
                myMesh.landBoundaryH[i].node2[j] = readData.toInt();
                readData = readDataList.value(2);
                myMesh.landBoundaryH[i].elevation[j] = readData.toDouble();
                readData = readDataList.value(3);
                myMesh.landBoundaryH[i].subcritical[j] = readData.toDouble();
                readData = readDataList.value(4);
                myMesh.landBoundaryH[i].supercritical[j] = readData.toDouble();
                readData = readDataList.value(5);
                myMesh.landBoundaryH[i].pipe_ht[j] = readData.toDouble();
                readData = readDataList.value(6);
                myMesh.landBoundaryH[i].pipe_coef[j] = readData.toDouble();
                readData = readDataList.value(7);
                myMesh.landBoundaryH[i].pipe_diam[j] = readData.toDouble();
            }
        }
        else
        {
            myMesh.landBoundaryH[i].node1.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                readData = meshFile.readLine().simplified();
                myMesh.landBoundaryH[i].node1[j] = readData.toInt();
            }
        }
    }

    meshFile.close();

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::createAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::createAdcircHashes(adcirc_mesh &myMesh)
#endif
{

    //...variables
    int i,j;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3;
#ifdef GUI
    dialog.setLabelText("Hashing the ADCIRC mesh...");
#endif

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
        localHash.addData(hashSeed.toUtf8(),hashSeed.length());

        //...Save the local hash for this node into the array
        myMesh.node[i].locationHash = localHash.result().toHex();

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

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
        localHash.addData(hashSeed.toUtf8(),hashSeed.length());

        //...Save the local hash for this node into the array
        myMesh.element[i].elementHash = localHash.result().toHex();

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        localHash.reset();

        //Hash the boundary code, for open boundaries, -1
        hashSeed.sprintf("%+6i",-1);
        localHash.addData(hashSeed.toUtf8(),hashSeed.length());

        //Accumulate a hash along the boundary string
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
        {
            hashSeed = QString();
            hashSeed = myMesh.node[myMesh.openBoundaryH[i].node1[j]-1].locationHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }

        //Save the hash
        myMesh.openBoundaryH[i].boundary_hash = localHash.result().toHex();
    }

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        localHash.reset();

        //Hash the boundary code
        hashSeed.sprintf("%+6i",myMesh.landBoundaryH[i].code);
        localHash.addData(hashSeed.toUtf8(),hashSeed.length());

        //Accumulate a hash along the boundary string
        for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
        {

            //Single node boundaries and dual node boundaries
            //lumped since we only care if the position
            //changes, not the attributes
            if(myMesh.landBoundaryH[i].code  == 0   ||
                myMesh.landBoundaryH[i].code == 1   ||
                myMesh.landBoundaryH[i].code == 2   ||
                myMesh.landBoundaryH[i].code == 10  ||
                myMesh.landBoundaryH[i].code == 11  ||
                myMesh.landBoundaryH[i].code == 12  ||
                myMesh.landBoundaryH[i].code == 20  ||
                myMesh.landBoundaryH[i].code == 21  ||
                myMesh.landBoundaryH[i].code == 22  ||
                myMesh.landBoundaryH[i].code == 30  ||
                myMesh.landBoundaryH[i].code == 52  ||
                myMesh.landBoundaryH[i].code == 102 ||
                myMesh.landBoundaryH[i].code == 112 ||
                myMesh.landBoundaryH[i].code == 122 ||
                myMesh.landBoundaryH[i].code == 3   ||
                myMesh.landBoundaryH[i].code == 13  ||
                myMesh.landBoundaryH[i].code == 23 )
            {
                //Accumulate a hash along the boundary string
                for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                {
                    hashSeed = QString();
                    hashSeed = myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
                    localHash.addData(hashSeed.toUtf8(),hashSeed.length());
                }
            }
            else if(myMesh.landBoundaryH[i].code == 4  ||
                    myMesh.landBoundaryH[i].code == 24 ||
                    myMesh.landBoundaryH[i].code == 5  ||
                    myMesh.landBoundaryH[i].code == 25 )
            {
                //Accumulate a hash along the boundary string
                for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                {
                    hashSeed = QString();
                    hashSeed = myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash +
                               myMesh.node[myMesh.landBoundaryH[i].node2[j]-1].locationHash;
                    localHash.addData(hashSeed.toUtf8(),hashSeed.length());
                }
            }
            else
            {
                //Accumulate a hash along the boundary string
                for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                {
                    hashSeed = QString();
                    hashSeed = myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
                    localHash.addData(hashSeed.toUtf8(),hashSeed.length());
                }
            }
        }

        //Save the hash
        myMesh.landBoundaryH[i].boundary_hash = localHash.result().toHex();
    }

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::sortAdcircHashes(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::sortAdcircHashes(adcirc_mesh &myMesh)
#endif
{
    int i,j;

#ifdef GUI
    dialog.setLabelText("Sorting the hashes...");
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Sort the open boundary segments
    QVector<adcirc_boundary_hash> openboundaryList;
    openboundaryList.resize(myMesh.NumOpenBoundaries);
    openboundaryList = myMesh.openBoundaryH;
    qSort(openboundaryList);

    //...Clear the information from the mesh about open boundaries
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
        myMesh.openBoundaryH[i].node1.clear();
    myMesh.openBoundaryH.clear();

    //...Save the sorted information
    myMesh.openBoundaryH = openboundaryList;

    //...Hash the open boundary array
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        myMesh.openBoundaryH[i].node1_hash.resize(myMesh.openBoundaryH[i].NumNodes);
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
            myMesh.openBoundaryH[i].node1_hash[j] =
                    myMesh.node[myMesh.openBoundaryH[i].node1[j]-1].locationHash;
    }

    //...Sort the land boundary segments
    QVector<adcirc_boundary_hash> landBoundaryList;
    landBoundaryList.resize(myMesh.NumLandBoundaries);
    landBoundaryList = myMesh.landBoundaryH;
    qSort(landBoundaryList);

    //...Clear the boundary arrays
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        myMesh.landBoundaryH[i].node1.clear();
        myMesh.landBoundaryH[i].node2.clear();
        myMesh.landBoundaryH[i].elevation.clear();
        myMesh.landBoundaryH[i].subcritical.clear();
        myMesh.landBoundaryH[i].supercritical.clear();
        myMesh.landBoundaryH[i].pipe_ht.clear();
        myMesh.landBoundaryH[i].pipe_coef.clear();
        myMesh.landBoundaryH[i].pipe_diam.clear();
    }

    //...Transfer the new boundary array
    myMesh.landBoundaryH = landBoundaryList;
    landBoundaryList.clear();

    //Accumulate a hash along the boundary string
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        if(myMesh.landBoundaryH[i].code  == 0   ||
            myMesh.landBoundaryH[i].code == 1   ||
            myMesh.landBoundaryH[i].code == 2   ||
            myMesh.landBoundaryH[i].code == 10  ||
            myMesh.landBoundaryH[i].code == 11  ||
            myMesh.landBoundaryH[i].code == 12  ||
            myMesh.landBoundaryH[i].code == 20  ||
            myMesh.landBoundaryH[i].code == 21  ||
            myMesh.landBoundaryH[i].code == 22  ||
            myMesh.landBoundaryH[i].code == 30  ||
            myMesh.landBoundaryH[i].code == 52  ||
            myMesh.landBoundaryH[i].code == 102 ||
            myMesh.landBoundaryH[i].code == 112 ||
            myMesh.landBoundaryH[i].code == 122 ||
            myMesh.landBoundaryH[i].code == 3   ||
            myMesh.landBoundaryH[i].code == 13  ||
            myMesh.landBoundaryH[i].code == 23 )
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 ||
                myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            myMesh.landBoundaryH[i].node2_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
                myMesh.landBoundaryH[i].node2_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node2[j]-1].locationHash;
            }
        }
        else
        {
            myMesh.landBoundaryH[i].node1_hash.resize(myMesh.landBoundaryH[i].NumNodes);
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                myMesh.landBoundaryH[i].node1_hash[j] =
                        myMesh.node[myMesh.landBoundaryH[i].node1[j]-1].locationHash;
        }
    }

    //...Transfer the sorted node list
    myMesh.node = nodeList;
    nodeList.clear();

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::writeAdcircHashMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString line,tempString;
    QString elevation,supercritical,subcritical,pipeht,pipediam,pipecoef;
    QString hashSeed,hashSeed1,hashSeed2,hashSeed3,hashSeed4;
    int i,j;

    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

#ifdef GUI
    dialog.setLabelText("Writing the hashed ADCIRC mesh...");
#endif

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
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumElements;i++)
    {
        hashSeed1 = myMesh.element[i].elementHash;
        hashSeed2 = myMesh.element[i].h1;
        hashSeed3 = myMesh.element[i].h2;
        hashSeed4 = myMesh.element[i].h3;
        hashSeed = hashSeed1+hashSeed2+hashSeed3+hashSeed4;
        fullHash.addData(hashSeed.toUtf8(),hashSeed.length());

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
        {
            hashSeed = myMesh.openBoundaryH[i].node1_hash[j];
            fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        if(myMesh.landBoundaryH[i].code  == 0   ||
            myMesh.landBoundaryH[i].code == 1   ||
            myMesh.landBoundaryH[i].code == 2   ||
            myMesh.landBoundaryH[i].code == 10  ||
            myMesh.landBoundaryH[i].code == 11  ||
            myMesh.landBoundaryH[i].code == 12  ||
            myMesh.landBoundaryH[i].code == 20  ||
            myMesh.landBoundaryH[i].code == 21  ||
            myMesh.landBoundaryH[i].code == 22  ||
            myMesh.landBoundaryH[i].code == 30  ||
            myMesh.landBoundaryH[i].code == 52  ||
            myMesh.landBoundaryH[i].code == 102 ||
            myMesh.landBoundaryH[i].code == 112 ||
            myMesh.landBoundaryH[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {

            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             elevation+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             myMesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_diam[j]);
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j]+
                             myMesh.landBoundaryH[i].node2_hash[j]+
                             elevation+subcritical+supercritical+pipeht+
                             pipecoef+pipediam;
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                hashSeed = myMesh.landBoundaryH[i].node1_hash[j];
                fullHash.addData(hashSeed.toUtf8(),hashSeed.length());
            }
        }
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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif

    }

    //...Write the open boundaries
    output << myMesh.NumOpenBoundaries << " \n";
    output << myMesh.NumOpenBoundaryNodes << " \n";
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        output << myMesh.openBoundaryH[i].boundary_hash << "  "
               << myMesh.openBoundaryH[i].NumNodes << " \n";
        for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
            output << myMesh.openBoundaryH[i].node1_hash[j] << " \n";
    }

    //...Write the land boundaries
    output << myMesh.NumLandBoundaries << " \n";
    output << myMesh.NumLandBoundaryNodes << " \n";
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        output << myMesh.landBoundaryH[i].boundary_hash << "  "
               << myMesh.landBoundaryH[i].NumNodes << "   "
               << myMesh.landBoundaryH[i].code << " \n";
        if(myMesh.landBoundaryH[i].code  == 0   ||
            myMesh.landBoundaryH[i].code == 1   ||
            myMesh.landBoundaryH[i].code == 2   ||
            myMesh.landBoundaryH[i].code == 10  ||
            myMesh.landBoundaryH[i].code == 11  ||
            myMesh.landBoundaryH[i].code == 12  ||
            myMesh.landBoundaryH[i].code == 20  ||
            myMesh.landBoundaryH[i].code == 21  ||
            myMesh.landBoundaryH[i].code == 22  ||
            myMesh.landBoundaryH[i].code == 30  ||
            myMesh.landBoundaryH[i].code == 52  ||
            myMesh.landBoundaryH[i].code == 102 ||
            myMesh.landBoundaryH[i].code == 112 ||
            myMesh.landBoundaryH[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
                output << myMesh.landBoundaryH[i].node1_hash[j] << " \n";
        }
        else if(myMesh.landBoundaryH[i].code == 3  ||
                myMesh.landBoundaryH[i].code == 13 ||
                myMesh.landBoundaryH[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " "
                       << elevation << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundaryH[i].code == 4  ||
                myMesh.landBoundaryH[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " "
                       << myMesh.landBoundaryH[i].node2_hash[j] << " "
                       << elevation << " " << subcritical << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundaryH[i].code == 5  ||
                myMesh.landBoundaryH[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundaryH[i].NumNodes;j++)
            {
                elevation.sprintf("%+18.12e",myMesh.landBoundaryH[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundaryH[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundaryH[i].pipe_diam[j]);
                output << myMesh.landBoundaryH[i].node1_hash[j] << " " <<
                          myMesh.landBoundaryH[i].node2_hash[j] << " " <<
                          elevation << " " << subcritical << " " << supercritical <<
                          pipeht << " " << pipecoef << " " << pipediam << " \n";
            }
        }
        else
        {
            for(j=0;j<myMesh.openBoundaryH[i].NumNodes;j++)
                output << myMesh.landBoundaryH[i].node1_hash[j] << " \n";
        }
    }

    outputFile.close();

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::readAdcircSha1Mesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString tempString;
    QStringList tempList;
    int i,j;

#ifdef GUI
    dialog.setLabelText("Reading the hashed ADCIRC mesh...");
#endif

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

#ifdef GUI
    dialog.setMaximum((myMesh.NumNodes+myMesh.NumElements)*3);
#endif

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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
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

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Read the open boundaries
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumOpenBoundaries = tempString.toInt();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumOpenBoundaryNodes = tempString.toInt();

    myMesh.openBoundary.resize(myMesh.NumOpenBoundaries);
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.openBoundary[i].boundary_hash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.openBoundary[i].NumNodes = tempString.toInt();
        myMesh.openBoundary[i].node1.resize(myMesh.openBoundary[i].NumNodes);
        myMesh.openBoundary[i].node1_hash.resize(myMesh.openBoundary[i].NumNodes);
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
        {
            tempString = meshFile.readLine().simplified();
            myMesh.openBoundary[i].node1_hash[j] = tempString;
        }
    }

    //...Read the land boundaries
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumLandBoundaries = tempString.toInt();
    tempString = meshFile.readLine().simplified();
    tempList = tempString.split(" ");
    tempString = tempList.value(0);
    myMesh.NumLandBoundaryNodes = tempString.toInt();

    myMesh.landBoundary.resize(myMesh.NumLandBoundaries);
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        tempString = meshFile.readLine().simplified();
        tempList = tempString.split(" ");
        myMesh.landBoundary[i].boundary_hash = tempList.value(0);
        tempString = tempList.value(1);
        myMesh.landBoundary[i].NumNodes = tempString.toInt();
        tempString = tempList.value(2);
        myMesh.landBoundary[i].code = tempString.toInt();

        if(myMesh.landBoundary[i].code  == 0   ||
            myMesh.landBoundary[i].code == 1   ||
            myMesh.landBoundary[i].code == 2   ||
            myMesh.landBoundary[i].code == 10  ||
            myMesh.landBoundary[i].code == 11  ||
            myMesh.landBoundary[i].code == 12  ||
            myMesh.landBoundary[i].code == 20  ||
            myMesh.landBoundary[i].code == 21  ||
            myMesh.landBoundary[i].code == 22  ||
            myMesh.landBoundary[i].code == 30  ||
            myMesh.landBoundary[i].code == 52  ||
            myMesh.landBoundary[i].code == 102 ||
            myMesh.landBoundary[i].code == 112 ||
            myMesh.landBoundary[i].code == 122 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                myMesh.landBoundary[i].node1_hash[j] = tempString;
            }
        }
        else if(myMesh.landBoundary[i].code == 3  ||
                myMesh.landBoundary[i].code == 13 ||
                myMesh.landBoundary[i].code == 23 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                tempString = tempList.value(1);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(2);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
            }
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].subcritical.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                myMesh.landBoundary[i].node2_hash[j] = tempList.value(1);
                tempString = tempList.value(2);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(3);
                myMesh.landBoundary[i].subcritical[j] = tempString.toDouble();
                tempString = tempList.value(4);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
            }
        }
        else if(myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node2_hash.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].elevation.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].supercritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].subcritical.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_ht.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_coef.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].pipe_diam.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                tempList = tempString.split(" ");
                myMesh.landBoundary[i].node1_hash[j] = tempList.value(0);
                myMesh.landBoundary[i].node2_hash[j] = tempList.value(1);
                tempString = tempList.value(2);
                myMesh.landBoundary[i].elevation[j] = tempString.toDouble();
                tempString = tempList.value(3);
                myMesh.landBoundary[i].subcritical[j] = tempString.toDouble();
                tempString = tempList.value(4);
                myMesh.landBoundary[i].supercritical[j] = tempString.toDouble();
                tempString = tempList.value(5);
                myMesh.landBoundary[i].pipe_ht[j] = tempString.toDouble();
                tempString = tempList.value(6);
                myMesh.landBoundary[i].pipe_coef[j] = tempString.toDouble();
                tempString = tempList.value(7);
                myMesh.landBoundary[i].pipe_diam[j] = tempString.toDouble();
            }
        }
        else
        {
            myMesh.landBoundary[i].node1.resize(myMesh.landBoundary[i].NumNodes);
            myMesh.landBoundary[i].node1_hash.resize(myMesh.landBoundary[i].NumNodes);
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                tempString = meshFile.readLine().simplified();
                myMesh.landBoundary[i].node1_hash[j] = tempString;
            }
        }
    }

    meshFile.close();
    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::numberAdcircMesh(adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::numberAdcircMesh(adcirc_mesh &myMesh)
#endif
{
    QMap<QString,int> mapping_s2a;
    int i,j;
    double a;

#ifdef GUI
    dialog.setLabelText("Numbering the ADCIRC mesh...");
#endif

    //...Create a mapping table
    for(i=0;i<myMesh.NumNodes;i++)
    {
        mapping_s2a[myMesh.node[i].locationHash] = i;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Generate the element table
    for(i=0;i<myMesh.NumElements;i++)
    {
        myMesh.element[i].c1 = mapping_s2a[myMesh.element[i].h1];
        myMesh.element[i].c2 = mapping_s2a[myMesh.element[i].h2];
        myMesh.element[i].c3 = mapping_s2a[myMesh.element[i].h3];

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    //...Map the open boundary arrays
    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
            myMesh.openBoundary[i].node1[j] = mapping_s2a[myMesh.openBoundary[i].node1_hash[j]];
    }

    //...Map the land boundary array
    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        if(myMesh.landBoundary[i].code  == 0   ||
            myMesh.landBoundary[i].code == 1   ||
            myMesh.landBoundary[i].code == 2   ||
            myMesh.landBoundary[i].code == 10  ||
            myMesh.landBoundary[i].code == 11  ||
            myMesh.landBoundary[i].code == 12  ||
            myMesh.landBoundary[i].code == 20  ||
            myMesh.landBoundary[i].code == 21  ||
            myMesh.landBoundary[i].code == 22  ||
            myMesh.landBoundary[i].code == 30  ||
            myMesh.landBoundary[i].code == 52  ||
            myMesh.landBoundary[i].code == 102 ||
            myMesh.landBoundary[i].code == 112 ||
            myMesh.landBoundary[i].code == 122 ||
            myMesh.landBoundary[i].code == 3   ||
            myMesh.landBoundary[i].code == 13  ||
            myMesh.landBoundary[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 ||
                myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
                myMesh.landBoundary[i].node2[j] = mapping_s2a[myMesh.landBoundary[i].node2_hash[j]];
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
                myMesh.landBoundary[i].node1[j] = mapping_s2a[myMesh.landBoundary[i].node1_hash[j]];
        }
    }

    //...Now put the boundaries in East --> West order
    QVector<adcirc_boundary> openBoundary;
    QVector<adcirc_boundary> landBoundary;
    openBoundary = myMesh.openBoundary;
    landBoundary = myMesh.landBoundary;

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        a = 0.0;
        for(j=0;j<openBoundary[i].NumNodes;j++)
            a += myMesh.node[openBoundary[i].node1[j]].x;
        a = a / openBoundary[i].NumNodes;
        openBoundary[i].averageLongitude = a;
    }

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        a = 0.0;
        for(j=0;j<landBoundary[i].NumNodes;j++)
            a += myMesh.node[landBoundary[i].node1[j]].x;
        a = a / landBoundary[i].NumNodes;
        landBoundary[i].averageLongitude = a;
    }

    qSort(openBoundary);
    qSort(landBoundary);

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
        myMesh.openBoundary[i].node1.clear();
    myMesh.openBoundary.clear();

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        myMesh.landBoundary[i].node1.clear();
        myMesh.landBoundary[i].node2.clear();
        myMesh.landBoundary[i].elevation.clear();
        myMesh.landBoundary[i].subcritical.clear();
        myMesh.landBoundary[i].supercritical.clear();
        myMesh.landBoundary[i].pipe_ht.clear();
        myMesh.landBoundary[i].pipe_coef.clear();
        myMesh.landBoundary[i].pipe_diam.clear();
    }
    myMesh.landBoundary.clear();

    myMesh.openBoundary = openBoundary;
    myMesh.landBoundary = landBoundary;

    return ERR_NOERR;
}

#ifdef GUI
int adcirc_io::writeAdcircMesh(QString fileName, adcirc_mesh &myMesh, QProgressDialog &dialog, int &counter)
#else
int adcirc_io::writeAdcircMesh(QString fileName, adcirc_mesh &myMesh)
#endif
{
    QString line,elevation,supercritical,subcritical,pipeht,pipecoef,pipediam,node1,node2;
    int i,j;
    QFile outputFile(fileName);
    QTextStream output(&outputFile);
    outputFile.open(QIODevice::WriteOnly);

#ifdef GUI
    //...Set the progress bar update time
    polling = QTime::currentTime().addMSecs(100);

    dialog.setLabelText("Writing the ADCIRC mesh...");
#endif

    output << myMesh.header << "\n";
    output << myMesh.NumElements << " " << myMesh.NumNodes << "\n";
    for(i=0;i<myMesh.NumNodes;i++)
    {
        line.sprintf("%10i %+18.12e %+18.12e %+18.12e \n",i+1,myMesh.node[i].x,
                     myMesh.node[i].y,myMesh.node[i].z);
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    for(i=0;i<myMesh.NumElements;i++)
    {
        line.sprintf("%10i %5i %10i %10i %10i \n",i+1,3,myMesh.element[i].c1+1,
                myMesh.element[i].c2+1,myMesh.element[i].c3+1);
        output << line;

#ifdef GUI
        //...Update the progress bar
        updateProgress(counter,dialog);

        //...Catch the cancelled signal
        if(dialog.wasCanceled())
            return ERR_CANCELED;
#endif
    }

    output << myMesh.NumOpenBoundaries << " \n";
    output << myMesh.NumOpenBoundaryNodes << " \n";

    for(i=0;i<myMesh.NumOpenBoundaries;i++)
    {
        output << myMesh.openBoundary[i].NumNodes << " \n";
        for(j=0;j<myMesh.openBoundary[i].NumNodes;j++)
        {
            node1.sprintf("%10i",myMesh.openBoundary[i].node1[j]+1);
            output << node1 << " \n";
        }
    }

    output << myMesh.NumLandBoundaries << " \n";
    output << myMesh.NumLandBoundaryNodes << " \n";

    for(i=0;i<myMesh.NumLandBoundaries;i++)
    {
        output << myMesh.landBoundary[i].NumNodes << " "
               <<  myMesh.landBoundary[i].code << " \n";
        if(myMesh.landBoundary[i].code  == 0   ||
            myMesh.landBoundary[i].code == 1   ||
            myMesh.landBoundary[i].code == 2   ||
            myMesh.landBoundary[i].code == 10  ||
            myMesh.landBoundary[i].code == 11  ||
            myMesh.landBoundary[i].code == 12  ||
            myMesh.landBoundary[i].code == 20  ||
            myMesh.landBoundary[i].code == 21  ||
            myMesh.landBoundary[i].code == 22  ||
            myMesh.landBoundary[i].code == 30  ||
            myMesh.landBoundary[i].code == 52  ||
            myMesh.landBoundary[i].code == 102 ||
            myMesh.landBoundary[i].code == 112 ||
            myMesh.landBoundary[i].code == 122 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                output << node1 << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 3  ||
                myMesh.landBoundary[i].code == 13 ||
                myMesh.landBoundary[i].code == 23 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                output << node1 << " " << elevation << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 4  ||
                myMesh.landBoundary[i].code == 24 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                node2.sprintf("%10i",myMesh.landBoundary[i].node2[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundary[i].subcritical[j]);
                output << node1 << " " << node2 << " " << elevation << " "
                       << subcritical << " " << supercritical << " \n";
            }
        }
        else if(myMesh.landBoundary[i].code == 5  ||
                myMesh.landBoundary[i].code == 25 )
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                node2.sprintf("%10i",myMesh.landBoundary[i].node2[j]+1);
                elevation.sprintf("%+18.12e",myMesh.landBoundary[i].elevation[j]);
                supercritical.sprintf("%+18.12e",myMesh.landBoundary[i].supercritical[j]);
                subcritical.sprintf("%+18.12e",myMesh.landBoundary[i].subcritical[j]);
                pipeht.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_ht[j]);
                pipecoef.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_coef[j]);
                pipediam.sprintf("%+18.12e",myMesh.landBoundary[i].pipe_diam[j]);
                output << node1 << " " << node2 << " " << elevation << " "
                       << subcritical << " " << supercritical << " "
                       << pipeht << " " << pipecoef << " " << pipediam << " \n";
            }
        }
        else
        {
            for(j=0;j<myMesh.landBoundary[i].NumNodes;j++)
            {
                node1.sprintf("%10i",myMesh.landBoundary[i].node1[j]+1);
                output << node1 << " \n";
            }
        }

    }
    outputFile.close();
    return ERR_NOERR;
}

#ifdef GUI
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
#endif
