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
#include <org/obinject/meta/Uuid.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

static Bool UUID_GENERATOR_RANDOMIZE = true;

//----------------------------------------------------------------------
Uuid::Uuid() {
    if (UUID_GENERATOR_RANDOMIZE == false) {
        UUID_GENERATOR_RANDOMIZE = true;
        srand(time(NULL));
    }
    mostSigBits = ((ULong) rand()) << 32;
    mostSigBits |= ((ULong) rand());
    leastSigBits = ((ULong) rand()) << 32;
    leastSigBits += ((ULong) rand());
}

//----------------------------------------------------------------------
string Uuid::digits(ULong val, Int digits) {
    stringstream ss;
    ULong hi = ((ULong) 1) << (digits * 4);
    ss << setfill('0') << setw(digits + 1) << hex << (hi | (val & (hi - 1)));
    return ss.str().substr(1, digits);
}

//----------------------------------------------------------------------
Int Uuid::compareTo(Uuid* uuid) {
    if (this->mostSigBits == uuid->mostSigBits) {
        if (this->leastSigBits > uuid->leastSigBits)
            return 1;
        else if (this->leastSigBits < uuid->leastSigBits)
            return -1;
        else return 0;
    } else if (this->mostSigBits > uuid->mostSigBits) return 1;
    else return -1;
}

//----------------------------------------------------------------------
void Uuid::fromString(string value) {
    UInt val_comp[6];
    //split the components of the UUID   
    sscanf(value.c_str(), "%8x-%4x-%4x-%4x-%8x%4x",
            &val_comp[0], &val_comp[1], &val_comp[2], &val_comp[3], &val_comp[4], &val_comp[5]);
    //Most Significant Bits
    this->mostSigBits = val_comp[0];
    this->mostSigBits <<= 16;
    this->mostSigBits |= val_comp[1];
    this->mostSigBits <<= 16;
    this->mostSigBits |= val_comp[2];
    //Least Significant Bits
    this->leastSigBits = val_comp[3];
    this->leastSigBits <<= 32;
    this->leastSigBits |= val_comp[4];
    this->leastSigBits <<= 16;
    this->leastSigBits |= val_comp[5];
}

//----------------------------------------------------------------------
string Uuid::toString() {
    ULong most = (ULong)this->mostSigBits;
    ULong least = (ULong)this->leastSigBits;
    return ( digits((most >> 32), 8) + "-"
            + digits((most >> 16), 4) + "-"
            + digits((most), 4) + "-"
            + digits((least >> 48), 4) + "-"
            + digits((least >> 16), 8)
            + digits((least), 4));
}

} /* meta */
} /* obinject */
} /* org */