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
#ifndef ORG_OBINJECT_BLOCK_SEQUENTIALNODE_H
#define ORG_OBINJECT_BLOCK_SEQUENTIALNODE_H

#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Entity.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/EntityNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename E>
class SequentialNode : public EntityNode<E> {
protected:
    static const Int sizeOfFeatures = Node::sizeOfLong + Node::sizeOfInt;

    static const Int sizeOfEntry = Node::sizeOfInt;

    void decrementNumberOfEntities();

    Int freeSpace();

    void incrementNumberEntities();

    Int getNodeType();

    int readOffset(Int idx);

    void writeOffset(Int idx, Int off);

public:
    static const Int nodeType = 3;

    SequentialNode(Node * node);

    virtual ~SequentialNode() {
    }

    Bool addEntity(E * entity);

    E * buildEntity(Int idx);

    void clear();

    Uuid* findEntity(E* entity);

    E* findUuid(Uuid * uuid);

    static Bool matchNodeType(Node* node);

    Int readNumberOfEntities();

};

//----------------------------------------------------------------------

template <typename E>
SequentialNode<E>::SequentialNode(Node * node) : EntityNode<E>(node) {
    static_assert(is_base_of<Entity<E>, E>::value, "Generic type is not derived of the Entity class");
    this->initialize(node);
}//SequentialNode

//----------------------------------------------------------------------

template <typename E>
Int SequentialNode<E>::freeSpace() {
    int size = this->readNumberOfEntities();
    if (size == 0) {
        return this->sizeOfArray() - Node::sizeOfHeader();
    } else {
        return this->sizeOfArray() - Node::sizeOfHeader() -
                sizeOfFeatures - (size * sizeOfEntry) -
                (this->sizeOfArray() - readOffset(size - 1));
    }//endif
}//freeSpace

//----------------------------------------------------------------------

template <typename E>
E * SequentialNode<E>::buildEntity(Int idx) {
    if ((idx >= 0) && (idx < this->readNumberOfEntities())) {
        E * entity = new E();
        entity->pullEntity(this->getArray(), readOffset(idx));
        return entity;
    } else {
        return NULL;
    }//endif
}//buildEntity

//----------------------------------------------------------------------

template <typename E>
Int SequentialNode<E>::readOffset(Int idx) {
    int pos = Node::sizeOfHeader() + sizeOfFeatures + (idx * sizeOfEntry);
    return this->readInt(pos);
}//readOffset

//----------------------------------------------------------------------

template <typename E>
void SequentialNode<E>::writeOffset(Int idx, Int off) {
    int pos = Node::sizeOfHeader() + sizeOfFeatures + (idx * sizeOfEntry);
    this->writeInt(pos, off);
}//writeOffset

//----------------------------------------------------------------------

template <typename E>
void SequentialNode<E>::incrementNumberEntities() {
    int size = this->readNumberOfEntities() + 1;
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, size);
}//incrementNumberEntities

//----------------------------------------------------------------------

template <typename E>
void SequentialNode<E>::decrementNumberOfEntities() {
    int size = this->readNumberOfEntities() - 1;
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, size);
}//decrementNumberOfEntities

//----------------------------------------------------------------------

template <typename E>
void SequentialNode<E>::clear() {
    //zering next node
    int pos = Node::sizeOfHeader();
    this->writeLong(pos, 0);
    //zering numberOfKey
    pos = Node::sizeOfHeader() + Node::sizeOfLong;
    this->writeInt(pos, 0);
}//clear

//----------------------------------------------------------------------

template <typename E>
Bool SequentialNode<E>::addEntity(E * entity) {
    int off;
    int idx = this->readNumberOfEntities();

    if (entity->sizeOfEntity() + sizeOfEntry < freeSpace()) {
        //if is first position
        if (idx == 0) {
            off = this->sizeOfArray() - entity->sizeOfEntity();
        } else {
            off = readOffset(idx - 1) - entity->sizeOfEntity();
        }//endif
        //offset
        writeOffset(idx, off);
        //entity
        entity->pushEntity(this->getArray(), off);
        //increment
        incrementNumberEntities();
        //clean home
        return true;
    } else {
        //clean home
        return false;
    }//endif
}//addEntity

//----------------------------------------------------------------------

template <typename E>
Uuid* SequentialNode<E>::findEntity(E* entity) {
    int i;
    Entity<E> * objSer;
    int total = readNumberOfEntities();
    for (i = 0; i < total; i++) {
        objSer = buildEntity(i);
        if (objSer->isEqual(entity) == true) {
            //clean home
            Uuid *uuid = new Uuid(
                    objSer->getUuid()->getMostSignificantBits(),
                    objSer->getUuid()->getLeastSignificantBits());
            delete objSer;
            return uuid;
        }//endif
        //clean home
        delete objSer;
    }//endfor
    return NULL;
}//findEntity

//----------------------------------------------------------------------

template <typename E>
E* SequentialNode<E>::findUuid(Uuid * uuid) {
    int i;
    E * objSer;
    int total = this->readNumberOfEntities();
    for (i = 0; i < total; i++) {
        objSer = this->buildEntity(i);
        if (objSer->getUuid()->equals(uuid)) {
            return objSer;
        }//endif
        delete objSer;
    }//endfor
    return NULL;
}//findUuid

//----------------------------------------------------------------------

template <typename E>
Int SequentialNode<E>::getNodeType() {
    return this->nodeType;
}//getNodeType

//----------------------------------------------------------------------

template <typename E>
Int SequentialNode<E>::readNumberOfEntities() {
    int pos = Node::sizeOfHeader() + Node::sizeOfLong;
    return this->readInt(pos);
}//readNumberOfEntities

//----------------------------------------------------------------------

} /* block */
} /* obinject */
} /* org */

#endif /* ORG_OBINJECT_BLOCK_SEQUENTIALNODE_H */

