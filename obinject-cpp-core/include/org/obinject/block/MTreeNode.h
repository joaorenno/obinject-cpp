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
#ifndef ORG_OBINJECT_BLOCK_MTREENODE
#define	ORG_OBINJECT_BLOCK_MTREENODE

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/KeyNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class MTreeNode : public KeyNode<K> {
protected:
    long calculatedDistance;
    
    void decrementNumberOfKeys();

    Int freeSpace();

    void incrementNumberOfKeys();

    Int readOffset(Int idx);

    virtual Int sizeOfEntry() = 0;

    Int sizeOfFeatures() {
        return MTreeNode<K>::sizeOfInt;
    }

    void writeOffset(Int idx, Int off);

public:

    MTreeNode(Node * node) : KeyNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        calculatedDistance = 0;
    }
    
    long getCalculatedDistance() {
        return calculatedDistance;
    }

    virtual ~MTreeNode();

    K * buildKey(Int idx);

    void clear();

    Int indexOfKey(K * key);

    Int readNumberOfKeys();

    Double readDistanceToParent(Int idx);

    virtual void remove(Int idx) = 0;

    Bool replace(Int idx, K * replaceKey);

    void writeDistanceToParent(Int idx, Double dist);



};

//----------------------------------------------------------------------

template <typename K>
inline MTreeNode<K>::~MTreeNode() {
}

//----------------------------------------------------------------------

template <typename K>
K * MTreeNode<K>::buildKey(Int idx) {
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
void MTreeNode<K>::clear() {
    //zering numberOfKeys
    int pos = MTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::decrementNumberOfKeys() {
    int num = this->readNumberOfKeys() - 1;
    int pos = MTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//decrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::freeSpace() {
    int num = this->readNumberOfKeys();
    if (num == 0) {
        return this->sizeOfArray() - MTreeNode<K>::sizeOfHeader() - this->sizeOfFeatures();
    } else {
        return this->sizeOfArray() - MTreeNode<K>::sizeOfHeader()
                - this->sizeOfFeatures() - (num * this->sizeOfEntry())
                - (this->sizeOfArray() - this->readOffset(num - 1));
    }//endif
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::indexOfKey(K * key) {
    K * objKey;
	int total = this->readNumberOfKeys();

	for (int i = 0; i < total; i++)
	{
	    objKey = this->buildKey(i);
	    if (objKey->distanceTo(key) == 0)
	    {
		return i;
	    }
	}
        delete objKey;

	return -1;
}//indexOfKey

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::incrementNumberOfKeys() {
    int num = this->readNumberOfKeys() + 1;
    int pos = MTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//incrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Double MTreeNode<K>::readDistanceToParent(Int idx) {
    Int pos = MTreeNode<K>::sizeOfHeader()
            + this->sizeOfFeatures()
            + ((idx + 1) * this->sizeOfEntry())
            - MTreeNode<K>::sizeOfInt // off
            - MTreeNode<K>::sizeOfDouble; // parent distance

    return this->readDouble(pos);
}//readDistanceToParent

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::readNumberOfKeys() {
    int pos = MTreeNode<K>::sizeOfHeader();
    return this->readInt(pos);
}//readNumberOfKeys

//----------------------------------------------------------------------

template <typename K>
Int MTreeNode<K>::readOffset(Int idx) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    return this->readInt(pos);
}//readOffset

//----------------------------------------------------------------------

template <typename K>
Bool MTreeNode<K>::replace(Int idx, K * replaceKey) {
    Int total = this->readNumberOfKeys();
    Int offLastKey = this->readOffset(total - 1);
    Int length = this->readOffset(idx) - offLastKey;
    Int sizeOldKey = idx == 0 ? this->sizeOfArray() - this->readOffset(0) : this->readOffset(idx - 1) - this->readOffset(idx);
    Int offSize = sizeOldKey - replaceKey->sizeOfKey();
    if (replaceKey->sizeOfKey() <= this->freeSpace() + sizeOldKey) {
        this->move(offLastKey + offSize, offLastKey, length);
        for (int i = idx; i <= total - 1; i++) {
            this->writeOffset(i, this->readOffset(i) + offSize);
        }
        replaceKey->pushKey(this->getArray(), this->readOffset(idx));
        return true;
    } else {
        return false;
    }
}//replace

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::writeDistanceToParent(Int idx, Double dist) {
    int pos = MTreeNode<K>::sizeOfHeader()
            + this->sizeOfFeatures() 
            + ((idx + 1) * this->sizeOfEntry()) 
            - MTreeNode<K>::sizeOfInt 
            - MTreeNode<K>::sizeOfDouble;
    this->writeDouble(pos, dist);
}//writeDistanceToParent    

//----------------------------------------------------------------------

template <typename K>
void MTreeNode<K>::writeOffset(int idx, int off) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx + 1) * this->sizeOfEntry()) - Node::sizeOfInt;
    this->writeInt(pos, off);
}//writeOffset    


} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_MTREENODE */

