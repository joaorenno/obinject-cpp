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
#ifndef ORG_OBINJECT_META_COMPARABLE_H
#define	ORG_OBINJECT_META_COMPARABLE_H

#include <string>
#include <type_traits>
#include <org/obinject/meta/all.h>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

template <typename T>
class Comparable {
public:

    Comparable() {
        static_assert(is_base_of<Comparable<T>, T>::value, "Generic type is not derived of the Comparable class");
    }

    template <typename U>
    operator Comparable<U>* () const {
        return NULL;
    }

    virtual ~Comparable() = 0;

    virtual Int compareTo(T *obj) = 0;

};

template <typename T>
inline Comparable<T>::~Comparable() {
}

} /* meta */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_META_COMPARABLE_H */

