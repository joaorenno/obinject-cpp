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

#ifndef ORG_OBINJECT_STORAGE_RTREE_H
#define	ORG_OBINJECT_STORAGE_RTREE_H

#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <stack>
#include <list>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/device/Workspace.h>
#include <org/obinject/device/Session.h>
#include <org/obinject/queries/AveragePerformance.h>
#include <org/obinject/block/RTreeDescriptor.h>
#include <org/obinject/block/RTreeLeaf.h>
#include <org/obinject/block/RTreeIndex.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Key.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/storage/EuclideanGeometry.h>

using namespace std;
using namespace std::chrono;

namespace org {
namespace obinject {
namespace storage {

template <typename K>
class RTree : public AbstractKeyStructure<K> {
private:
    bool test;           //controla a saida do COUT (joaorenno)
    //*!duvida Nao precisa de this->getKeyClass() nem do new?
    EuclideanGeometry<K> * geometry;

    PerformanceMeasurement * averageForAdd;

    PerformanceMeasurement * averageForFind;

    class RTreePromotion {
    private:
        K * firstKey;
        Long firstSubPageId;
        K * secondKey;
        Long secondSubPageId;

    public:

        RTreePromotion(K * firstKey, Long fistSubPageId, K * secondKey, Long secondSubPageId) {
            this->firstKey = firstKey;
            this->firstSubPageId = fistSubPageId;
            this->secondKey = secondKey;
            this->secondSubPageId = secondSubPageId;
        }

        ~RTreePromotion(){
//            delete firstKey;
//            delete secondKey;
        }

        K * getFirstKey() {
            return firstKey;
        }

        Long getFirstSubPageId() {
            return firstSubPageId;
        }

        K * getSecondKey() {
            return secondKey;
        }

        Long getSecondSubPageId() {
            return secondSubPageId;
        }

        Long setFirstKey(K * firstKey){
            this->firstKey = firstKey;
        }

        Long setSecondKey(K * secondKey){
            this->secondKey = secondKey;
        }

    };

    RTreePromotion * splitIndex(Session * se, RTreeIndex<K> * fullIndex,
            K * key, Long subId);


    RTreePromotion * splitLeaf(Session * se, RTreeLeaf<K> * fullLeaf,
            K * key, Uuid * uuid);


public:

    RTree(Workspace * workspace);

    virtual ~RTree();

    Bool add(K * key);

    Uuid * find(K * key);

    EuclideanGeometry<K> * getGeometry(){
        return geometry;
    }//getGeometry

    PerformanceMeasurement * getAverageForAdd() {
        return averageForAdd;
    }//getAverageForAdd

    PerformanceMeasurement * getAverageForFind() {
        return averageForFind;
    }//getAverageForFind
    
    bool getTest() {
        return test;
    }
    
    void setTest(int test) {
        this->test = test;
    }

    Long getRootPageId();

    Bool remove(K * key);

    void showAll();

