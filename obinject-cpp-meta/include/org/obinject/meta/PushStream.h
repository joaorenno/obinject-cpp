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
#ifndef ORG_OBINJECT_META_PUSHSTREAM_H
#define	ORG_OBINJECT_META_PUSHSTREAM_H

#include <list>
#include <org/obinject/types.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Stream.h>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

class PushStream : public Stream {
public:

    PushStream(Byte array[], Int position) : Stream::Stream(array, position) {
    }

    virtual ~PushStream() {
    }

    void pushBoolean(Bool value);

    void pushByte(Byte value);

    void pushChar(Char value);

    //void pushCollection(list<Entity*> collection);

    void pushDouble(Double value);

    void pushFloat(Float value);

    void pushInt(Int value);

    void pushLong(Long value);

    void pushShort(Short value);

    void pushString(string *value);

    void pushUuid(Uuid *value);
};

} /* meta */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_META_PUSHSTREAM_H */

