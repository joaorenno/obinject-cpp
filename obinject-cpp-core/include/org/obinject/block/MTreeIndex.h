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
#ifndef ORG_OBINJECT_BLOCK_MTREEINDEX
#define	ORG_OBINJECT_BLOCK_MTREEINDEX

#include <cfloat>
#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/MTreeNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class MTreeIndex : public MTreeNode<K> {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

    Int sizeOfEntry() {
        return Node::sizeOfLong + Node::sizeOfDouble + Node::sizeOfDouble + Node::sizeOfInt;
    }//sizeOfEntry

    void writeSubPageId(Int idx, Long pageId);

public:
    static const Int nodeType = 7;

    MTreeIndex(Node * node) : MTreeNode<K>(node) {
        static_assert(is_base_of<Metric<K>, K>::value, "Generic type is not derived of the Order class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
        this->initialize(node);
    }

    virtual ~MTreeIndex() {
    }

    Bool addKey(K * key, Double dist, Long subPageId, Double covRad);

    Bool addKey(K * key1, Double dist1, Long subPageId1, Double covRad1, K * key2, Double dist2, Long subPageId2, Double covRad2);

    Double distanceToQualify(Int idx, K * insertKey);

    Int indexOfInsertion(K * key);

    Int indexOfSubPageId(Long pageId);

    Double readCoveringRadius(Int idx);

    void writeCoveringRadius(Int idx, Double rad);

    static Bool matchNodeType(Node * node) {
        return node->readNodeType() == MTreeIndex<K>::nodeType;
    }//matchNodeType

    Long readSubPageId(Int idx);

    void remove(Int idx);

};

//----------------------------------------------------------------------