    void bfs();
};

//------------------------------------------------------------------------------

//*!Duvida quanto a variavel dscriptor
template <typename K>
RTree<K>::RTree(Workspace * workspace) : AbstractKeyStructure<K>(workspace) {
    static_assert(is_base_of<Rectangle<K>, K>::value, "Generic type is not derived of the Order class");
    static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    averageForAdd = new AveragePerformance();
    averageForFind = new AveragePerformance();
    //?????????????????????????????????????
    geometry = new EuclideanGeometry<K>();
    Session * se = this->getWorkspace()->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor* descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    delete descriptor;
    se->close();
    delete se;
}

//------------------------------------------------------------------------------

template <typename K>
RTree<K>::~RTree() {
    delete averageForAdd;
    delete averageForFind;
    delete geometry;
}//~RTree

//------------------------------------------------------------------------------

template <typename K>
Bool RTree<K>::add(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    Long diskAccess = se->getBlockAccess();
    Long verifications = this->geometry->getCalculatedOverlap();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Long rootPageId = descriptor->readRootPageId();
//cout << "rootPageId - " << rootPageId << endl;

    if (rootPageId == 0) // tree is empty
    {
        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(se->create());
        leaf->addKey(key, key->getUuid());
        // Circularly link
        leaf->writePreviousPageId(leaf->getPageId());
        leaf->writeNextPageId(leaf->getPageId());
        descriptor->writeRootPageId(leaf->getPageId());
        descriptor->incTreeHeight();
        //clean home
        delete leaf;
    } else {
        Int pathLength = descriptor->readTreeHeight();
//cout << "pathLength - " << pathLength << endl;
        Long * path = new Long[pathLength];
        Node * node = se->load(rootPageId);

        Int level = 1;
        path[0] = 0;

        while(RTreeIndex<K>::matchNodeType(node)){
            RTreeIndex<K> * index = new RTreeIndex<K>(node);
            index->setTest(getTest());
            path[level] = index->getPageId();
            level++;
            Int qualify = index->indexOfInsertion(key, geometry);
//            if(test) cout << "qualify - " << qualify << endl << endl;
            Long subPageId = index->readSubPageId(qualify);
            node = se->load(subPageId);
            //clean home
            delete index;
        }

        RTreeLeaf<K> * leaf = new RTreeLeaf<K>(node);
        
        if (leaf->addKey(key, key->getUuid()) == false) {
            bool promote = true;            
            Long pageIdRem = leaf->getPageId();

            RTreePromotion * objPromote = this->splitLeaf(se, leaf, key, key->getUuid());
            RTreePromotion * objIndexPromote;
            level = pathLength - 1;

            while (level>0){
                node = se->load(path[level]);

                Int idxReplc;

                if (promote == true){
                    RTreeIndex<K> * index = new RTreeIndex<K>(node);
                    index->setTest(getTest());
                    idxReplc = index->indexOfSubPageId(pageIdRem);
                    index->replace(idxReplc, objPromote->getFirstKey());

                    if (index->addKey(objPromote->getSecondKey(), objPromote->getSecondSubPageId())) {
                        promote = false;
                    } else {
                        objIndexPromote = this->splitIndex(se, index, objPromote->getSecondKey(), objPromote->getSecondSubPageId());

                        pageIdRem=node->getPageId();
                        promote = true;
                        //clean home
                        delete objPromote;
                        //update objPromote
                        objPromote = objIndexPromote;
                    }
                    //clean home
                    delete index;
                }

                level--;
            }

            if (promote == true){
//                cout << ">>>>> promote" << endl;
                RTreeIndex<K> * newIndex = new RTreeIndex<K>(se->create());
                newIndex->setTest(getTest());
                newIndex->addKey(objPromote->getFirstKey(), objPromote->getFirstSubPageId());
                newIndex->addKey(objPromote->getSecondKey(), objPromote->getSecondSubPageId());

                newIndex->writePreviousPageId(newIndex->getPageId());
                newIndex->writeNextPageId(newIndex->getPageId());

                descriptor->writeRootPageId(newIndex->getPageId());
                descriptor->incTreeHeight();
                delete newIndex;
            }
            //clean home
            delete objPromote;
        }
        //clean home
        delete [] path;
        delete leaf;
    }

    diskAccess = se->getBlockAccess() - diskAccess;
    averageForAdd->incrementDiskAccess(diskAccess);
    verifications = this->geometry->getCalculatedOverlap() - verifications;
    averageForAdd->incrementVerification(verifications);

    delete descriptor;
    se->close();
    delete se;

    duration<Double> time_span = duration_cast<duration<Double>>(steady_clock::now() - t1);
    averageForAdd->incrementTime(time_span.count()*1000000);
    averageForAdd->incrementMeasurement();

    return true;

}//add

//------------------------------------------------------------------------------


template <typename K>
Uuid * RTree<K>::find(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    Long diskAccess = se->getBlockAccess();
    Long verifications = this->geometry->getCalculatedOverlap();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Int total, overlap;
    Long pageId;
    K * storedKey;
    Uuid * uuidFinded = NULL;

    //Método Find foi mudado de stack qualifies para List Qualifies
    list<Long> qualifies;
    qualifies.push_back(descriptor->readRootPageId());
    do{
        pageId = qualifies.front();
        qualifies.pop_front();
        Node * node = se->load(pageId);

        if (RTreeIndex<K>::matchNodeType(node)) {
            RTreeIndex<K> *index = new RTreeIndex<K>(node);
            index->setTest(getTest());
            total = index->readNumberOfKeys();
            overlap = 0;

            for (Int i=0; i<total; i++){
                storedKey = index->buildKey(i);

                if (geometry->isOverlap(storedKey, key)){
                    qualifies.push_back(index->readSubPageId(i));
                    overlap++;
                }
                delete storedKey;
            }

            delete index;

        } else {
            RTreeLeaf<K> *leaf = new RTreeLeaf<K>(node);
            Int idx = leaf->indexOfKey(key);
//            cout << idx << endl;
            if (idx!= -1){
                uuidFinded = leaf->readEntityUuid(idx);
                delete leaf;
                break;
            }
            delete leaf;
        }
    } while (!qualifies.empty());

    se->close();

    diskAccess = se->getBlockAccess() - diskAccess;
    averageForFind->incrementDiskAccess(diskAccess);
    verifications = this->geometry->getCalculatedOverlap() - verifications;
    averageForFind->incrementVerification(verifications);

    //clean home
    delete se;
    delete descriptor;

    duration<Double> time_span = duration_cast<duration<Double>>(steady_clock::now() - t1);
    averageForFind->incrementTime(time_span.count()*1000000);
    averageForFind->incrementMeasurement();

    return uuidFinded;
}

//------------------------------------------------------------------------------

template <typename K>
Long RTree<K>::getRootPageId(){
    Session * se = this->getWorkspace()->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));
    Long rootPageId = descriptor->readRootPageId();

