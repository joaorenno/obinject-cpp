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
#ifndef ORG_OBINJECT_BLOCK_BTREENODE
#define	ORG_OBINJECT_BLOCK_BTREENODE

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/KeyNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class BTreeNode : public KeyNode<K> {
protected:
    Long calculatedComparisons;
    
    void decrementNumberOfKey();

    Int freeSpace();

    void incrementNumberOfKey();

    Int readOffset(Int idx);

    virtual Int sizeOfEntry() = 0;

    virtual Int sizeOfFeatures() = 0;

    void writeOffset(Int idx, Int off);

public:

    BTreeNode(Node * node) : KeyNode<K>(node) {
        static_assert(is_base_of<Order<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_base_of<Comparable<K>, K>::value, "Generic type is not derived of the Comparable class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        calculatedComparisons = 0;
    }

    virtual ~BTreeNode();

    K * buildKey(Int idx);

    virtual void clear();

    Int indexOfKey(K * key);

    virtual Int readNumberOfKeys();
    
    Long getCalculatedComparisons() {
        return calculatedComparisons;
    }

};

//----------------------------------------------------------------------

template <typename K>
inline BTreeNode<K>::~BTreeNode() {
}

//----------------------------------------------------------------------

template <typename K>
K * BTreeNode<K>::buildKey(Int idx) {
    K * key;
    if ((idx >= 0) && (idx < this->readNumberOfKeys())) {
        key = new K();
        //pull the key of the page
        key->pullKey(this->getArray(), this->readOffset(idx));
        return key;
    }//endif
    return NULL;
}//buildKey

//----------------------------------------------------------------------

template <typename K>
void BTreeNode<K>::clear() {
    //zering numberOfKeys
    int pos = BTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
void BTreeNode<K>::decrementNumberOfKey() {
    int num = this->readNumberOfKeys() - 1;
    int pos = BTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//decrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int BTreeNode<K>::freeSpace() {
    int num = this->readNumberOfKeys();
    if (num == 0) {
        return this->sizeOfArray() - BTreeNode<K>::sizeOfHeader() - this->sizeOfFeatures();
    } else {
        return this->sizeOfArray() - BTreeNode<K>::sizeOfHeader()
                - this->sizeOfFeatures() - (num * this->sizeOfEntry())
                - (this->sizeOfArray() - this->readOffset(num - 1));
    }//endif
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
void BTreeNode<K>::incrementNumberOfKey() {
    int num = this->readNumberOfKeys() + 1;
    int pos = BTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//incrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int BTreeNode<K>::indexOfKey(K * key) {
    // binary search
    int half;
    int lower = 0;
    int upper = this->readNumberOfKeys() - 1;
    K * objHalf;
    //find binary seach
    while (lower <= upper) {
        half = lower + ((upper - lower) / 2);
        objHalf = buildKey(half);
        calculatedComparisons++;
        if (key->compareTo(objHalf) == 0) {
            delete objHalf;
            return half;
        } else { 
            calculatedComparisons++;
            if (key->compareTo(objHalf) < 0) {
                upper = half - 1;
            } else {
                lower = half + 1;
            }//endif
        }
        delete objHalf;
    }//endwhile		
    return -1;
}//indexOfKey

//----------------------------------------------------------------------

template <typename K>
Int BTreeNode<K>::readNumberOfKeys() {
    int pos = BTreeNode<K>::sizeOfHeader();
    return this->readInt(pos);
}//readNumberOfKeys

//----------------------------------------------------------------------

template <typename K>
Int BTreeNode<K>::readOffset(Int idx) {
    int pos = BTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    return this->readInt(pos);
}//readOffset

//----------------------------------------------------------------------

template <typename K>
void BTreeNode<K>::writeOffset(int idx, int off) {
    int pos = BTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    this->writeInt(pos, off);
}//writeOffset    

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_BTREENODE */

