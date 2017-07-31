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
#ifndef ORG_OBINJECT_BLOCK_RTREENODE
#define	ORG_OBINJECT_BLOCK_RTREENODE

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/KeyNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class RTreeNode : public KeyNode<K> {
protected:
    
    void decrementNumberOfKeys();

    Int freeSpace(Int sizeOfKey);

    void incrementNumberOfKeys();

    Int getOffset(Int idx, Int sizeOfKey);

    virtual Int sizeOfEntry() = 0;

    Int sizeOfFeatures() {
        return Node::sizeOfInt;
    }

public:

    RTreeNode(Node * node) : KeyNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    }

    virtual ~RTreeNode();

    K * buildKey(Int idx);

    void clear();

    Int indexOfKey(K * key);

    Int readNumberOfKeys();


};

//----------------------------------------------------------------------

template <typename K>
inline RTreeNode<K>::~RTreeNode() {
}

//----------------------------------------------------------------------

template <typename K>
K * RTreeNode<K>::buildKey(Int idx) {
    if ((idx >= 0) && (idx < this->readNumberOfKeys())) {
        K * key = new K();
        //pull the key of the page
        key->pullKey(this->getArray(), this->getOffset(idx, key->sizeOfKey()));
        return key;
    }//endif
    return NULL;
}//buildKey

//----------------------------------------------------------------------

template <typename K>
void RTreeNode<K>::clear() {
    //zering numberOfKeys
    int pos = RTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename K>
void RTreeNode<K>::decrementNumberOfKeys() {
    int num = this->readNumberOfKeys() - 1;
    int pos = RTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//decrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::freeSpace(Int sizeOfKey) {
    int total = this->readNumberOfKeys();
    
    if (total == 0) {
        return this->sizeOfArray() - RTreeNode<K>::sizeOfHeader() - this->sizeOfFeatures();
    } else {
        return this->sizeOfArray() - RTreeNode<K>::sizeOfHeader()
                - this->sizeOfFeatures() - (total * this->sizeOfEntry())
                - (this->sizeOfArray() - this->getOffset(total - 1, sizeOfKey)); // Keys
    }//endif
}//freeSpace

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::indexOfKey(K * key) {
    K * objKey;
    int total = this->readNumberOfKeys();
//    cout << "KEY NUMBER - " << total << endl;

    for (int i = 0; i < total; i++)
    {
        objKey = this->buildKey(i);
        if (objKey->distanceTo(key) == 0)
        {
            //clean home
            delete objKey;
            return i;
        }
        //clean home
        delete objKey;
    }

    return -1;
}//indexOfKey

//----------------------------------------------------------------------

template <typename K>
void RTreeNode<K>::incrementNumberOfKeys() {
    int num = this->readNumberOfKeys() + 1;
    int pos = RTreeNode<K>::sizeOfHeader();
    this->writeInt(pos, num);
}//incrementNumberOfKey

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::readNumberOfKeys() {
    int pos = RTreeNode<K>::sizeOfHeader();
    return this->readInt(pos);
}//readNumberOfKeys

//----------------------------------------------------------------------

template <typename K>
Int RTreeNode<K>::getOffset(Int idx, Int sizeOfKey)
{
    return this->sizeOfArray() - ((idx + 1) * sizeOfKey);
}


} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_RTREENODE */