    //clean home
    delete descriptor;
    se->close();
    delete se;

    return rootPageId;
}

//------------------------------------------------------------------------------

template <typename K>
Bool RTree<K>::remove(K * key) {
    return false;
}//remove

//------------------------------------------------------------------------------

template <typename K>
typename RTree<K>::RTreePromotion * RTree<K>::splitIndex(Session * se,
        RTreeIndex<K> * fullIndex, K * key, Long subId){
//    cout << ">>>>> slipt index" << endl;
    Int i, j;
    Int total = fullIndex->readNumberOfKeys() +1;
    RTreeIndex<K> * newIndex = new RTreeIndex<K>(se->create());
    newIndex->setTest(getTest());

    //Copying keys and subs
    K ** vectKey = new K*[total];
    Long * vectSub = new Long[total];
    for(i=0; i<total-1; i++){
        vectKey[i] = fullIndex->buildKey(i);
        vectSub[i] = fullIndex->readSubPageId(i);
    }
    vectKey[i] = key;
    vectSub[i] = subId;

    //Choose promote
    Double ocup = -1.0;
    RTreePromotion * objPromote;

    Double d;
    Int idx1 = 0, idx2 = 1;

    for(i=0; i<total-1; i++){
        for(j=i+1; j<total; j++){
            K* mbrUnion = this->geometry->rectUnion(vectKey[i], vectKey[j]);
            //Calc Dead space: union(mbr1, mbr2) - (mbr1 + mbr2 - intersec(mbr1, mbr2))
            d= this->geometry->occupancy(mbrUnion)
                    - (this->geometry->occupancy(vectKey[i])
                    + this->geometry->occupancy(vectKey[j]));

            if(d > ocup){
                ocup = d;
                idx1 = i;
                idx2 = j;
            }
            //clean home
            delete mbrUnion;
        }
    }
    //Cleanning and Inserting
    fullIndex->clear();
    fullIndex->addKey(vectKey[idx1], vectSub[idx1]);
    //Cleanning and inserting idx2
    newIndex->clear();
    newIndex->addKey(vectKey[idx2], vectSub[idx2]);
    //Promoted MBR
    objPromote = new RTreePromotion(vectKey[idx1], fullIndex->getPageId(), vectKey[idx2],
            newIndex->getPageId());
    //Distributing keys
    for (i=0; i<total; i++){
        if(!(i == idx1 || i == idx2)){
            K* union1 = this->geometry->rectUnion(objPromote->getFirstKey(), vectKey[i]);
            Double ocup1 = geometry->occupancy(union1) - this->geometry->occupancy(objPromote->getFirstKey());
            K* union2 = this->geometry->rectUnion(objPromote->getSecondKey(), vectKey[i]);
            Double ocup2 = geometry->occupancy(union2) - this->geometry->occupancy(objPromote->getSecondKey());

            if (ocup1 < ocup2){
                if (!fullIndex->addKey(vectKey[i], vectSub[i])){
                    throw std::invalid_argument( "add fail" );
                }
                //clean home
                delete objPromote->getFirstKey();
                
                objPromote->setFirstKey(union1);
                
                //clean home
                delete union2;
            }else if (ocup1 > ocup2){
                if (!newIndex->addKey(vectKey[i], vectSub[i])){
                    throw std::invalid_argument( "add fail" );
                }
                //clean home
                delete objPromote->getSecondKey();
                
                objPromote->setSecondKey(union2);
                
                //clean home
                delete union1;
            }else{
                Double ocupProm1 = this->geometry->occupancy(objPromote->getFirstKey());
                Double ocupProm2 = this->geometry->occupancy(objPromote->getSecondKey());
                if (ocupProm1 < ocupProm2) {
                    if (!fullIndex->addKey(vectKey[i], vectSub[i])){
                        throw std::invalid_argument( "add fail" );
                    }
                    //clean home
                    delete objPromote->getFirstKey();
                 
                    objPromote->setFirstKey(union1);
                
                    //clean home
                    delete union2;
                }else if (ocupProm1 > ocupProm2) {
                    if (!newIndex->addKey(vectKey[i], vectSub[i])) {
                        throw std::invalid_argument( "add fail" );
                    }
                    //clean home
                    delete objPromote->getSecondKey();
                
                    objPromote->setSecondKey(union2);
                
                    //clean home
                    delete union1;
                }else{
                    Double dist1 = objPromote->getFirstKey()->distanceTo(vectKey[i]);
                    Double dist2 = objPromote->getSecondKey()->distanceTo(vectKey[i]);

                    if (dist1 < dist2) {
                        if (!fullIndex->addKey(vectKey[i], vectSub[i])) {
                            throw std::invalid_argument( "add fail" );
                        }
                        //clean home
                        delete objPromote->getFirstKey();
                
                        objPromote->setFirstKey(union1);
                
                        //clean home
                        delete union2;
                    } else if (dist1 > dist2) {
                        if (!newIndex->addKey(vectKey[i], vectSub[i])) {
                            throw std::invalid_argument( "add fail" );
                        }
                        //clean home
                        delete objPromote->getSecondKey();
                
                        objPromote->setSecondKey(union2);
                
                        //clean home
                        delete union1;
                    } else {
                        if (fullIndex->readNumberOfKeys() <= newIndex->readNumberOfKeys()) {
                            if (!fullIndex->addKey(vectKey[i], vectSub[i])) {
                                throw std::invalid_argument( "add fail" );
                            }
                            //clean home
                            delete objPromote->getFirstKey();
                
                            objPromote->setFirstKey(union1);
                
                            //clean home
                            delete union2;
                        } else {
                            if (!newIndex->addKey(vectKey[i], vectSub[i])) {
                                throw std::invalid_argument( "add fail" );
                            }
                            //clean home
                            delete objPromote->getSecondKey();
                
                            objPromote->setSecondKey(union2);
                
                            //clean home
                            delete union1;
                        }
                    }
                }
            }
        }
    }

    //Circularly linked list
    newIndex->writeNextPageId(fullIndex->readNextPageId());
    fullIndex->writeNextPageId(newIndex->getPageId());
    newIndex->writePreviousPageId(fullIndex->getPageId());
    Long nextPageId = newIndex->readNextPageId();
    if (nextPageId != fullIndex->getPageId()){
        RTreeIndex<K> *nextIndex = new RTreeIndex<K>(se->load(nextPageId));
        nextIndex->setTest(getTest());
        nextIndex->writePreviousPageId(newIndex->getPageId());

    }else{
        fullIndex->writePreviousPageId(newIndex->getPageId());

    }
    //clean home
    for (i = 0; i < total-1; i++){
//        delete vectKey[i];
    }
    delete [] vectKey;
    delete [] vectSub;
    delete newIndex;
    return objPromote;
}

//------------------------------------------------------------------------------

template <typename K>
typename RTree<K>::RTreePromotion * RTree<K>::splitLeaf(Session * se,
        RTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid){
//    cout << ">>>>> slipt leaf" << endl;
    Int i, j;
    Int total = fullLeaf->readNumberOfKeys() +1;
    RTreeLeaf<K> * newLeaf = new RTreeLeaf<K>(se->create());
        
    //Copying keys and UUids
    K ** vecKey = new K*[total];
    Uuid ** vecUuid = new Uuid*[total];
    for (i = 0; i < total-1; i++){
        vecKey[i] = fullLeaf->buildKey(i);
        vecUuid[i] = fullLeaf->readEntityUuid(i);
    }
    vecKey[i] = key;
    vecUuid[i] = uuid;

    //Choose promote
    Double ocup = -1.0;

    RTreePromotion * objPromote;

    Double d;
    Int idx1 = 0, idx2 = 1;

    for(i=0; i<total-1; i++){ //Quadratic Split
        for(j=i+1; j<total; j++){
            K * mbrUnion = geometry->rectUnion(vecKey[i], vecKey[j]);
            //Calc Dead Space
            d = geometry->occupancy(mbrUnion) - (geometry->occupancy(vecKey[i]) + geometry->occupancy(vecKey[j]));

            if (d>ocup){
                ocup = d;
                idx1 = i;
                idx2 = j;
            }
            //clean home
            delete mbrUnion;
        }
    }

    //cleanning fullndex and inserting vecKey[idx1]
    fullLeaf->clear();
    fullLeaf->addKey(vecKey[idx1], vecUuid[idx1]);
    //cleanning newIndex and inserting vecKey[idx2]
    newLeaf->clear();
    newLeaf->addKey(vecKey[idx2], vecUuid[idx2]);
    //promote MBR
    objPromote = new RTreePromotion(vecKey[idx1], fullLeaf->getPageId(), vecKey[idx2], newLeaf->getPageId());
    //Distributing Keys
    for (i = 0; i < total; i++) {
        if (!(i == idx1 || i == idx2)) {
            K* union1 = this->geometry->rectUnion(objPromote->getFirstKey(), vecKey[i]);
            Double ocup1 = geometry->occupancy(union1) - geometry->occupancy(objPromote->getFirstKey());
            K* union2 = this->geometry->rectUnion(objPromote->getSecondKey(), vecKey[i]);
            Double ocup2 = geometry->occupancy(union2) - geometry->occupancy(objPromote->getSecondKey());
            
            if (ocup1 < ocup2) {
                if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
                    throw std::invalid_argument( "add fail" );
                }
                //clean home
//                delete objPromote->getFirstKey();
                
                objPromote->setFirstKey(union1);
                
                //clean home
                delete union2;                
            } else if (ocup1 > ocup2) {
                if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
                    throw std::invalid_argument( "add fail" );
                }
                //clean home
//                delete objPromote->getSecondKey();

                objPromote->setSecondKey(union2);
                
                //clean home
                delete union1;                
            } else {
                Double ocupProm1 = geometry->occupancy(objPromote->getFirstKey());
                Double ocupProm2 = geometry->occupancy(objPromote->getSecondKey());
                if (ocupProm1 < ocupProm2) {
                    if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
                        throw std::invalid_argument( "add fail" );
                    }
                    //clean home
//                    delete objPromote->getFirstKey();
                    
                    objPromote->setFirstKey(union1);
                    
                    //clean home
                    delete union2;  
                } else if (ocupProm1 > ocupProm2) {
                    if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
                        throw std::invalid_argument( "add fail" );
                    }
                    //clean home
//                    delete objPromote->getSecondKey();
                    
                    objPromote->setSecondKey(union2);
                    
                    //clean home
                    delete union1;  
                } else {
                    Double dist1 = objPromote->getFirstKey()->distanceTo(vecKey[i]);
                    Double dist2 = objPromote->getSecondKey()->distanceTo(vecKey[i]);

                    if (dist1 < dist2) {
                        if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
                            throw std::invalid_argument( "add fail" );
                        }
                        //clean home
//                        delete objPromote->getFirstKey();
                        
                        objPromote->setFirstKey(union1);
                        
                        //clean home
                        delete union2;                          
                    } else if (dist1 > dist2) {
                        if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
                            throw std::invalid_argument( "add fail" );
                        }
                        //clean home
//                        delete objPromote->getSecondKey();
                        
