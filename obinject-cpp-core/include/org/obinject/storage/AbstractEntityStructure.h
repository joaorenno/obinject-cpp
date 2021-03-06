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
#ifndef ORG_OBINJECT_STORAGE_ABSTRACTENTITYSTRUCTURE_H
#define	ORG_OBINJECT_STORAGE_ABSTRACTENTITYSTRUCTURE_H

#include <type_traits>
#include <org/obinject/device/all.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/storage/EntityStructure.h>
#include <org/obinject/meta/all.h>

using namespace std;

namespace org {
namespace obinject {
namespace storage {

template <typename E>
class AbstractEntityStructure : public AbstractStructure<E>, EntityStructure<E> {
public:

    AbstractEntityStructure(Workspace * workspace) : AbstractStructure<E>(workspace) {
        static_assert(is_base_of<Entity<E>, E>::value, "Generic type is not derived of the Entity class");
    }

    virtual ~AbstractEntityStructure() = 0;

    virtual E *find(Uuid *uuid) = 0;
    virtual Bool add(E *obj) = 0;
    virtual Long getRootPageId() = 0;
    virtual Bool remove(E *obj) = 0;

};

//----------------------------------------------------------------------

template <typename E>
inline AbstractEntityStructure<E>::~AbstractEntityStructure() {
}

} /* storage */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_STORAGE_ABSTRACTENTITYSTRUCTURE_H */

