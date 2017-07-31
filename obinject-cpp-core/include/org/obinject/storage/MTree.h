/*
Copyright (C) 2013     Enzo Seraphim

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
or visit <http://www.gnu.org/licenses/>
 */
#ifndef ORG_OBINJECT_STORAGE_MTREE_H
#define	ORG_OBINJECT_STORAGE_MTREE_H

#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <queue>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/device/Workspace.h>
#include <org/obinject/device/Session.h>
#include <org/obinject/queries/AveragePerformance.h>
#include <org/obinject/block/MTreeDescriptor.h>
#include <org/obinject/block/MTreeLeaf.h>
#include <org/obinject/block/MTreeIndex.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Key.h>

using namespace std;
using namespace std::chrono;

namespace org {
namespace obinject {
namespace storage {

template <typename K>
class MTree : public AbstractKeyStructure<K> {
private:

    class MTreePromotion {
    private:
        K * firstKey;
        double firstCoveringRadius;
        long fistSubPageId;
        K * secondKey;
        double secondCoveringRadius;
        long secondSubPageId;

    public:

        MTreePromotion(K * firstKey, double firstCoveringRadius, long fistSubPageId,
                K * secondKey, double secondCoveringRadius, long secondSubPageId) {
            this->firstKey = firstKey;
            this->firstCoveringRadius = firstCoveringRadius;
            this->fistSubPageId = fistSubPageId;
            this->secondKey = secondKey;
            this->secondCoveringRadius = secondCoveringRadius;
            this->secondSubPageId = secondSubPageId;
        }

        ~MTreePromotion(){
            delete firstKey;
            delete secondKey;
        }
        
        double getFirstCoveringRadius() {
            return firstCoveringRadius;
        }

        K * getFirstKey() {
            return firstKey;
        }

        long getFistSubPageId() {
            return fistSubPageId;
        }

        double getSecondCoveringRadius() {
            return secondCoveringRadius;
        }

        K * getSecondKey() {
            return secondKey;
        }

        long getSecondSubPageId() {
            return secondSubPageId;
        }

        void setFirstCoveringRadius(double firstCoveringRadius) {
            this->firstCoveringRadius = firstCoveringRadius;
        }

        void setSecondCoveringRadius(double secondCoveringRadius) {
            this->secondCoveringRadius = secondCoveringRadius;
        }
    };

    class FindQualify {
    private:
        long pageId;
        double distanceParentToPointQuery;

    public:

        FindQualify(long pageId, double distanceParentToPointQuery) {
            this->pageId = pageId;
            this->distanceParentToPointQuery = distanceParentToPointQuery;
        }

        double getDistanceParentToPointQuery() {
            return distanceParentToPointQuery;
        }

        long getPageId() {
            return pageId;
        }

        const bool operator<(const FindQualify & obj) const {
            return (this->getDistanceParentToPointQuery() < obj->getDistanceParentToPointQuery());
        }
    };


    PerformanceMeasurement * averageForAdd;
    PerformanceMeasurement * averageForFind;

    double precisionError;

    MTreePromotion * splitIndex(Session * se, MTreeIndex<K> * fullIndex, 
            K * firstKey, Long firstSubId, Double firstCov, 
            K * secondKey, Long secondSubId, Double secondCov);

    MTreePromotion * splitLeaf(Session * se, MTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid);


public:

    MTree(Workspace * workspace);

    virtual ~MTree();

    Bool add(K * key);

    Uuid * find(K * key);

    PerformanceMeasurement * getAverageForAdd() {
        return averageForAdd;
    }//getAverageForAdd

    PerformanceMeasurement * getAverageForFind() {
        return averageForFind;
    }//getAverageForFind

    Long getRootPageId();

    Bool remove(K * key);

    void bfs();

