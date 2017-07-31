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
#include <org/obinject/device/File.h>
#include <org/obinject/device/AbstractWorkspace.h>
#include <org/obinject/device/Workspace.h>
#include <org/obinject/block/Node.h>
#include <org/obinject/block/HeaderNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace device {

//----------------------------------------------------------------------

File::File(string fileName, int sizeArray) : AbstractWorkspace(fileName, sizeArray) {
    this->initialize();
}

//----------------------------------------------------------------------

File::File(string fileName) : AbstractWorkspace(fileName) {
    this->initialize();
}

//----------------------------------------------------------------------

Bool File::existWorkspace() {
    fstream objectFile(this->getName().c_str());
    bool res = objectFile.good();
    objectFile.close();
    return res;
}

//----------------------------------------------------------------------

void File::loadWorkspace() {
    file.open(this->getName().c_str(), fstream::in | fstream::out);
    Node * node = this->loadPage(0);
    HeaderNode * headerNode = new HeaderNode(node);
    this->sizeArray = headerNode->readSizeOfArray();
    this->flushPage(headerNode);
    //clean home
    delete node;
}

//----------------------------------------------------------------------

void File::createWorkspace() {
    file.open(this->getName().c_str(), fstream::out);
    file.close();
    file.open(this->getName().c_str(), fstream::in | fstream::out);
}

//----------------------------------------------------------------------

Bool File::deletePage(Long id) {
    //remove in file
    //....
    return false;
}

//----------------------------------------------------------------------

Node* File::loadPage(Long id) {
    Byte* array = new Byte[this->sizeOfArray()];
    ULong pos = id * this->sizeOfArray();
    Node * node = NULL;

    if (file.seekg(pos).good()) {
        if (file.read((Char*) array, this->sizeOfArray()).good()) {
            node = new Node(id, array, this->sizeOfArray());
        }
    }
    return node;
}

//----------------------------------------------------------------------

Bool File::flushPage(Node* node) {
    ULong pos = node->getPageId() * this->sizeOfArray();

    if (file.seekp(pos).good()) {
        if (file.write((Char*) node->getArray(), this->sizeOfArray()).good()) {
            node->destroy();
            delete node;
            return true;
        }
    }
    //storing and clean home    
    node->destroy();
    delete node;
    return false;
}

} /* device */
} /* obinject */
} /* org */