template <typename K>
Bool MTreeIndex<K>::addKey(K * key, Double dist, Long subPageId, Double covRad) {
    Int total = this->readNumberOfKeys();

    Int size = key->sizeOfKey();

    if (size + this->sizeOfEntry() > this->freeSpace()) // It there is not space.
    {
        return false;
    } else {
        Int off = total == 0 ? this->sizeOfArray() - size : this->readOffset(total - 1) - size;
        key->pushKey(this->getArray(), off); // Adding
        this->writeOffset(total, off);
        this->writeDistanceToParent(total, dist);
        this->writeSubPageId(total, subPageId);
        this->writeCoveringRadius(total, covRad);
        this->incrementNumberOfKeys();

        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Bool MTreeIndex<K>::addKey(K * key1, Double dist1, Long subPageId1, Double covRad1, K * key2, Double dist2, Long subPageId2, Double covRad2) {
    Int sizeKey1 = key1->sizeOfKey();
    Int sizeKey2 = key2->sizeOfKey();

    if (sizeKey1 + sizeKey2 + 2 * this->sizeOfEntry() > this->freeSpace()) {
        return false;
    } else {
        addKey(key1, dist1, subPageId1, covRad1);
        addKey(key2, dist2, subPageId2, covRad2);
        return true;
    }
}//addKey

//----------------------------------------------------------------------

template <typename K>
Double MTreeIndex<K>::distanceToQualify(Int idx, K * insertKey) {
    K * key = this->buildKey(idx);
    this->calculatedDistance++;
    double dist = insertKey->distanceTo(key);
    delete key;
    return dist;
}//distanceToQualify

//----------------------------------------------------------------------

template <typename K>
Int MTreeIndex<K>::indexOfInsertion(K * key) {
    Double distIn = DBL_MAX; // Minimum distance if key is inside some obj covering radius.
    Double distOut = DBL_MAX; // Minimum distance if key is outside some obj covering radius.
    Double newCoverage = DBL_MAX;
    Int idxIn = 0; // Index of obj in which key has minimum distance (inside cov rad).
    Int idxOut = 0; // Index of obj in which key has minimum distance (outside cov rad).
    Bool in = false; // Weather key is inside some cov rad.
    Int total = this->readNumberOfKeys();

    for (int i = 0; i < total; i++) {
        K * storedKey = this->buildKey(i); // Key stored in the node.

        Double rad = this->readCoveringRadius(i); // storedKey cov rad: r(Or)

        this->calculatedDistance++;
        Double dist = storedKey->distanceTo(key); // Distance from key to storedKey: d(Or, On)

        if (in) {
            //exists a radii covering
            if (dist <= rad && dist < distIn) // Looking for minimum distance.
            {
                //these radius too cover and is less
                distIn = dist; // Minimum distance between storedKey and key.
                idxIn = i; // storedKey in which distance to key is minimum.
            }
        } else {
            //does not exists a radii covering
            if (dist <= rad) // key is inside storedKey covering radius.
            {
                //these radius cover 
                in = true; // !in to set true once.
                distIn = dist; // Minimum distance between storedKey and key.
                idxIn = i; // storedKey in which distance to key is minimum.
            } else {
                //these radius does not cover 
                if (dist - rad < distOut) // If key is outside any storedKey cov rad
                {
                    //minimize distOut
                    distOut = dist - rad; // Minimum enlargement of cov rad.
                    idxOut = i; // storedKey to minimum cov rad enlargement.
                    newCoverage = dist;
                }
            }
        }
        //clean home
        delete storedKey;
    }

    if (in) // Key is inside entry[idxIn] covering radius and the distance is minimum
    {
        return idxIn;
    } else // No entry cov rad contains key.
    {
        // Increase the most suitable (minimum enlargement) entry cov rad.			
        //this.writeCoveringRadius(idxOut, this.readCoveringRadius(idxOut) + distOut);
        this->writeCoveringRadius(idxOut, newCoverage);
        return idxOut;
    }
}//indexOfInsertion

//----------------------------------------------------------------------

template <typename K>
Int MTreeIndex<K>::indexOfSubPageId(Long pageId) {
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
void MTreeIndex<K>::remove(Int idx) {
    Int sizeKey = idx == 0
            ? this->sizeOfArray() - this->readOffset(0)
            : this->readOffset(idx - 1) - this->readOffset(idx);
    Int total = this->readNumberOfKeys();
    Int length = this->readOffset(idx) - this->readOffset(total - 1);
    Int source = this->readOffset(total - 1);
    Int dest = source + sizeKey;

    this->move(dest, source, length);

    for (int k = idx; k < total - 1; k++) {
        this->writeOffset(k, this->readOffset(k + 1) + sizeKey);
        this->writeDistanceToParent(k, this->readDistanceToParent(k + 1));
        this->writeSubPageId(k, this->readSubPageId(k + 1));

        this->writeCoveringRadius(k, this->readCoveringRadius(k + 1));
    }
    this->decrementNumberOfKeys();
}//remove

//----------------------------------------------------------------------

template <typename K>
Double MTreeIndex<K>::readCoveringRadius(Int idx) {
    Int pos = MTreeIndex<K>::sizeOfHeader()
            + this->sizeOfFeatures()
            + (idx * this->sizeOfEntry())
            + MTreeIndex<K>::sizeOfLong; // sub

    return this->readDouble(pos);
}//readCoveringRadius

//----------------------------------------------------------------------

template <typename K>
void MTreeIndex<K>::writeCoveringRadius(Int idx, Double rad) {
    int pos = MTreeIndex<K>::sizeOfHeader()
            + this->sizeOfFeatures()
            + (idx * this->sizeOfEntry())
            + MTreeIndex<K>::sizeOfLong; // sub

    this->writeDouble(pos, rad);
}//writeCoveringRadius

//----------------------------------------------------------------------

template <typename K>
Long MTreeIndex<K>::readSubPageId(Int idx) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    return this->readLong(pos);
}//readSubPageId

//----------------------------------------------------------------------

template <typename K>
void MTreeIndex<K>::writeSubPageId(Int idx, Long pageId) {
    int pos = MTreeNode<K>::sizeOfHeader() + this->sizeOfFeatures() + (idx * this->sizeOfEntry());
    this->writeLong(pos, pageId);
}//writeSubPageId

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_MTREEINDEX */

