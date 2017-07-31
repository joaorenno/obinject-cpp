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
#ifndef ORG_OBINJECT_BLOCK_RTREELEAF
#define	ORG_OBINJECT_BLOCK_RTREELEAF

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/RTreeNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class RTreeLeaf : public RTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfUuid;
    }//sizeOfEntry

    void writeEntityUuid(Int idx, Uuid * uuid);
public:
    static const Int nodeType = 6;

    RTreeLeaf(Node * node) : RTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~RTreeLeaf() {
    }

    Bool addKey(K * key, Uuid * uuid);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == RTreeLeaf<K>::nodeType;
    }//matchNodeType

    Uuid * readEntityUuid(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool RTreeLeaf<K>::addKey(K * key, Uuid * uuid) {
    Int total = this->readNumberOfKeys();
    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace(size)) { // It there is not space.
        return false;
    } else {
        Int off = this->getOffset(total, size);
        key->pushKey(this->getArray(), off); // Adding
        this->writeEntityUuid(total, uuid);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid * RTreeLeaf<K>::readEntityUuid(Int idx) {
    int pos = RTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readUuid(pos);
}//readEntityUuid

//----------------------------------------------------------------------

template <typename K>
void RTreeLeaf<K>::writeEntityUuid(Int idx, Uuid * uuid) {
    int pos = RTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeUuid(pos, uuid);
}//writeEntityUuid

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_RTREELEAF */

