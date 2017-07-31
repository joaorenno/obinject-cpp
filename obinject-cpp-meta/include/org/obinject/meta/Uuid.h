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
#ifndef ORG_OBINJECT_META_UUID_H
#define	ORG_OBINJECT_META_UUID_H

#include <string>
#include <org/obinject/meta/all.h>
#include <time.h>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

class Uuid {
private:
    Long mostSigBits;
    Long leastSigBits;
    static string digits(ULong val, Int digits);

public:

    Uuid();
    
    Uuid(Long mostSigBits, Long leastSigBits) {
        assign(mostSigBits, leastSigBits);
    }
    
    Uuid(string value){
        fromString(value);
    }
    
    virtual ~Uuid() {
    }
    
    void assign(Long mostSigBits, Long leastSigBits) {
        this->mostSigBits = mostSigBits;
        this->leastSigBits = leastSigBits;
    }

    Int compareTo(Uuid* uuid);

    bool equals(Uuid* uuid) {
        return (this->mostSigBits == uuid->mostSigBits)
                && (this->leastSigBits == uuid->leastSigBits);
    }

    void fromString(string value);

    Long getLeastSignificantBits() {
        return this->leastSigBits;
    }

    Long getMostSignificantBits() {
        return this->mostSigBits;
    }

    string toString();
};

} /* meta */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_META_UUID_H */

