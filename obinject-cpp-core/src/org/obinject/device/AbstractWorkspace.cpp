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
#include <org/obinject/device/AbstractWorkspace.h>
#include <org/obinject/block/HeaderNode.h>
#include <org/obinject/device/Session.h>
#include <string>

using namespace std;

namespace org {
namespace obinject {
namespace device {

//----------------------------------------------------------------------

AbstractWorkspace::AbstractWorkspace(string name) {
    this->sizeArray = 4096;
    this->name = name;
}//AbstractWorkspace

//----------------------------------------------------------------------

AbstractWorkspace::AbstractWorkspace(string name, Int sizeArray) {
    this->sizeArray = sizeArray;
    this->name = name;
}//AbstractWorkspace

//----------------------------------------------------------------------

void AbstractWorkspace::createHeaderNode() {
    Byte *array = new Byte[sizeArray];
    for (int i = 0; i<this->sizeArray; i++) {
        array[i] = 0;
    }
    Node * node = new Node(0, array, sizeArray);
    HeaderNode *headerNode = new HeaderNode(node);
    headerNode->writeSizeOfArray(sizeArray);
    headerNode->writeLastPageId(0);
    headerNode->writeLastSessionId(0);
    headerNode->writePreviousPageId(0);
    headerNode->writeNextPageId(0);
    //storing and clean home
    this->flushPage(headerNode);
    delete node;
}//createHeaderNode

//----------------------------------------------------------------------

void AbstractWorkspace::initialize() {
    if (existWorkspace()) {
        loadWorkspace();
    } else {
        createWorkspace();
        createHeaderNode();
    }
}//initialize

//----------------------------------------------------------------------

Long AbstractWorkspace::findUniqueDescriptor(Int nodeType) {
    Long pageId;
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode * headerNode = new HeaderNode(node);
    int idx = headerNode->indexOfDescriptorNodeType(nodeType);
    if (idx == -1) {
        pageId = newPageId(headerNode);
        headerNode->addEntry(nodeType, pageId);
    } else {
        pageId = headerNode->readDescriptorPageId(idx);
    }
    //clean home
    this->flushPage(headerNode);
    delete node;
    return pageId;
}//findUniqueDescriptor

//----------------------------------------------------------------------

const string AbstractWorkspace::getName() {
    return name;
}//getName

//----------------------------------------------------------------------

Long AbstractWorkspace::incrementPageId() {
    //adjusting LastPageId
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode * headerNode = new HeaderNode(node);
    long pageId = newPageId(headerNode);
    this->flushPage(headerNode);
    delete node;
    return pageId;
}//incrementPageId

//----------------------------------------------------------------------

Long AbstractWorkspace::incrementSessionId() {
    Node * node = this->loadPage(pageIdDescriptor);
    HeaderNode *headerNode = new HeaderNode(node);
    long inc = headerNode->readLastSessionId() + 1;
    headerNode->writeLastSessionId(inc);
    this->flushPage(headerNode);
    delete node;
    return inc;
}//incrementSessionId

//----------------------------------------------------------------------

Long AbstractWorkspace::newPageId(HeaderNode *header) {
    Long pageId = header->readLastPageId() + 1;
    header->writeLastPageId(pageId);
    //creating node
    Byte *array = new Byte[this->sizeOfArray()];
    for (int i = 0; i<this->sizeArray; i++) {
        array[i] = 0;
    }
    Node *node = new Node(pageId, array, this->sizeOfArray());
    this->flushPage(node);
    return pageId;
}//newPageId

//----------------------------------------------------------------------

Session *AbstractWorkspace::openSession() {
    return new Session(this);
}//openSession

//----------------------------------------------------------------------

Int AbstractWorkspace::sizeOfArray() {
    return sizeArray;
}//sizeOfArray

} /* device */
} /* obinject */
} /* org */