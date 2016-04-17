#include "libAdcircHash.h"
#include "metis.h"
#include <QDebug>

bool nodeHashLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    if(node1->positionHash<node2->positionHash)
        return true;
    else
        return false;
}

bool elementHashLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    if(element1->hash<element2->hash)
        return true;
    else
        return false;
}

bool boundaryHashLessThan(const adcirc_boundary *boundary1, const adcirc_boundary *boundary2)
{
    if(boundary1->hash<boundary2->hash)
        return true;
    else
        return false;
}


Adcirc_hashlib::Adcirc_hashlib(QObject *parent) : QObject(parent)
{

}

int Adcirc_hashlib::hashAdcircMesh(QString inputFile, QString outputFile)
{
    int ierr;

    this->mesh = new adcirc_mesh();
    ierr = this->mesh->read(inputFile);
    if(ierr!=ERROR_NOERROR)
        return -1;

    ierr = this->mesh->hashMesh();

    ierr = this->createPartitions();

    //this->organizeHashes();
    //this->writeHashedMesh(outputFile);

    if(ierr!=ERROR_NOERROR)
        return -1;

    return 0;
}


int Adcirc_hashlib::writeHashedMesh(QString outputFile)
{
    int i,j,k;
    QString fileName,file,line,x,y,z,hash;
    QFile thisFile;

    this->buildDirectoryTree(outputFile);

    int nFiles = qPow(16,this->nSplit);

    //...Write the node files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->nodeDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->nodeHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->nodeHashList[i].length();j++)
        {
            hash = this->nodeHashList[i].value(j)->positionHash;
            x.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.x());
            y.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.y());
            z.sprintf("%+018.12e",this->nodeHashList[i].value(j)->position.z());
            line = QString("%1 %2 %3 %4 \n").arg(hash).arg(x).arg(y).arg(z);
            thisFile.write(line.toUtf8());
        }

        thisFile.close();
    }

    //...Write the element files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->elemDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->elementHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->elementHashList[i].length();j++)
        {
            line = QString("%1 %2 %3 %4 \n")
                    .arg(this->elementHashList[i].value(j)->hash)
                    .arg(this->elementHashList[i].value(j)->connections[0]->positionHash)
                    .arg(this->elementHashList[i].value(j)->connections[1]->positionHash)
                    .arg(this->elementHashList[i].value(j)->connections[2]->positionHash);
            thisFile.write(line.toUtf8());
        }
        thisFile.close();
    }

    //...Write the open boundary files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->openBoundDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->openBCHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->openBCHashList[i].length();j++)
        {
            line = QString("%1 %2 \n").arg(this->openBCHashList[i].value(j)->hash).arg(this->openBCHashList[i].value(j)->numNodes);
            thisFile.write(line.toUtf8());
            for(k=0;k<this->openBCHashList[i].value(j)->numNodes;k++)
            {
                line = this->formatBoundaryHashLine(this->openBCHashList[i].value(j),k);
                thisFile.write(line.toUtf8());
            }
        }
        thisFile.close();
    }

    //...Write the land boundary files
    for(i=0;i<nFiles;i++)
    {
        file     = QString::number(i,16);
        if(file.length()<this->nSplit)
            file = "0"+file;
        fileName = this->landBoundDir.path()+"/"+file;
        thisFile.setFileName(fileName);
        if(!thisFile.open(QIODevice::WriteOnly))
            return -1;

        line = QString("%1 \n").arg(this->landBCHashList[i].length());
        thisFile.write(line.toUtf8());

        for(j=0;j<this->landBCHashList[i].length();j++)
        {
            line = QString("%1 %2 \n").arg(this->landBCHashList[i].value(j)->hash).arg(this->landBCHashList[i].value(j)->numNodes);
            thisFile.write(line.toUtf8());
            for(k=0;k<this->landBCHashList[i].value(j)->numNodes;k++)
            {
                line = this->formatBoundaryHashLine(this->landBCHashList[i].value(j),k);
                thisFile.write(line.toUtf8());
            }
        }
        thisFile.close();
    }

    return 0;
}


