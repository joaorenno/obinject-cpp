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
#ifndef ORG_OBINJECT_BLOCK_ALL_H
#define	ORG_OBINJECT_BLOCK_ALL_H

#include <org/obinject/types.h>

namespace org {
namespace obinject {
namespace block {

class AbstractNode;
class BTreeDescriptor;
template <typename K> class BTreeIndex;
template <typename K> class BTreeLeaf;
template <typename K> class BTreeNode;
class DescriptorNode;
template <typename E> class EntityNode;
class HeaderNode;
template <typename K> class KeyNode;
class Node;
class SequentialDescriptor;
template <typename E> class SequentialNode;

} /* block */
} /* obinject */
} /* org */

using namespace org::obinject::block;

#endif	/* ORG_OBINJECT_BLOCK_ALL_H */