                        objPromote->setSecondKey(union2);
                        
                        //clean home
                        delete union1;  
                    } else {
                        if (fullLeaf->readNumberOfKeys() <= newLeaf->readNumberOfKeys()) {
                            if (!fullLeaf->addKey(vecKey[i], vecUuid[i])) {
                                throw std::invalid_argument( "add fail" );
                            }
                            //clean home
//                            delete objPromote->getFirstKey();

                            objPromote->setFirstKey(union1);
                            
                            //clean home
                            delete union2;  
                        } else {
                            if (!newLeaf->addKey(vecKey[i], vecUuid[i])) {
                                throw std::invalid_argument( "add fail" );
                            }
                            //clean home
//                            delete objPromote->getSecondKey();

                            objPromote->setSecondKey(union2);
                             
                            //clean home
                            delete union1;  
                        }
                    }
                }
            }
        }
    }

    //Circularly Link
    newLeaf->writeNextPageId(fullLeaf->readNextPageId());
    fullLeaf->writeNextPageId(newLeaf->getPageId());
    newLeaf->writePreviousPageId(fullLeaf->getPageId());
    Long nextPageId = newLeaf->readNextPageId();
    if (nextPageId != fullLeaf->getPageId()) {
        RTreeLeaf<K> * nextLeaf = new RTreeLeaf<K>(se->load(nextPageId));
        nextLeaf->writePreviousPageId(newLeaf->getPageId());
        //clean home
        delete nextLeaf;
    } else {
        fullLeaf->writePreviousPageId(newLeaf->getPageId());
    }
    //clean home
    for (i = 0; i < total-1; i++){
        delete vecKey[i];
        delete vecUuid[i];
    }
    delete [] vecKey;
    delete [] vecUuid;
    delete newLeaf;
    
    return objPromote;
}

