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
#ifndef ORG_OBINJECT_BLOCK_BTREEDESCRIPTOR
#define	ORG_OBINJECT_BLOCK_BTREEDESCRIPTOR

#include <org/obinject/block/all.h>
#include <org/obinject/block/DescriptorNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

class BTreeDescriptor : public DescriptorNode {
protected:

    Int getNodeType() {
        return nodeType;
    }//getNodeType

public:
    static const Int nodeType = 1004;

    BTreeDescriptor(Node * node) : DescriptorNode(node) {
        initialize(node);
    }//BTreeDescriptor

    virtual ~BTreeDescriptor() {
    }

    void clear() {
        writePreviousPageId(0);
        writeNextPageId(0);
        writeRootPageId(0);
    }//clear

    Long readRootPageId() {
        int pos = AbstractNode::sizeOfHeader();
        return this->readLong(pos);
    }//readRootPageId

    Int readTreeHeight() {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfLong;
        return this->readInt(pos);
    }//readTreeHeight

    void writeRootPageId(Long pageId) {
        int pos = AbstractNode::sizeOfHeader();
        this->writeLong(pos, pageId);
    }//writeRootPageId

    void writeTreeHeight(Int height) {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfLong;
        this->writeInt(pos, height);
    }//writeTreeHeight

    void incTreeHeight() {
        this->writeTreeHeight(this->readTreeHeight() + 1);
    }

};

//----------------------------------------------------------------------

} /* block */
} /* obinject */
} /* org */

#endif	/*ORG_OBINJECT_BLOCK_BTREEDESCRIPTOR */

