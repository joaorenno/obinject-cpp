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
#ifndef ORG_OBINJECT_BLOCK_BTREEINDEX
#define	ORG_OBINJECT_BLOCK_BTREEINDEX

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/BTreeNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class BTreeIndex : public BTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfLong + Node::sizeOfInt;
    }//sizeOfEntry

    Int sizeOfFeatures() {
        return Node::sizeOfInt + Node::sizeOfLong;
    }//sizeOfFeatures

    void writeSubPageId(Int idx, Long pageId);

public:
    static const Int nodeType = 5;

    BTreeIndex(Node * node) : BTreeNode<K>(node) {
        static_assert(is_base_of<Order<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_base_of<Comparable<K>, K>::value, "Generic type is not derived of the Comparable class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~BTreeIndex() {
    }

    Bool addFirstKey(K * key, Long leftId, Long rightId);

    Bool addKey(K * key, Long pageId);

    Int indexOfQualify(K * key);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == BTreeIndex<K>::nodeType;
    }//matchNodeType

    Long readSubPageId(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool BTreeIndex<K>::addFirstKey(K * key, Long leftId, Long rightId) {
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
        writeSubPageId(0, leftId);
        writeSubPageId(1, rightId);
        this->incrementNumberOfKey();
        return true;
    } else {
        return false;
    }//endif
}//addFirstKey

//----------------------------------------------------------------------

template <typename K>
Bool BTreeIndex<K>::addKey(K * key, Long pageId) {
    int i;
    int total = this->readNumberOfKeys();
    int len, dest, src, idx = 0, off;
    int size = key->sizeOfKey();
    K * obj;
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
                this->writeSubPageId(i + 1, this->readSubPageId(i));
            }//endif
            //clean home
            delete obj;
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
        this->writeSubPageId(idx + 1, pageId);
        //push the key in page
        key->pushKey(this->getArray(), off);
        //increment
        this->incrementNumberOfKey();
        return true;
    } else {
        return false;
    }//endif	
}//addKey

//----------------------------------------------------------------------

template <typename K>
Int BTreeIndex<K>::indexOfQualify(K * key) {
    // binary search
    K * obj;
    int idx;
    int total = this->readNumberOfKeys();
    int half = 0;
    int lower = 0;
    int upper = total;
    //find binary seach
    while (lower < upper) {
        half = lower + ((upper - lower) / 2);
        obj = this->buildKey(half);
        this->calculatedComparisons++;
        if (key->compareTo(obj) <= 0) {
            //clean home
            delete obj;
            if (upper == half) {
                break;
            }
            upper = half;
        } else {
            //clean home
            delete obj;
            if (lower == half) {
                break;
            }
            lower = half;
        }//endif
    }
    if (half > 0) {
        idx = half - 1;
    } else {
        idx = half;
    }
    obj = this->buildKey(idx);
    while (key->compareTo(obj) >= 0) {
        this->calculatedComparisons++;
        idx++;
        if (idx < total) {
            //clean home
            delete obj;
            obj = this->buildKey(idx);
        } else {
            break;
        }
    }
    //clean home
    delete obj;
    this->calculatedComparisons++;
    return idx;
}//indexOfQualify

//----------------------------------------------------------------------

template <typename K>
Long BTreeIndex<K>::readSubPageId(Int idx) {
    int pos;
    if (idx == 0) {
        pos = BTreeIndex<K>::sizeOfHeader() + Node::sizeOfInt;
    } else {
        pos = BTreeIndex<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx - 1) * this->sizeOfEntry());
    }//endif
    return this->readLong(pos);
}//readSubPageId

//----------------------------------------------------------------------

template <typename K>
void BTreeIndex<K>::writeSubPageId(Int idx, Long pageId) {
    int pos;
    if (idx == 0) {
        pos = BTreeIndex<K>::sizeOfHeader() + Node::sizeOfInt;
    } else {
        pos = BTreeIndex<K>::sizeOfHeader() + this->sizeOfFeatures() + ((idx - 1) * this->sizeOfEntry());
    }//endif
    this->writeLong(pos, pageId);
}//writeSubPageId

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_BTREEINDEX */

