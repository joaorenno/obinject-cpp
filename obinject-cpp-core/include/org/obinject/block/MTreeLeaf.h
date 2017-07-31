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
#ifndef ORG_OBINJECT_BLOCK_MTREELEAF
#define	ORG_OBINJECT_BLOCK_MTREELEAF

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/MTreeNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class MTreeLeaf : public MTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfUuid + Node::sizeOfDouble + Node::sizeOfInt;
    }//sizeOfEntry

    void writeEntityUuid(Int idx, Uuid * uuid);
public:
    static const Int nodeType = 6;

    MTreeLeaf(Node * node) : MTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~MTreeLeaf() {
    }

    Bool addKey(K * key, Double dist, Uuid * uuid);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == MTreeLeaf<K>::nodeType;
    }//matchNodeType

    Uuid * readEntityUuid(Int idx);

    void remove(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool MTreeLeaf<K>::addKey(K * key, Double dist, Uuid * uuid) {
    Int total = this->readNumberOfKeys();

    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace()) // It there is not space.
    {
        return false;
    } else {
        int off = total == 0 ? this->sizeOfArray() - size : this->readOffset(total - 1) - size;
        key->pushKey(this->getArray(), off); // Adding
        this->writeOffset(total, off);
        this->writeDistanceToParent(total, dist);
        this->writeEntityUuid(total, uuid);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid * MTreeLeaf<K>::readEntityUuid(Int idx) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readUuid(pos);
}//readEntityUuid

//----------------------------------------------------------------------

template <typename K>
void MTreeLeaf<K>::remove(Int idx) {
    int sizeKey = idx == 0
            ? this->sizeOfArray() - this->readOffset(0)
            : this->readOffset(idx - 1) - this->readOffset(idx);
    int total = this->readNumberOfKeys();
    int length = this->readOffset(idx) - this->readOffset(total - 1);
    int source = this->readOffset(total - 1);
    int dest = source + sizeKey;

    this->move(dest, source, length);

    for (int k = idx; k < total - 1; k++) {
        this->writeOffset(k, this->readOffset(k + 1) + sizeKey);
        this->writeDistanceToParent(k, this->readDistanceToParent(k + 1));
        this->writeEntityUuid(k, this->readEntityUuid(k + 1));
    }
    this->decrementNumberOfKeys();
}

//----------------------------------------------------------------------

template <typename K>
void MTreeLeaf<K>::writeEntityUuid(Int idx, Uuid * uuid) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeUuid(pos, uuid);
}//writeEntityUuid

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_MTREELEAF */