    Int height();

};

//----------------------------------------------------------------------

template <typename K>
MTree<K>::MTree(Workspace * workspace) : AbstractKeyStructure<K>(workspace) {
    static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
    static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    averageForAdd = new AveragePerformance();
    averageForFind = new AveragePerformance();
    precisionError = 0.00000001;
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
    //clean home
    delete descriptor;
    se->close();
    delete se;
}//MTree

//----------------------------------------------------------------------

template <typename K>
MTree<K>::~MTree() {
    delete averageForAdd;
    delete averageForFind;
}//~MTree

//----------------------------------------------------------------------

template <typename K>
Bool MTree<K>::add(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    long diskAccess = se->getBlockAccess();
    long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();

    if (rootPageId == 0) // tree is empty
    {
        MTreeLeaf<K> * leaf = new MTreeLeaf<K>(se->create());
        leaf->addKey(key, DBL_MAX, key->getUuid());
        // Circularly link
        leaf->writePreviousPageId(leaf->getPageId());
        leaf->writeNextPageId(leaf->getPageId());
        descriptor->writeRootPageId(leaf->getPageId());
        descriptor->incTreeHeight();
        averageForAdd->incrementVerification(leaf->getCalculatedDistance());
        //clean home
        delete leaf;
    } else {
        int pathLength = descriptor->readTreeHeight();
        long * path = new long[pathLength];
        Node * node = se->load(rootPageId);
        double dist = DBL_MAX; // Distance to parent
        
        int level = 1;
        int qualify;
        long subPageId;
        path[0] = 0;
        
        while (MTreeIndex<K>::matchNodeType(node)) {
            MTreeIndex<K> * index = new MTreeIndex<K>(node);
            path[level] = index->getPageId();
            level++;
            qualify = index->indexOfInsertion(key);
            subPageId = index->readSubPageId(qualify);
            dist = index->distanceToQualify(qualify, key);
            node = se->load(subPageId);
            averageForAdd->incrementVerification(index->getCalculatedDistance());
            //clean home
            delete index;
        }

        MTreeLeaf<K> * leaf = new MTreeLeaf<K>(node);

        if (leaf->addKey(key, dist, key->getUuid()) == false) {
            int idxSub, idxParent;
            long pageIdSub;
            long pageIdActual;
            double distPromoParent1, distPromoParent2;
            double coveringSub = 0;
            double cov1, cov2;
            bool promote = true;
            MTreeIndex<K> * actualIndex, * parentIndex;
            K * parentKey;
            //split leaf
            MTreePromotion * objPromote = splitLeaf(se, leaf, key, key->getUuid());
            MTreePromotion * objIndexPromote;
            
            level = pathLength - 1;
            long parent = path[level];

            if (parent != 0) {
                //pageId of the sub node
                pageIdSub = leaf->getPageId();
                actualIndex = new MTreeIndex<K>(se->load(parent));

                while (level > 0) {
                    if (path[level - 1] != 0) {
                        //pageId of actual node
                        pageIdActual = actualIndex->getPageId();
                        parentIndex = new MTreeIndex<K>(se->load(path[level - 1]));

                        //calc dist
                        idxParent = parentIndex->indexOfSubPageId(pageIdActual);
                        parentKey = parentIndex->buildKey(idxParent);
                        distPromoParent1 = objPromote->getFirstKey()->distanceTo(parentKey);
                        distPromoParent2 = objPromote->getSecondKey()->distanceTo(parentKey);
                        
                        averageForAdd->incrementVerification(parentIndex->getCalculatedDistance()+2);
                        //clean home
                        delete parentKey;
                    } else {
                        parentIndex = NULL;
                        distPromoParent1 = DBL_MAX;
                        distPromoParent2 = DBL_MAX;
                    }

                    //search the index 
                    idxSub = actualIndex->indexOfSubPageId(pageIdSub);
                    if (promote == true) {
                        //remove old promoted key
                        actualIndex->remove(idxSub);

                        if (actualIndex->addKey(objPromote->getFirstKey(), distPromoParent1, objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius(),
                                objPromote->getSecondKey(), distPromoParent2, objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius())) {
                            promote = false;
                        } else {
                            //split index                            
                            objIndexPromote = splitIndex(se, actualIndex, 
                                    objPromote->getFirstKey(), objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius(),
                                    objPromote->getSecondKey(), objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius());
                            promote = true;
                            //clean home
                            delete objPromote;
                            //update objPromote
                            objPromote = objIndexPromote;
                        }//endif

                        cov1 = distPromoParent1 + objPromote->getFirstCoveringRadius();
                        cov2 = distPromoParent2 + objPromote->getSecondCoveringRadius();
                        if (cov1 > cov2) {
                            coveringSub = cov1;
                        } else {
                            coveringSub = cov2;
                        }
                    } else {
                        if (actualIndex->readCoveringRadius(idxSub) < coveringSub) {
                            actualIndex->writeCoveringRadius(idxSub, coveringSub);
                        }
                        coveringSub = actualIndex->readDistanceToParent(idxSub)
                                + actualIndex->readCoveringRadius(idxSub);
                    }
                    pageIdSub = actualIndex->getPageId();
                    
                    averageForAdd->incrementVerification(actualIndex->getCalculatedDistance());
                    //clean home
                    delete actualIndex;                
                    //update actual
                    actualIndex = parentIndex;
                    level--;
                }
                //clean home 
                delete actualIndex;
            }                
            if (promote == true) {
                //promote
                MTreeIndex<K> * newRoot = new MTreeIndex<K>(se->create());
                newRoot->addKey(objPromote->getFirstKey(), DBL_MAX, objPromote->getFistSubPageId(), objPromote->getFirstCoveringRadius());
                newRoot->addKey(objPromote->getSecondKey(), DBL_MAX, objPromote->getSecondSubPageId(), objPromote->getSecondCoveringRadius());
                // Circularly link
                newRoot->writePreviousPageId(newRoot->getPageId());
                newRoot->writeNextPageId(newRoot->getPageId());
                //ajust rootPageId										
                descriptor->writeRootPageId(newRoot->getPageId());
                descriptor->incTreeHeight();
                
                averageForAdd->incrementVerification(newRoot->getCalculatedDistance());
                delete newRoot;
            }
            //clean home
            delete objPromote;
        }
        averageForAdd->incrementVerification(leaf->getCalculatedDistance());
        //clean home
        delete [] path;
        delete leaf;
    }

    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForAdd->incrementDiskAccess(diskAccess);
    //clean home
    delete descriptor;
    se->close();
    delete se;

    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    averageForAdd->incrementTime(time_span.count()*1000000);
    averageForAdd->incrementMeasurement();
    return true;
}//add

//----------------------------------------------------------------------
template <typename K>
Uuid * MTree<K>::find(K * key) {   
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    long diskAccess = se->getBlockAccess();
    long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
    int total, idxOverlap;
    long pageId;
    double distParentToQuery, dist, cov;
    FindQualify * objQualify;
    K * storedKey;
    Uuid * uuidFinded = NULL;

    priority_queue<FindQualify*> qualifies;
    qualifies.push(new FindQualify(descriptor->readRootPageId(), DBL_MAX));
    do {
        objQualify = qualifies.top();
        qualifies.pop();

        pageId = objQualify->getPageId();
        distParentToQuery = objQualify->getDistanceParentToPointQuery();

        Node * node = se->load(objQualify->getPageId());

        if (MTreeIndex<K>::matchNodeType(node)) {
            MTreeIndex<K> * index = new MTreeIndex<K>(node);
            total = index->readNumberOfKeys();
            idxOverlap = 0;

            for (int i = 0; i < total; i++) {
                cov = index->readCoveringRadius(i);

                //triangle inequality 
                if (distParentToQuery - index->readDistanceToParent(i) <= cov) {
                    storedKey = index->buildKey(i); // Key stored in the node->

                    averageForFind->incrementVerification(1);
                    dist = storedKey->distanceTo(key); // Distance from key to storedKey: d(Or, On)

                    if (dist <= cov) {
                        qualifies.push(new FindQualify(index->readSubPageId(i), dist));
                        idxOverlap++;
                    }
                    //clean home
                    delete storedKey;
                }
            }
            
            averageForFind->incrementVerification(index->getCalculatedDistance());
            //clean home
            delete index;
        } else {
            MTreeLeaf<K> * leaf = new MTreeLeaf<K>(node);

            total = leaf->readNumberOfKeys();

            for (int i = 0; i < total; i++) {
                //triangle inequality 
                if (leaf->readDistanceToParent(i) == distParentToQuery) {
                    storedKey = leaf->buildKey(i);
                    averageForFind->incrementVerification(2);
                    dist = key->distanceTo(storedKey);
                    if ((dist == 0) && (storedKey->distanceTo(key) == 0)) {
                        uuidFinded = leaf->readEntityUuid(i);
                        //clean home
                        delete storedKey;
                        break;
                    }
                    //clean home
                    delete storedKey;
                }

            }

            averageForFind->incrementVerification(leaf->getCalculatedDistance());
            //clean home
            delete leaf;
        }
        //clean home
        delete objQualify;
        //free each block after use
        se->close();
    } while ((!qualifies.empty()) &&(uuidFinded == NULL));    
    while(!qualifies.empty()){
        objQualify = qualifies.top();
        qualifies.pop();
        delete objQualify;
    }
    
    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForFind->incrementDiskAccess(diskAccess);
    //clean home
    delete descriptor;
    //se->close();
    delete se;
    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    averageForFind->incrementTime(time_span.count()*1000000);
    averageForFind->incrementMeasurement();

    return uuidFinded;

}//find

//----------------------------------------------------------------------

template <typename K>
Long MTree<K>::getRootPageId() {
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return rootPageId;
}//getRootPageId

//----------------------------------------------------------------------

template <typename K>
Bool MTree<K>::remove(K * key) {
    return false;
}//remove

//----------------------------------------------------------------------

template <typename K>
typename MTree<K>::MTreePromotion * MTree<K>::splitIndex(Session * se,
        MTreeIndex<K> * fullIndex, 
        K * firstKey, Long firstSubId, Double firstCov,
        K * secondKey, Long secondSubId, Double secondCov) {
    int i, k1, k2;
    long nextPageId;
    double dist1, dist2;
    bool failAddKey;
    int total = fullIndex->readNumberOfKeys() + 2;
    MTreePromotion * objPromote = NULL;
    MTreeIndex<K> * newIndex = new MTreeIndex<K>(se->create());
    MTreeIndex<K> * nextIndex;
    K * objRep1;
    K * objRep2;
    Byte * serialRep1;
    Byte * serialRep2;

    //copy orded of keys and subPageId
    K ** vectKey = new K*[total];
    long * vectSub = new long[total + 1];
    double * vectCoverage = new double[total];
    //coping key and subPageId
    for (i = 0; i < total - 2; i++) {
        vectKey[i] = fullIndex->buildKey(i);
        vectSub[i] = fullIndex->readSubPageId(i);
        vectCoverage[i] = fullIndex->readCoveringRadius(i);
    }//endfor
    //copy key, subPageId and coverage
    vectKey[i] = firstKey;
    vectSub[i] = firstSubId;
    vectCoverage[i] = firstCov;
    vectKey[i + 1] = secondKey;
    vectSub[i + 1] = secondSubId;
    vectCoverage[i + 1] = secondCov;

    do {
        k1 = rand() % total;
        do {
            k2 = rand() % total;
        } while (k1 == k2);
        //zering
        fullIndex->clear();
        newIndex->clear();
        //choose promotion
        failAddKey = false;

        //copy representative 1
        objRep1 = new K();
        serialRep1 = new Byte[vectKey[k1]->sizeOfKey()];
        vectKey[k1]->pushKey(serialRep1, 0);
        objRep1->pullKey(serialRep1, 0);
        //copy representative 2
        objRep2 = new K();
        serialRep2 = new Byte[vectKey[k2]->sizeOfKey()];
        vectKey[k2]->pushKey(serialRep2, 0);
        objRep2->pullKey(serialRep2, 0);

        objPromote = new MTreePromotion(objRep1, 0, fullIndex->getPageId(), objRep2, 0, newIndex->getPageId());

        //distributing keys
        for (i = 0; i < total; i++) {
            averageForAdd->incrementVerification(2);
            dist1 = objPromote->getFirstKey()->distanceTo(vectKey[i]);
            dist2 = objPromote->getSecondKey()->distanceTo(vectKey[i]);
            if (dist1 < dist2) {
                if (fullIndex->addKey(vectKey[i], dist1, vectSub[i], vectCoverage[i])) {
                    //adjusting covering radius of the promoted Key1
                    if (objPromote->getFirstCoveringRadius() < (dist1 + vectCoverage[i])) {
                        objPromote->setFirstCoveringRadius(dist1 + vectCoverage[i]);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            } else {
                if (newIndex->addKey(vectKey[i], dist2, vectSub[i], vectCoverage[i])) {
                    //adjusting covering radius of the promoted Key2
                    if (objPromote->getSecondCoveringRadius() < (dist2 + vectCoverage[i])) {
                        objPromote->setSecondCoveringRadius(dist2 + vectCoverage[i]);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            }
        }//endfor
        //clean home
        delete [] serialRep1;
        delete [] serialRep2;
        if(failAddKey){
            //deleting objPromote(objRep1, objRep2)
            delete objPromote;
        }
    } while (failAddKey);

    // Circularly linked list
    newIndex->writeNextPageId(fullIndex->readNextPageId());
    fullIndex->writeNextPageId(newIndex->getPageId());
    newIndex->writePreviousPageId(fullIndex->getPageId());
    nextPageId = newIndex->readNextPageId();
    if (nextPageId != fullIndex->getPageId()) {
        nextIndex = new MTreeIndex<K>(se->load(nextPageId));
        nextIndex->writePreviousPageId(newIndex->getPageId());
        averageForAdd->incrementVerification(nextIndex->getCalculatedDistance());
        delete nextIndex;
    } else {
        fullIndex->writePreviousPageId(newIndex->getPageId());
    }
    //ajusting Covering Radius
    objPromote->setFirstCoveringRadius(objPromote->getFirstCoveringRadius() + precisionError);
    objPromote->setSecondCoveringRadius(objPromote->getSecondCoveringRadius() + precisionError);    
    //clean home
    for (i = 0; i < total - 2; i++) {
        delete vectKey[i];
    }//endfor
    delete [] vectKey;
    delete [] vectSub;
    delete [] vectCoverage;
    delete newIndex;
    return objPromote;
}//splitIndex

//----------------------------------------------------------------------

template <typename K>
typename MTree<K>::MTreePromotion * MTree<K>::splitLeaf(Session * se,
        MTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid) {

    int i, k1, k2;
    double dist1, dist2;
    long nextPageId;
    bool failAddKey;
    int total = fullLeaf->readNumberOfKeys() + 1;
    MTreePromotion * objPromote = NULL;
    MTreeLeaf<K> * nextLeaf;
    MTreeLeaf<K> * newLeaf = new MTreeLeaf<K>(se->create());
    K * objRep1;
    K * objRep2;
    Byte * serialRep1;
    Byte * serialRep2;
    long verifications = 0;

    //copy all keys and subPageId
    Uuid ** vectUuid = new Uuid*[total];
    K ** vectKey = new K*[total];
    //coping key and subPageId
    for (i = 0; i < total - 1; i++) {
        vectKey[i] = fullLeaf->buildKey(i);
        vectUuid[i] = fullLeaf->readEntityUuid(i);
    }//endfor
    //copy key and pageId in the correct position
    vectKey[i] = key;
    //copy the pageId in the correct position
    vectUuid[i] = uuid;

    do {
        k1 = rand() % total;
        do {
            k2 = rand() % total;
        } while (k1 == k2);
        //zering
        fullLeaf->clear();
        newLeaf->clear();
        //choose promotion
        failAddKey = false;

        //copy representative 1
        objRep1 = new K();
        serialRep1 = new Byte[vectKey[k1]->sizeOfKey()];
        vectKey[k1]->pushKey(serialRep1, 0);
        objRep1->pullKey(serialRep1, 0);
        //copy representative 2
        objRep2 = new K();
        serialRep2 = new Byte[vectKey[k2]->sizeOfKey()];
        vectKey[k2]->pushKey(serialRep2, 0);
        objRep2->pullKey(serialRep2, 0);
        
        objPromote = new MTreePromotion(objRep1, 0, fullLeaf->getPageId(), objRep2, 0, newLeaf->getPageId());

        //distributing keys
        for (i = 0; i < total; i++) {
            averageForAdd->incrementVerification(2);
            dist1 = objPromote->getFirstKey()->distanceTo(vectKey[i]);
            dist2 = objPromote->getSecondKey()->distanceTo(vectKey[i]);
            // increment verifications
            verifications += 2;
            
            if (dist1 < dist2) {
                if (fullLeaf->addKey(vectKey[i], dist1, vectUuid[i])) {
                    if (objPromote->getFirstCoveringRadius() < dist1) {
                        objPromote->setFirstCoveringRadius(dist1);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            } else {
                if (newLeaf->addKey(vectKey[i], dist2, vectUuid[i])) {
                    if (objPromote->getSecondCoveringRadius() < dist2) {
                        objPromote->setSecondCoveringRadius(dist2);
                    }
                } else {
                    failAddKey = true;
                    break;
                }
            }
        }//endfor
        //clean home
        delete [] serialRep1;
        delete [] serialRep2;
        if(failAddKey){
            //deleting objPromote(objRep1, objRep2)
            delete objPromote;
        }
    } while (failAddKey);

    // Circularly link
    newLeaf->writeNextPageId(fullLeaf->readNextPageId());
    fullLeaf->writeNextPageId(newLeaf->getPageId());
    newLeaf->writePreviousPageId(fullLeaf->getPageId());
    nextPageId = newLeaf->readNextPageId();
    if (nextPageId != fullLeaf->getPageId()) {
        nextLeaf = new MTreeLeaf<K>(se->load(nextPageId));
        nextLeaf->writePreviousPageId(newLeaf->getPageId());
        averageForAdd->incrementVerification(nextLeaf->getCalculatedDistance());
        delete nextLeaf;
    } else {
        fullLeaf->writePreviousPageId(newLeaf->getPageId());
    }
    //ajusting Covering Radius
    objPromote->setFirstCoveringRadius(objPromote->getFirstCoveringRadius() + precisionError);
    objPromote->setSecondCoveringRadius(objPromote->getSecondCoveringRadius() + precisionError);

    averageForAdd->incrementVerification(newLeaf->getCalculatedDistance());

    //clean home 
    for (i = 0; i < total - 1; i++) {
        delete vectKey[i];
        delete vectUuid[i];
    }//endfor
    delete [] vectKey;
    delete [] vectUuid;
    delete newLeaf;
    return objPromote;
}
//----------------------------------------------------------------------

template <typename K>
int MTree<K>::height() {   
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(MTreeDescriptor::nodeType);
    MTreeDescriptor * descriptor = new MTreeDescriptor(se->load(pageIdDescriptor));
    int total = 0;
    long rootPageId = descriptor->readRootPageId();

    Node * node = NULL;
    MTreeIndex<K> * index = NULL;
    long sub;
    
    if (rootPageId != 0) {
        node = se->load(rootPageId);
        while (MTreeIndex<K>::matchNodeType(node)) {
            index = new MTreeIndex<K>(node);
            sub = index->readSubPageId(0);
            node = se->load(sub);
            total++;
            //clean home
            delete index;
        }//endwhile
        total++;
    }//endif
    
    // clean home
    delete descriptor;
    se->close();
    delete se;
    
    return total;
}

} /* storage */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_STORAGE_MTREE_H */

