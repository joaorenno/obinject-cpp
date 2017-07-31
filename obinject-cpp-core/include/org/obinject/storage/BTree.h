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
#ifndef ORG_OBINJECT_STORAGE_BTREE_H
#define	ORG_OBINJECT_STORAGE_BTREE_H

#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/device/Workspace.h>
#include <org/obinject/device/Session.h>
#include <org/obinject/queries/AveragePerformance.h>
#include <org/obinject/block/BTreeDescriptor.h>
#include <org/obinject/block/BTreeLeaf.h>
#include <org/obinject/block/BTreeIndex.h>
#include <org/obinject/meta/all.h>

using namespace std;
using namespace std::chrono;

namespace org {
namespace obinject {
namespace storage {

template <typename K>
class BTree : public AbstractKeyStructure<K> {
private:

    class BTreePromotion {
    public:
        K * key;
        long subPageId;

        BTreePromotion(K * key, long subPageId) {
            static_assert(is_base_of<Order<K>, K>::value, "Generic type is not derived of the Order class");
            static_assert(is_base_of<Comparable<K>, K>::value, "Generic type is not derived of the Comparable class");
            static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
            this->key = key;
            this->subPageId = subPageId;
        }

        virtual ~BTreePromotion() {
            delete key;
        }
    };

    PerformanceMeasurement * averageForAdd;
    PerformanceMeasurement * averageForFind;

    BTreePromotion * splitIndex(BTreeIndex<K> * fullIndex, K * key, Long pageId, Session * se);

    BTreePromotion * splitLeaf(BTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid, Session * se);


public:

    BTree(Workspace * workspace);

    virtual ~BTree();

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
    
