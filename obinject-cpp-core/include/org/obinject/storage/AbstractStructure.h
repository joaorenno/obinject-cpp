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
#ifndef ORG_OBINJECT_STORAGE_ABSTRACTSTRUCTURE_H
#define	ORG_OBINJECT_STORAGE_ABSTRACTSTRUCTURE_H

#include <org/obinject/device/all.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/Structure.h>

using namespace std;

namespace org {
namespace obinject {
namespace storage {

template <typename T>
class AbstractStructure : public Structure<T> {
private:
    Workspace * workspace;
public:

    AbstractStructure(Workspace * workspace) {
        this->workspace = workspace;
    }

    virtual ~AbstractStructure() = 0;

    Workspace * getWorkspace() {
        return workspace;
    }

    virtual Bool add(T *obj) = 0;
    virtual Long getRootPageId() = 0;
    virtual Bool remove(T *obj) = 0;
};

//----------------------------------------------------------------------

template <typename T>
inline AbstractStructure<T>::~AbstractStructure() {
}

} /* meta */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_STORAGE_ABSTRACTSTRUCTURE_H */

