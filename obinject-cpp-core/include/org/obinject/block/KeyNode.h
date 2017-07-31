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
#ifndef ORG_OBINJECT_BLOCK_KEYNODE
#define	ORG_OBINJECT_BLOCK_KEYNODE

#include <org/obinject/meta/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/block/AbstractNode.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

template <typename K>
class KeyNode : public AbstractNode {
public:

    KeyNode(Node * node) : AbstractNode(node) {
        static_assert(is_base_of<Key<K>, K>::value, "Generic type is not derived of the Key class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    }

    virtual ~KeyNode() = 0;

    virtual void clear() = 0;

    virtual Int readNumberOfKeys() = 0;

};

//----------------------------------------------------------------------

template <typename K>
inline KeyNode<K>::~KeyNode() {
}

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_KEYNODE */