    int height();

};

//----------------------------------------------------------------------

template <typename K>
BTree<K>::BTree(Workspace * workspace) : AbstractKeyStructure<K>(workspace) {
    static_assert(is_base_of<Order<K>, K>::value, "Generic type is not derived of the Order class");
    static_assert(is_base_of<Comparable<K>, K>::value, "Generic type is not derived of the Comparable class");
    static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    averageForAdd = new AveragePerformance();
    averageForFind = new AveragePerformance();
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    //clean home
    delete descriptor;
    se->close();
    delete se;
}//BTree

//----------------------------------------------------------------------

template <typename K>
BTree<K>::~BTree() {
    delete averageForAdd;
    delete averageForFind;
}//~BTree

//----------------------------------------------------------------------

template <typename K>
Bool BTree<K>::add(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    long diskAccess = se->getBlockAccess();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();
    BTreeLeaf<K> * leaf;
    BTreeLeaf<K> * newLeaf;
    BTreeIndex<K> * index;
    BTreeIndex<K> * newIndex;
    BTreePromotion * objPromote = NULL;
    BTreePromotion * objIndexPromote = NULL;
    Node * node;
    bool promote;
    long * path;
    long parent;
    long sub;
    int level;
    int treeHeight;
    // If root does not exist
    if (rootPageId == 0) {
        // Add first leaf
        newLeaf = new BTreeLeaf<K>(se->create());
        newLeaf->addFirstKey(key, key->getUuid());
        // Circularly link
        newLeaf->writePreviousPageId(newLeaf->getPageId());
        newLeaf->writeNextPageId(newLeaf->getPageId());
        // Adjust rootPageId
        descriptor->writeRootPageId(newLeaf->getPageId());
        descriptor->incTreeHeight();
        averageForAdd->incrementVerification(newLeaf->getCalculatedComparisons());
        delete newLeaf;
    } else {
        treeHeight = descriptor->readTreeHeight();
        path = new long[treeHeight];
        node = se->load(rootPageId);
        level = 1;
        while (BTreeIndex<K>::matchNodeType(node)) {
            index = new BTreeIndex<K>(node);
            path[level] = index->getPageId();
            level++;
            sub = index->readSubPageId(index->indexOfQualify(key));
            node = se->load(sub);
            averageForAdd->incrementVerification(index->getCalculatedComparisons());
            delete index;
        }//endwhile

        leaf = new BTreeLeaf<K>(node);
        if (leaf->addKey(key, key->getUuid()) == false) {
            // Split
            promote = true;
            node = leaf;
            // Split leaf
            objPromote = splitLeaf(leaf, key, key->getUuid(), se);

            level = treeHeight - 1;

            while (level > 0) {
                parent = path[level];
                node = se->load(parent);

                if (promote == true) {
                    //add in index
                    index = new BTreeIndex<K>(node);
                    if (index->addKey(objPromote->key, objPromote->subPageId) == false) {
                        // Split index
                        objIndexPromote = this->splitIndex(index, objPromote->key, objPromote->subPageId, se);
                        //clean home
                        delete objPromote;
                        //update objPromote
                        objPromote = objIndexPromote;
                        promote = true;
                    } else {
                        promote = false;
                    }//endif
                    averageForAdd->incrementVerification(index->getCalculatedComparisons());
                    // clean home
                    delete index;
                }//endif
                level--;
            }//endwhile

            if (promote == true) {
                // Promote
                newIndex = new BTreeIndex<K>(se->create());
                newIndex->addFirstKey(objPromote->key, node->getPageId(), objPromote->subPageId);
                // Circularly link
                newIndex->writePreviousPageId(newIndex->getPageId());
                newIndex->writeNextPageId(newIndex->getPageId());
                descriptor->writeRootPageId(newIndex->getPageId());
                descriptor->incTreeHeight();
                averageForAdd->incrementVerification(newIndex->getCalculatedComparisons());
                //clean home
                delete newIndex;
            }//endif
            //clean home
            delete objPromote;
        }//endif
        averageForAdd->incrementVerification(leaf->getCalculatedComparisons());
        //clean home
        delete [] path;
        delete leaf;
    }//endif
    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForAdd->incrementDiskAccess(diskAccess);
    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    averageForAdd->incrementTime(time_span.count()*1000000);
    averageForAdd->incrementMeasurement();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return true;
}//add

//----------------------------------------------------------------------

template <typename K>
Uuid * BTree<K>::find(K * key) {
    steady_clock::time_point t1 = steady_clock::now();
    Session * se = this->getWorkspace()->openSession();
    long diskAccess = se->getBlockAccess();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();
    Uuid * uuidFinded = NULL;
    BTreeLeaf<K> * leaf;
    BTreeIndex<K> * index;
    Node * node;
    long sub;
    if (rootPageId != 0) {
        node = se->load(rootPageId);
        while (BTreeIndex<K>::matchNodeType(node)) {
            index = new BTreeIndex<K>(node);
            sub = index->readSubPageId(index->indexOfQualify(key));
            node = se->load(sub);
            averageForFind->incrementVerification(index->getCalculatedComparisons());
            //clean home
            delete index;
        }//endwhile
        leaf = new BTreeLeaf<K>(node);
        int idx = leaf->indexOfKey(key);
        if (idx >= 0) {
            uuidFinded = leaf->readEntityUuid(idx);
        }//endif
        averageForFind->incrementVerification(leaf->getCalculatedComparisons());
        // clean home
        delete leaf;
    }//endif
    //statistic for add
    diskAccess = se->getBlockAccess() - diskAccess;
    averageForFind->incrementDiskAccess(diskAccess);
    duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - t1);
    averageForFind->incrementTime(time_span.count()*1000000);
    averageForFind->incrementMeasurement();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return uuidFinded;
}//find

//----------------------------------------------------------------------

template <typename K>
Long BTree<K>::getRootPageId() {
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return rootPageId;
}//getRootPageId

//----------------------------------------------------------------------

template <typename K>
Bool BTree<K>::remove(K * key) {
    bool result = false;
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readRootPageId();
    BTreeIndex<K> * index;
    BTreeLeaf<K> * leaf;
    Node * node;
    long sub;
    Int idx;
    if (rootPageId != 0) {
        node = se->load(descriptor->readRootPageId());

        while (BTreeIndex<K>::matchNodeType(node)) {
            index = new BTreeIndex<K>(node);
            sub = index->readSubPageId(index->indexOfQualify(key));
            node = se->load(sub);
            //clean home
            delete index;
        }//endwhile
        leaf = new BTreeLeaf<K>(node);
        idx = leaf->indexOfKey(key);
        result = leaf->remove(idx);
        //clean home
        delete leaf;
    }//endif
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return result;
}//remove

//----------------------------------------------------------------------
template <typename K>
typename BTree<K>::BTreePromotion * BTree<K>::splitIndex(BTreeIndex<K> * fullIndex, K * key, Long pageId, Session * se) {
    int total = fullIndex->readNumberOfKeys() + 1;
    int half = ((int) (total / 2));
    K * objHalf = new K();
    BTreeIndex<K> * newIndex = new BTreeIndex<K>(se->create());
    BTreeIndex<K> * nextIndex;
    BTreePromotion * promote;
    long nextPageId;
    int posKey;
    int i;
    Byte * serialHalf;
    // Copy orded of keys and subPageId
    long * vectSub = new long[total + 1];
    K ** vectKey = new K*[total];
    // Copy key and subPageId
    for (i = 0; i < total - 1; i++) {
        vectKey[i] = fullIndex->buildKey(i);
        vectSub[i] = fullIndex->readSubPageId(i);
    }//endfor
    // Last subPageId
    vectSub[i] = fullIndex->readSubPageId(i);
    // Moving for find position of the key
    while (i > 0) { 
        averageForAdd->incrementVerification(1);
        if (key->compareTo(vectKey[i - 1]) < 0) {
            // Copy the key and pageId in the last position
            vectKey[i] = vectKey[i - 1];
            vectSub[i + 1] = vectSub[i];
            i--;
        }
        else {
            break;
        }
    }//endwhile
    // Copy key and pageId in the correct position
    posKey = i;
    vectKey[i] = key;
    // Copy the pageId in the correct position
    vectSub[i + 1] = pageId;
    // Copy lower half of keys and subPageId
    fullIndex->addFirstKey(vectKey[0], vectSub[0], vectSub[1]);
    for (i = 1; i < half; i++) {
        fullIndex->addKey(vectKey[i], vectSub[i + 1]);
    }//endfor
    // Copy upper half of keys and subPageId
    newIndex->addFirstKey(vectKey[half + 1], vectSub[half + 1], vectSub[half + 2]);
    for (i = half + 1; i < total; i++) {
        newIndex->addKey(vectKey[i], vectSub[i + 1]);
    }//endfor   
    //copy half object 
    serialHalf = new Byte[vectKey[half]->sizeOfKey()];
    vectKey[half]->pushKey(serialHalf, 0);
    objHalf->pullKey(serialHalf, 0);
    promote = new BTreePromotion(objHalf, newIndex->getPageId());
    // Circularly link
    newIndex->writeNextPageId(fullIndex->readNextPageId());
    fullIndex->writeNextPageId(newIndex->getPageId());
    newIndex->writePreviousPageId(fullIndex->getPageId());
    nextPageId = newIndex->readNextPageId();
    if (nextPageId != fullIndex->getPageId()) {
        nextIndex = new BTreeIndex<K>(se->load(nextPageId));
        nextIndex->writePreviousPageId(newIndex->getPageId());
        //clean home 
        delete nextIndex;
    } else {
        fullIndex->writePreviousPageId(newIndex->getPageId());
    }//endif    
    //clean home 
    for (i = 0; i < total; i++) {
        if (i != posKey) {
            delete vectKey[i];
        }
    }//endfor
    delete [] vectKey;
    delete [] vectSub;
    delete [] serialHalf;
    delete newIndex;

    return promote;
}//splitIndex

//----------------------------------------------------------------------

template <typename K>
typename BTree<K>::BTreePromotion * BTree<K>::splitLeaf(BTreeLeaf<K> * fullLeaf, K * key, Uuid * uuid, Session * se) {
    int total = fullLeaf->readNumberOfKeys() + 1;
    int half = ((int) (total / 2));
    K * objHalf = new K();
    Byte * serialHalf;
    BTreeLeaf<K> * newLeaf = new BTreeLeaf<K>(se->create());
    BTreeLeaf<K> * nextLeaf;
    BTreePromotion * promote;
    long nextPageId;
    int posKey;
    int i;
    // Copy all keys and uuid
    Uuid ** vecUuid = new Uuid*[total];
    K ** vectKey = new K*[total];
    // Copy key and uuid
    for (i = 0; i < total - 1; i++) {
        vectKey[i] = fullLeaf->buildKey(i);
        vecUuid[i] = fullLeaf->readEntityUuid(i);
    }//endfor
    // Moving for find position of the key
    while (i > 0) { 
        averageForAdd->incrementVerification(1);
        if (key->compareTo(vectKey[i - 1]) < 0) {
            // Copy the key and pageId in the last position
            vectKey[i] = vectKey[i - 1];
            vecUuid[i] = vecUuid[i - 1];
            i--;
        }
        else {
            break;
        }
    }//endwhile
    // Copy key and pageId in the correct position
    posKey = i;
    vectKey[i] = key;
    // Copy the pageId in the correct position
    vecUuid[i] = uuid;
    // Copy lower half of keys and uuid		
    fullLeaf->addFirstKey(vectKey[0], vecUuid[0]);
    for (i = 1; i < half; i++) {
        fullLeaf->addKey(vectKey[i], vecUuid[i]);
    }//endfor
    // Copy upper half of keys and uuid
    newLeaf->addFirstKey(vectKey[half], vecUuid[half]);
    for (i = half + 1; i < total; i++) {
        newLeaf->addKey(vectKey[i], vecUuid[i]);
    }//endfor
    //copy half object 
    serialHalf = new Byte[vectKey[half]->sizeOfKey()];
    vectKey[half]->pushKey(serialHalf, 0);
    objHalf->pullKey(serialHalf, 0);
    promote = new BTreePromotion(objHalf, newLeaf->getPageId());
    // Circularly link
    newLeaf->writeNextPageId(fullLeaf->readNextPageId());
    fullLeaf->writeNextPageId(newLeaf->getPageId());
    newLeaf->writePreviousPageId(fullLeaf->getPageId());
    nextPageId = newLeaf->readNextPageId();
    if (nextPageId != fullLeaf->getPageId()) {
        nextLeaf = new BTreeLeaf<K>(se->load(nextPageId));
        nextLeaf->writePreviousPageId(newLeaf->getPageId());
        delete nextLeaf;
    } else {
        fullLeaf->writePreviousPageId(newLeaf->getPageId());
    }
    //clean home 
    for (i = 0; i < total; i++) {
        if (i != posKey) {
            delete vectKey[i];
            delete vecUuid[i];
        }
    }//endfor
    delete [] vectKey;
    delete [] vecUuid;
    delete [] serialHalf;
    delete newLeaf;
    return promote;
}
//----------------------------------------------------------------------

template <typename K>
int BTree<K>::height() {   
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(BTreeDescriptor::nodeType);
    BTreeDescriptor * descriptor = new BTreeDescriptor(se->load(pageIdDescriptor));
    int total = 0;
    long rootPageId = descriptor->readRootPageId();
    
    Node * node = NULL;
    BTreeIndex<K> * index = NULL;
    long sub;

    if (rootPageId != 0) {
        node = se->load(rootPageId);
        while (BTreeIndex<K>::matchNodeType(node)) {
            index = new BTreeIndex<K>(node);
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

#endif	/* ORG_OBINJECT_STORAGE_BTREE_H */

