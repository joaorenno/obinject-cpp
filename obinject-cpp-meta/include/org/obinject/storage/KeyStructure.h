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
#ifndef ORG_OBINJECT_STORAGE_KEYSTRUCTURE_H
#define	ORG_OBINJECT_STORAGE_KEYSTRUCTURE_H

#include <type_traits>
#include <org/obinject/meta/all.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/Structure.h>

using namespace std;

namespace org {
namespace obinject {
namespace storage {

template <typename K>
class KeyStructure : public Structure<K> {
public:

    KeyStructure() {
        static_assert(is_base_of<Key<K>, K>::value, "Generic type is not derived of the Key class");
        static_assert(is_convertible<K, Entity<K>*>::value, "Generic type is not derived of the Entity class");
    }

    virtual ~KeyStructure() = 0;

    Uuid *find(K *key);
};

template <typename K>
inline KeyStructure<K>::~KeyStructure() {
}

} /* storage */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_STORAGE_KEYSTRUCTURE_H */

