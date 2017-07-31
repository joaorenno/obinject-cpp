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
#include <org/obinject/device/Workspace.h>
#include <org/obinject/block/Node.h>
#include <org/obinject/block/HeaderNode.h>
#include <org/obinject/device/Memory.h>

#include <iostream>

using namespace std;

namespace org {
namespace obinject {
namespace device {

//----------------------------------------------------------------------

Memory::Memory(Int sizeArray) : AbstractWorkspace("Memory", sizeArray) {
    this->initialize();
}//Memory

//----------------------------------------------------------------------

Memory::~Memory() {
    map<Long, Byte*>::iterator it = nodes.begin();
    while(it != nodes.end()){
        //clean home
        delete [] (*it).second;
        //next
        it++;        
    }//endwhile
}//~Memory

//----------------------------------------------------------------------

Bool Memory::existWorkspace() {
    return false;
}//existWorkspace

//----------------------------------------------------------------------

void Memory::loadWorkspace() {
    //ok
}//loadWorkspace

//----------------------------------------------------------------------

void Memory::createWorkspace() {
    //ok
}//createWorkspace

//----------------------------------------------------------------------

Bool Memory::deletePage(Long id) {
    map<Long, Byte*>::iterator it = nodes.find(id);
    if (it == nodes.end()) {
        delete [] (it->second);
        nodes.erase(it);
        return true;
    } else {
        return false;
    }//endif
}//deletePage

//----------------------------------------------------------------------

Bool Memory::flushPage(Node *node) {
    nodes.insert(std::pair<Long, Byte*>(node->getPageId(), node->getArray()));
    delete node;
    return true;
}//flushPage

//----------------------------------------------------------------------

Node *Memory::loadPage(Long id) {
    Byte * array = nodes.at(id);
    return new Node(id, array, this->sizeArray);
}//loadPage

} /* device */
} /* obinject */
} /* org */