int Adcirc_hashlib::organizeHashes()
{
    int i,subIndex;
    bool err;
    QString subString;

    this->nodeHashList.resize(qPow(16,this->nSplit));
    this->elementHashList.resize(qPow(16,this->nSplit));
    this->openBCHashList.resize(qPow(16,this->nSplit));
    this->landBCHashList.resize(qPow(16,this->nSplit));

    for(i=0;i<this->mesh->numNodes;i++)
    {
        subString = this->mesh->nodes[i]->positionHash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->nodeHashList[subIndex].append(this->mesh->nodes[i]);
    }

    for(i=0;i<this->mesh->numElements;i++)
    {
        subString = this->mesh->elements[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->elementHashList[subIndex].append(this->mesh->elements[i]);
    }

    for(i=0;i<this->mesh->numOpenBoundaries;i++)
    {
        subString = this->mesh->openBC[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->openBCHashList[subIndex].append(this->mesh->openBC[i]);
    }

    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        subString = this->mesh->landBC[i]->hash.left(this->nSplit);
        subIndex  = subString.toInt(&err,16);
        this->landBCHashList[subIndex].append(this->mesh->landBC[i]);
    }

    for(i=0;i<qPow(16,this->nSplit);i++)
    {
        std::sort(this->nodeHashList[i].begin(),this->nodeHashList[i].end(),nodeHashLessThan);
        std::sort(this->elementHashList[i].begin(),this->elementHashList[i].end(),elementHashLessThan);
        std::sort(this->openBCHashList[i].begin(),this->openBCHashList[i].end(),boundaryHashLessThan);
        std::sort(this->landBCHashList[i].begin(),this->landBCHashList[i].end(),boundaryHashLessThan);
    }

    return 0;
}


int Adcirc_hashlib::buildDirectoryTree(QString directory)
{

    bool err;

    this->myDir.setPath(directory);
    this->nodeDir.setPath(directory+"/nodes");
    this->elemDir.setPath(directory+"/elements");
    this->boundDir.setPath(directory+"/boundaries");
    this->openBoundDir.setPath(directory+"/boundaries/openBoundaries");
    this->landBoundDir.setPath(directory+"/boundaries/landBoundaries");

    //...Create the main directories
    if(!myDir.exists())
    {
        err = QDir().mkdir(myDir.path());
        if(!err)
            return -1;
    }

    if(!nodeDir.exists())
    {
        err = QDir().mkdir(nodeDir.path());
        if(!err)
            return -1;
    }

    if(!elemDir.exists())
    {
        err = QDir().mkdir(elemDir.path());
        if(!err)
            return -1;
    }

    if(!boundDir.exists())
    {
        err = QDir().mkdir(boundDir.path());
        if(!err)
            return -1;
    }

    if(!openBoundDir.exists())
    {
        err = QDir().mkdir(openBoundDir.path());
        if(!err)
            return -1;
    }

    if(!landBoundDir.exists())
    {
        err = QDir().mkdir(landBoundDir.path());
        if(!err)
            return -1;
    }

    return 0;
}


QString Adcirc_hashlib::formatBoundaryHashLine(adcirc_boundary *boundary, int index)
{

    QString line;
    QString crest,super,sub,pipeht,pipediam,pipecoef;

    if(boundary->code==3 || boundary->code==13 || boundary->code==23)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        line = QString("%1 %2 %3 \n").arg(boundary->n1[index]->positionHash)
                                  .arg(crest)
                                  .arg(super);
    }
    else if(boundary->code==4 || boundary->code == 24)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        sub.sprintf("%+018.12e",boundary->supercritical[index]);
        line = QString("%1 %2 %3 %4 %5 \n").arg(boundary->n1[index]->positionHash)
                                        .arg(boundary->n2[index]->positionHash)
                                        .arg(crest)
                                        .arg(sub)
                                        .arg(super);
    }
    else if(boundary->code==5 || boundary->code == 25)
    {
        crest.sprintf("%+018.12e",boundary->crest[index]);
        super.sprintf("%+018.12e",boundary->supercritical[index]);
        sub.sprintf("%+018.12e",boundary->supercritical[index]);
        pipeht.sprintf("%+018.12e",boundary->pipeHeight[index]);
        pipediam.sprintf("%+018.12e",boundary->pipeDiam[index]);
        pipecoef.sprintf("%+018.12e",boundary->pipeCoef[index]);
        line = QString("%1 %2 %3 %4 %5 %6 %7 %8 \n").arg(boundary->n1[index]->positionHash)
                                                 .arg(boundary->n2[index]->positionHash)
                                                 .arg(crest)
                                                 .arg(sub)
                                                 .arg(super)
                                                 .arg(pipeht)
                                                 .arg(pipecoef)
                                                 .arg(pipediam);
    }
    else
        line = QString("%1 \n").arg(boundary->n1[index]->positionHash);

    return line;
}

