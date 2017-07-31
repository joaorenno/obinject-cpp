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
#ifndef ORG_OBINJECT_BLOCK_RTREEINDEX
#define	ORG_OBINJECT_BLOCK_RTREEINDEX

#include <cfloat>
#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/storage/all.h>
#include <typeinfo>
#include <iomanip>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class RTreeIndex : public RTreeNode<K> {
private:
    bool test;           //controla a saida do COUT (joaorenno)
    static Long numMbrs;
    
protected:
    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfLong;
    }//sizeOfEntry

    void writeSubPageId(Int idx, Long pageId);

public:
    static const Int nodeType = 9;

    RTreeIndex(Node * node) : RTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~RTreeIndex() {
    }

    Bool addKey(K * key, Long subPageId);
    
    Bool addKey(K * key1, Long subPageId1, K * key2, Long subPageId2);
    
    Int indexOfInsertion(K * key, EuclideanGeometry<K> * geometry);
    
    Int indexOfSubPageId(Long pageId);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == RTreeIndex<K>::nodeType;
    }//matchNodeType
    
    static Long getNumMbrs() {
        return numMbrs;
    }
    
    bool getTest() {
        return test;
    }
    
    void setTest(int test) {
        this->test = test;
    }
    
    static void resetNumMbrs() {
        numMbrs = 0;
    }

    Long readSubPageId(Int idx);

    void replace(Int idx, K key);

};

//----------------------------------------------------------------------
template <typename K>
Long RTreeIndex<K>::numMbrs = 0;

template <typename K>
Bool RTreeIndex<K>::addKey(K * key, Long subPageId) {
    Int total = this->readNumberOfKeys();
    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace(size)) {   // It there is not space.
        return false;
    } else {
        numMbrs++;
        Int off = this->getOffset(total, size);
        key->pushKey(this->getArray(), off);
        this->writeSubPageId(total, subPageId);
        this->incrementNumberOfKeys();
        
        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Bool RTreeIndex<K>::addKey(K * key1, Long subPageId1, K * key2, Long subPageId2) {
    Int size = key1->sizeOfKey();

    if (size + size + 2 * this->sizeOfEntry() > this->freeSpace(size)) {
        return false;
    } else {
        numMbrs++;
        addKey(key1, subPageId1);
        addKey(key2, subPageId2);
        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Int RTreeIndex<K>::indexOfInsertion(K * key, EuclideanGeometry<K> * geometry) {
    Int total = this->readNumberOfKeys(); // Number of keys in node
    Double minOccupancy = DBL_MAX; // Minimum occupancy (area, volume...) of MBR.
    Double minEnlargement = DBL_MAX; // Minimum enlargement of occupancy (area, volume...) of MBR.
    Int idx = -1; // Qualified sub page
    K * storedKey; // To build keys stored into this node.
    K * newMbr = NULL;
    
    for (int i = 0; i < total; i++) {
        storedKey = this->buildKey(i);
        
        // Calc occupancy of stored key
//        cout << "key - " << storedKey->getWidth() << endl;
        Double befOcup = geometry->occupancy(storedKey); // Occupancy (area, volume...) BEFORE enlargement.
//        if(test) cout << std::setprecision(10) << "befOcup - " << befOcup << endl;
        K * mbrUnion = geometry->rectUnion(key, storedKey); // Enlargment
        Double aftOcup = geometry->occupancy(mbrUnion); // Occupancy (area, volume...) AFTER enlargement.
//        if(test) cout << std::setprecision(10) << "aftOcup - " << aftOcup << endl;

        Double enlarg = aftOcup - befOcup; // Enlargement of occupancy

        // Qualifies key with minimum enlargement. Resolve ties selecting minimum occupancy (area, volume...)    
        if (enlarg < minEnlargement) {
            minEnlargement = enlarg;
            minOccupancy = befOcup;
            idx = i; // Qualify
            //clean home
            delete newMbr;
            newMbr = mbrUnion;
        } else if (enlarg == minEnlargement && befOcup < minOccupancy) {
            minOccupancy = befOcup;
            idx = i; // Qualify
            //clean home
            delete newMbr;
            newMbr = mbrUnion;
        }else{
            //clean home
            delete mbrUnion;
        }
        //clean home
        delete storedKey;
    }

    // If MBR was enlarged:
    if (minEnlargement != 0)
    {
        // Writes enlargement
        newMbr->pushKey(this->getArray(), this->getOffset(idx, newMbr->sizeOfKey()));
    }
    //clean home
    delete newMbr;

    return idx;
        
}//indexOfInsertion

//----------------------------------------------------------------------

template <typename K>
Int RTreeIndex<K>::indexOfSubPageId(Long pageId) {
    Int total = this->readNumberOfKeys();
    for (int i = 0; i < total; i++) {
        if (this->readSubPageId(i) == pageId) {
            return i;
        }
    }

    return -1;
}//indexOfSubPageId

//----------------------------------------------------------------------

template <typename K>
Long RTreeIndex<K>::readSubPageId(Int idx) {
    int pos = RTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readLong(pos);
}//readSubPageId

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::replace(Int idx, K key) {
    Int off = this->getOffset(idx, key.sizeOfKey());
    key.pushKey(this->getArray(), off);
}//replace

//----------------------------------------------------------------------

template <typename K>
void RTreeIndex<K>::writeSubPageId(Int idx, Long pageId) {
    int pos = RTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeLong(pos, pageId);
}//writeSubPageId

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_RTREEINDEX */