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
#ifndef ORG_OBINJECT_BLOCK_BTREELEAF
#define	ORG_OBINJECT_BLOCK_BTREELEAF

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/BTreeNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class BTreeLeaf : public BTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfUuid + Node::sizeOfInt;
    }//sizeOfEntry

    Int sizeOfFeatures() {
        return Node::sizeOfInt;
    }//sizeOfFeatures

    void writeEntityUuid(Int idx, Uuid * uuid);
public:
    static const Int nodeType = 4;

    BTreeLeaf(Node * node) : BTreeNode<K>(node) {
        static_assert(is_base_of<Order<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_base_of<Comparable<K>, K>::value, "Generic type is not derived of the Comparable class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~BTreeLeaf() {
    }

    Bool addFirstKey(K * key, Uuid * uuid);

    Bool addKey(K * key, Uuid * uuid);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == BTreeLeaf<K>::nodeType;
    }//matchNodeType

    Uuid * readEntityUuid(Int idx);

    Bool remove(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool BTreeLeaf<K>::addFirstKey(K * key, Uuid * uuid) {
    int off;
    int size = key->sizeOfKey();
    //reset node
    this->clear();
    if (size + this->sizeOfEntry() < this->freeSpace()) {
        //insert offset
        off = this->sizeOfArray() - size;
        this->writeOffset(0, off);
        //push the key in page
        key->pushKey(this->getArray(), off);
        //insert subPages
        this->writeEntityUuid(0, uuid);
        this->incrementNumberOfKey();
        return true;
    } else {
        return false;
    }//endif
}//addFirstKey

//----------------------------------------------------------------------

template <typename K>
Bool BTreeLeaf<K>::addKey(K * key, Uuid * uuid) {
    int i;
    int total = this->readNumberOfKeys();
    int len, dest, src, idx = 0, off;
    int size = key->sizeOfKey();
    K * obj;
    Uuid * entityUuui;

    if ((total > 0) && (size + this->sizeOfEntry() < this->freeSpace())) {
        //positions dest and source
        dest = this->readOffset(total - 1) - size;
        src = this->readOffset(total - 1);
        //locating position
        for (i = total; i > 0; i--) {
            obj = this->buildKey(i - 1);
            this->calculatedComparisons++;
            if (obj->compareTo(key) < 0) {
                idx = i;
                //clean home
                delete obj;
                break;
            } else {
                //move offset
                this->writeOffset(i, (this->readOffset(i - 1) - size));
                //move subPageId
                entityUuui = this->readEntityUuid(i - 1);
                this->writeEntityUuid(i, entityUuui);
                //clean home
                delete entityUuui;
                delete obj;
            }//endif
        }//endfor
        //if is first position
        if (idx == 0) {
            len = this->sizeOfArray() - src;
            off = this->sizeOfArray() - size;
        } else {
            len = this->readOffset(idx - 1) - src;
            off = this->readOffset(idx - 1) - size;
        }//endif
        // move keys			
        this->move(dest, src, len);
        //offset
        this->writeOffset(idx, off);
        this->writeEntityUuid(idx, uuid);
        //push the key in page
        key->pushKey(this->getArray(), off);
        this->incrementNumberOfKey();
        return true;
    } else {
        return false;
    }//endif
}//addKey

//----------------------------------------------------------------------

template <typename K>
Uuid * BTreeLeaf<K>::readEntityUuid(Int idx) {
    int pos = AbstractNode::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readUuid(pos);
}//readEntityUuid

//----------------------------------------------------------------------

template <typename K>
Bool BTreeLeaf<K>::remove(Int idx) {
    int totalKey = this->readNumberOfKeys();
    int dest, src, len, sizeKey;
    Uuid * entityUuid;
    if ((idx >= 0) && (idx < totalKey)) {
        if (idx == 0) {
            sizeKey = this->sizeOfArray() - this->readOffset(0);
        } else {
            sizeKey = this->readOffset(idx - 1) - this->readOffset(idx);
        }
        src = this->readOffset(totalKey - 1);
        dest = src + sizeKey;
        len = this->readOffset(idx) - this->readOffset(totalKey - 1);
        this->move(dest, src, len);
        for (int i = idx; i < (totalKey - 1); i++) {
            this->writeOffset(i, this->readOffset(i + 1) + sizeKey);
            entityUuid = this->readEntityUuid(i + 1);
            this->writeEntityUuid(i, entityUuid);
            //clean home
            delete entityUuid;
        }
        this->decrementNumberOfKey();
        return true;
    } else {
        return false;
    }
}

//----------------------------------------------------------------------

template <typename K>
void BTreeLeaf<K>::writeEntityUuid(Int idx, Uuid * uuid) {
    int pos = AbstractNode::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeUuid(pos, uuid);
}//writeEntityUuid

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_BTREELEAF */