int Adcirc_hashlib::createPartitions()
{
    QFile debug("C:/Users/zcobell/Documents/Codes/adcirc_hashlib/debug.txt");
    if(!debug.open(QIODevice::WriteOnly))
        return -1;

    QVector<int> nedLoc,nEdges,xAdj;
    QVector<int> vertexWeights,edgeWeights;
    QVector<int> numDuals,itVect,itVect2;
    QVector<QVector<int> > iDuals,coNodes;
    QVector<adcirc_boundary*> boundary;
    QVector<int> node1,node2;
    int i1,i2,i3;
    int i,j,k,jNode;
    int n1,n2,maxDuals,nWeir,nEdgeTot;
    int nNodes,nPart,weightFlag,opType;
    int iNode,nCount,mNed,mNedLoc;
    int options[5];

    options[0] = 1;
    options[1] = 3;
    options[2] = 1;
    options[3] = 3;
    options[4] = 0;
    weightFlag = 0;
    nWeir      = 0;
    opType     = 2;
    nNodes     = this->mesh->numNodes;
    nPart      = 256;

    nedLoc.resize(nNodes);
    nEdges.resize(nNodes);
    xAdj.resize(nNodes+1);
    vertexWeights.resize(nNodes);
    edgeWeights.resize(nNodes);
    numDuals.resize(nNodes);
    itVect.resize(nNodes);
    itVect2.resize(nNodes);

    nedLoc.fill(0);
    nEdges.fill(0);
    xAdj.fill(0);
    vertexWeights.fill(0);
    edgeWeights.fill(0);
    numDuals.fill(0);
    itVect2.fill(0);


    //...This routine largely mimics the setup
    //   that ADCPREP uses for libmetis, so some
    //   things here will look familiar

    //...Step 1: Compute the weir duals
    for(i=0;i<this->mesh->numLandBoundaries;i++)
    {
        if(this->mesh->landBC[i]->code==4  ||
           this->mesh->landBC[i]->code==24 ||
           this->mesh->landBC[i]->code==5  ||
           this->mesh->landBC[i]->code==25 )
        {
            boundary.push_back(this->mesh->landBC[i]);
        }
    }

    for(i=0;i<boundary.length();i++)
    {
        for(j=0;j<boundary[i]->numNodes;j++)
        {
            nWeir = nWeir + 1;
            n1 = boundary[i]->n1[j]->id-1;
            n2 = boundary[i]->n2[j]->id-1;
            numDuals[n1] = numDuals[n1]+1;
            numDuals[n2] = numDuals[n2]+1;
            node1.push_back(n1);
            node2.push_back(n2);
        }
    }

    maxDuals = 0;
    for(i=0;i<nNodes;i++)
        if(maxDuals<numDuals[i])
            maxDuals = numDuals[i];

    iDuals.resize(maxDuals);
    for(i=0;i<maxDuals;i++)
    {
        iDuals[i].resize(nNodes);
        iDuals[i].fill(0);
    }

    for(i=0;i<nWeir;i++)
    {
        for(j=0;j<maxDuals;j++)
        {
            if(iDuals[j][node1[i]]==0)
            {
                iDuals[j][node1[i]] = node2[i];
                break;
            }
        }
    }

    for(i=0;i<nWeir;i++)
    {
        for(j=0;j<maxDuals;j++)
        {
            if(iDuals[j][node2[i]]==0)
            {
                iDuals[j][node2[i]] = node1[i];
                break;
            }
        }
    }

    qDebug() << "Maximum number of duals for any weir node: " << maxDuals;
    qDebug() << "Number of weir node pairs: " << nWeir;

    //...Step 2: Compute total edges and max co-nodes
    mNed = 0;
    for(i=0;i<3;i++)
    {
        for(j=0;j<this->mesh->numElements;j++)
        {
            iNode  = this->mesh->elements[j]->connections[i]->id-1;
            nCount = nedLoc[iNode] + 2;
            mNed   = mNed + 2;
            for(k=0;k<maxDuals;k++)
            {
                if(iDuals[k][iNode]!=0)
                {
                    nCount = nCount + 1;
                    mNed   = mNed + 1;
                }
            }
            nedLoc[iNode] = nCount;
        }
    }

    mNedLoc = 0;
    for(i=0;i<nNodes;i++)
    {
        if(nedLoc[i]>mNedLoc)
            mNedLoc = nedLoc[i];
    }

    qDebug() << "Maximum co-nodes for any node: " << mNedLoc;

    //...Step 3: Compute co-nodes
    coNodes.resize(mNedLoc+1);
    for(i=0;i<mNedLoc+1;i++)
    {
        coNodes[i].resize(nNodes);
        coNodes[i].fill(0);
    }

    for(k=0;k<3;k++)
    {
        switch(k){
            case 0:
                i1=0;
                i2=1;
                i3=2;
                break;
            case 1:
                i1=1;
                i2=2;
                i3=0;
                break;
            case 2:
                i1=2;
                i2=0;
                i3=1;
                break;
        }

        for(i=0;i<this->mesh->numElements;i++)
        {
            iNode = this->mesh->elements[i]->connections[i1]->id;
            coNodes[nEdges[iNode-1]][iNode-1] = this->mesh->elements[i]->connections[i2]->id;
            coNodes[nEdges[iNode-1]+1][iNode-1] = this->mesh->elements[i]->connections[i3]->id;
            nCount = nEdges[iNode-1] + 2;
            for(j=0;j<maxDuals;j++)
            {
                if(iDuals[j][iNode-1]!=0)
                {
                    nCount = nCount + 1;
                    coNodes[nCount][iNode-1] = iDuals[j][iNode-1];
                }
            }
            nEdges[iNode-1] = nCount;
        }
    }

    //...Remove redundancy in node lists
    nEdgeTot = 0;
    for(iNode=0;iNode<nNodes;iNode++)
    {

        itVect.resize(nEdges[iNode]);

        for(j=0;j<nEdges[iNode];j++)
            itVect[j] = coNodes[j][iNode];

        if(nEdges[iNode]>1)
        {
            nCount = nEdges[iNode];
            std::sort(itVect.begin(),itVect.end());
            for(k=0;k<itVect.length();k++)
            {
                debug.write(QString(QString::number(itVect[k])+"\n").toUtf8());
            }
            jNode = itVect[0];
            coNodes[0][iNode] = jNode;
            nCount = 1;
            for(j=1;j<nEdges[iNode];j++)
            {
                if(itVect[j]!=jNode)
                {
                    nCount = nCount + 1;
                    jNode  = itVect[j];
                    coNodes[nCount][iNode] = jNode;
                }
            }
        }
        else
        {
            qDebug() << "node = " << iNode << " is isolated.";
            std::exit(-1);
        }
        nEdges[iNode] = nCount;
        nEdgeTot = nEdgeTot + nCount;
        if(nEdges[iNode]==0)
        {
            qDebug() << "node = " << iNode << " belonds to no edges.";
            std::exit(-1);
        }
    }
    nEdgeTot = nEdgeTot / 2;
    qDebug() << "Edge Count: " << nEdgeTot;

    return 0;

}
