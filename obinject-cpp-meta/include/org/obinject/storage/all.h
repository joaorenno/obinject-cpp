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
#ifndef ORG_OBINJECT_STORAGE_ALL_H
#define	ORG_OBINJECT_STORAGE_ALL_H

#include <org/obinject/types.h>

namespace org {
namespace obinject {
namespace storage {

template <typename E> class AbstractEntityStructure;
template <typename K> class AbstractKeyStructure;
template <typename T> class AbstractStructure;
template <typename K> class EuclideanGeometry;
template <typename K> class BTree;
template <typename E> class EntityStructure;
template <typename K> class KeyStructure;
template <typename T> class Structure;
template <typename E> class Sequential;


} /* storage */
} /* obinject */
} /* org */

using namespace org::obinject::storage;

#endif	/* ORG_OBINJECT_STORAGE_ALL_H */

