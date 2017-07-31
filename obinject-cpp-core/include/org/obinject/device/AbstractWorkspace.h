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
#ifndef ORG_OBINJECT_DEVICE_ABSTRACTWORKSPACE_H
#define	ORG_OBINJECT_DEVICE_ABSTRACTWORKSPACE_H

#include <string>
#include <org/obinject/device/all.h>
#include <org/obinject/block/all.h>
#include <org/obinject/device/Workspace.h>

using namespace std;

namespace org {
namespace obinject {
namespace device {

class AbstractWorkspace : public Workspace {
private:

    static const Long pageIdDescriptor = 0;

    string name;

    Long newPageId(HeaderNode *header);

protected:

    AbstractWorkspace(string name);
    void initialize();
    virtual Bool existWorkspace() = 0;
    virtual void loadWorkspace() = 0;
    virtual void createWorkspace() = 0;
    void createHeaderNode();
    Int sizeArray;

public:

    AbstractWorkspace(string name, Int sizeArray);

    virtual ~AbstractWorkspace() = 0;

    virtual Bool deletePage(Long id) = 0;

    Long findUniqueDescriptor(Int nodeType);

    virtual Bool flushPage(Node *node) = 0;

    const string getName();

    Long incrementPageId();

    Long incrementSessionId();

    virtual Node *loadPage(Long id) = 0;

    Session *openSession();

    Int sizeOfArray();
};

//----------------------------------------------------------------------

inline AbstractWorkspace::~AbstractWorkspace() {
}

} /* device */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_DEVICE_ABSTRACTWORKSPACE_H */

