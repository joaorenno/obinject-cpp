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
#ifndef ORG_OBINJECT_STORAGE_SEQUENTIAL_H
#define	ORG_OBINJECT_STORAGE_SEQUENTIAL_H

#include <type_traits>
#include <org/obinject/block/all.h>
#include <org/obinject/block/SequentialDescriptor.h>
#include <org/obinject/block/SequentialNode.h>
#include <org/obinject/device/all.h>
#include <org/obinject/device/Session.h>
#include <org/obinject/device/Workspace.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractEntityStructure.h>
#include <org/obinject/meta/all.h>

using namespace std;

namespace org {
namespace obinject {
namespace storage {

template <typename E>
class Sequential : AbstractEntityStructure<E> {
public:

    Sequential(Workspace * workspace);

    virtual ~Sequential() {
    }

    Bool add(E * entity);

    E * find(Uuid * uuid);

    Uuid * find(E * entity);

    Long getRootPageId();

    Bool remove(E *obj) {
        return false;
    }
};

//----------------------------------------------------------------------

template <typename E>
Sequential<E>::Sequential(Workspace * workspace) : AbstractEntityStructure<E>(workspace) {
    static_assert(is_base_of<Entity<E>, E>::value, "Generic type is not derived of the Entity class");
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    //clean home
    delete descriptor;
    se->close();
    delete se;
}//Sequential

//----------------------------------------------------------------------

template <typename E>
Bool Sequential<E>::add(E * entity) {
    SequentialNode<E> * newNode, * end, * beginNode;

    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));

    if (descriptor->readBeginPageId() == 0) {
        //create SequentialNode
        newNode = new SequentialNode<E>(se->create());
        // Circularly link
        newNode->writePreviousPageId(newNode->getPageId());
        newNode->writeNextPageId(newNode->getPageId());

        descriptor->writeBeginPageId(newNode->getPageId());
        descriptor->writeEndPageId(newNode->getPageId());
        //clean home
        delete newNode;
    }//endif
    end = new SequentialNode<E>(se->load(descriptor->readEndPageId()));

    //adding object in end node
    if (end->addEntity(entity) == false) {
        // node is full, creating new SEQUENTIALNODE
        newNode = new SequentialNode<E>(se->create());
        //adding object
        newNode->addEntity(entity);
        // Circularly link
        newNode->writeNextPageId(descriptor->readBeginPageId());
        newNode->writePreviousPageId(end->getPageId());
        end->writeNextPageId(newNode->getPageId());
        beginNode = new SequentialNode<E>(se->load(descriptor->readBeginPageId()));
        beginNode->writePreviousPageId(newNode->getPageId());
        //ajust new end node
        descriptor->writeEndPageId(newNode->getPageId());
        //clean home
        delete newNode;
        delete beginNode;
    }
    //clean home
    delete end;
    delete descriptor;
    se->close();
    delete se;
    return true;
}//add

//----------------------------------------------------------------------

template <typename E>
E* Sequential<E>::find(Uuid * uuid) {
    E* entity = NULL;
    SequentialNode<E> * actualSeqNode;
    Session* se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor* descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long actualPageId = descriptor->readBeginPageId();
    long firstPageId = actualPageId;

    if (actualPageId != 0) {
        do {
            actualSeqNode = new SequentialNode<E>(se->load(actualPageId));

            entity = actualSeqNode->findUuid(uuid);
            actualPageId = actualSeqNode->readNextPageId();
            //clean home
            delete actualSeqNode;
        } while ((entity == NULL) && (actualPageId != firstPageId));

    }
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return entity;
}//find

//----------------------------------------------------------------------

template <typename E>
Uuid* Sequential<E>::find(E * entity) {
    SequentialNode<E> * actualSeqNode;
    Uuid * uuid = NULL;
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long actualPageId = descriptor->readBeginPageId();
    long firstPageId = actualPageId;

    if (actualPageId != 0) {
        do {
            actualSeqNode = new SequentialNode<E>(se->load(actualPageId));

            uuid = actualSeqNode->findEntity(entity);
            actualPageId = actualSeqNode->readNextPageId();
            //clean home
            delete actualSeqNode;
        } while ((uuid == NULL) && (actualPageId != firstPageId));

    }
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return uuid;
}//find

//----------------------------------------------------------------------

template <typename E>
Long Sequential<E>::getRootPageId() {
    Session * se = this->getWorkspace()->openSession();
    long pageIdDescriptor = se->findPageIdDescriptor(SequentialDescriptor::nodeType);
    SequentialDescriptor * descriptor = new SequentialDescriptor(se->load(pageIdDescriptor));
    long rootPageId = descriptor->readBeginPageId();
    //clean home
    delete descriptor;
    se->close();
    delete se;
    return rootPageId;
}//getRootPageId

} /* storage */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_STORAGE_SEQUENTIAL_H */