//------------------------------------------------------------------------------

template <typename K>
void RTree<K>::showAll(){
    Session * se = this->getWorkspace()->openSession();
    Long pageIdDescriptor = se->findPageIdDescriptor(RTreeDescriptor::nodeType);
    RTreeDescriptor * descriptor = new RTreeDescriptor(se->load(pageIdDescriptor));

    Node * node = se->load(descriptor->readRootPageId());

    while(RTreeIndex<K>::matchNodeType(node)){
        RTreeIndex<K>* index = new RTreeIndex<K>(node);
        index->setTest(getTest());
        Long subPageId = index->readSubPageId(0);
        node = se->load(subPageId);
        delete index;
    }

    Long firstPageId = node->getPageId();
    Long actualPageId = firstPageId;

    Int count = 1;
    do{
        RTreeLeaf<K>* leaf = new RTreeLeaf<K>(se->load(actualPageId));

        for (Int i=0; i<leaf->readNumberOfKeys(); i++){
            K* storedKey = leaf->buildKey(i);
            cout << count << " : " << storedKey->toString() << endl;
            count++;
            //clean home
            delete storedKey;
        }

        delete leaf;

        actualPageId = leaf->readNextPageId();

    }while (actualPageId != firstPageId);
    se->close();
    delete se;
    delete descriptor;
}

//------------------------------------------------------------------------------

} /*storage*/
} /*obinject*/
} /*org*/

#endif	/*ORG_OBINJECT_STORAGE_RTREE_H*/
