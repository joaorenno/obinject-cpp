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
#ifndef ORG_OBINJECT_BLOCK_HEADERNODE_H
#define	ORG_OBINJECT_BLOCK_HEADERNODE_H

#include <cstdlib>
#include <org/obinject/block/all.h>
#include <org/obinject/block/AbstractNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

class HeaderNode : public AbstractNode {
private:

    Int readDescriptorNodeType(Int idx) {
        int pos = HeaderNode::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry());
        return this->readInt(pos);
    }

    static Int sizeOfEntry() {
        return AbstractNode::sizeOfInt + AbstractNode::sizeOfLong;
    }

    static Int sizeOfFeatures() {
        return AbstractNode::sizeOfInt + AbstractNode::sizeOfLong + AbstractNode::sizeOfLong + AbstractNode::sizeOfInt;
    }

    void writeDescriptorNodeType(Int idx, Int nodeType) {
        int pos = HeaderNode::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry());
        this->writeInt(pos, nodeType);
    }

    void writeDescriptorPageId(Int idx, Long pageId) {
        int pos = HeaderNode::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry()) + HeaderNode::sizeOfInt;
        this->writeLong(pos, pageId);
    }

protected:

    Int freeSpace() {
        int num = this->readNumberOfEntries();
        if (num == 0) {
            return this->sizeOfArray() - HeaderNode::sizeOfHeader() - HeaderNode::sizeOfFeatures();
        } else {
            return this->sizeOfArray() - HeaderNode::sizeOfHeader()
                    - HeaderNode::sizeOfFeatures() - (num * HeaderNode::sizeOfEntry());
        }//endif
    }

    Int getNodeType() {
        return nodeType;
    }

    void incrementNumberOfKeys() {
        int num = this->readNumberOfEntries() + 1;
        int pos = HeaderNode::sizeOfHeader() + AbstractNode::sizeOfInt + AbstractNode::sizeOfLong + AbstractNode::sizeOfLong;
        this->writeInt(pos, num);
    }

public:
    static const Int nodeType = 1000;

    HeaderNode(Node *node) : AbstractNode(node) {
        this->initialize(node);
    }

    virtual ~HeaderNode() {
    }

    Bool addEntry(Int nodeType, Long pageId) {
        if (HeaderNode::sizeOfEntry() < this->freeSpace()) {
            int total = this->readNumberOfEntries();
            this->writeDescriptorNodeType(total, nodeType);
            this->writeDescriptorPageId(total, pageId);
            this->incrementNumberOfKeys();
            return true;
        } else {
            return false;
        }
    }

    /**
     * @see AbstractNode#clear()
     */
    void clear() {
        this->writeSizeOfArray(0);
        this->writeLastPageId(0);
        this->writeLastSessionId(0);
    }

    Int indexOfDescriptorNodeType(Int nodeType) {
        int total = this->readNumberOfEntries();

        for (int i = 0; i < total; i++) {
            if (nodeType == this->readDescriptorNodeType(i)) {
                return i;
            }
        }
        return -1;
    }

    Long readDescriptorPageId(Int idx) {
        int pos = HeaderNode::sizeOfHeader() + HeaderNode::sizeOfFeatures() + (idx * HeaderNode::sizeOfEntry()) + HeaderNode::sizeOfInt;
        return this->readLong(pos);
    }

    Int readNumberOfEntries() {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfInt + AbstractNode::sizeOfLong + AbstractNode::sizeOfLong;
        return this->readInt(pos);
    }

    Long readLastPageId() {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfInt;
        return this->readLong(pos);
    }

    Long readLastSessionId() {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfInt + AbstractNode::sizeOfLong;
        return this->readLong(pos);
    }

    Int readSizeOfArray() {
        int pos = AbstractNode::sizeOfHeader();
        return this->readInt(pos);
    }

    void writeLastPageId(Long last) {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfInt;
        this->writeLong(pos, last);
    }

    void writeLastSessionId(Long last) {
        int pos = AbstractNode::sizeOfHeader() + AbstractNode::sizeOfInt + AbstractNode::sizeOfLong;
        this->writeLong(pos, last);
    }

    void writeSizeOfArray(Int size) {
        int pos = AbstractNode::sizeOfHeader();
        this->writeInt(pos, size);
    }
};

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_HEADERNODE_H */

