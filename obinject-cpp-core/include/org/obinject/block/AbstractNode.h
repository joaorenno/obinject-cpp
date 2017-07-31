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
#ifndef ORG_OBINJECT_BLOCK_ABSTRACTNODE_H
#define	ORG_OBINJECT_BLOCK_ABSTRACTNODE_H

#include <org/obinject/block/all.h>
#include <org/obinject/block/Node.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

class AbstractNode : public Node {
public:

    AbstractNode(Node *node) : Node(node->getPageId(), node->getArray(), node->getSize()) {
    }

    virtual ~AbstractNode() = 0;

    virtual void clear() = 0;

protected:

    virtual Int getNodeType() = 0;

    void initialize(Node *node) {
        Int type = node->readNodeType();

        // zero indica que não foi definido o tipo do nó
        if (type == 0) {
            this->writeNodeType(this->getNodeType());
        } else if (type != this->getNodeType()) {
            // TODO exceptionize-it
            // error match type node
            throw new exception();
        }
    }

};

//----------------------------------------------------------------------

inline AbstractNode::~AbstractNode() {
}

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_ABSTRACTNODE_H */

