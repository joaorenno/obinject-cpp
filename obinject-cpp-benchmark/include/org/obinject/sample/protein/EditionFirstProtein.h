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
#ifndef ORG_OBINJECT_SAMPLE_PROTEIN_EDITIONFIRSTPROTEIN_H
#define	ORG_OBINJECT_SAMPLE_PROTEIN_EDITIONFIRSTPROTEIN_H

#include <string>
#include <org/obinject/meta/DistanceUtil.h>
#include <org/obinject/sample/protein/EntityProtein.h>
#include <org/obinject/meta/Edition.h>

using namespace std;

namespace org {
namespace obinject {
namespace sample {
namespace protein {

class EditionFirstProtein : public EntityProtein, public Edition<EditionFirstProtein> {
private:
    double preservedDistance;
public:

    EditionFirstProtein():EntityProtein(){
        preservedDistance=0;        
    }
    
    EditionFirstProtein(Protein * obj):EntityProtein(obj) {
        preservedDistance=0;
    }
    
    virtual ~EditionFirstProtein(){
        
    }

    Double getPreservedDistance() {
        return this->preservedDistance;
    }

    string* getString() {
        return this->getAminoAcids();
    }

    Double distanceTo(EditionFirstProtein * obj) {
        return DistanceUtil::protein(this->getString(), obj->getString());
    }

    KeyStructure<EditionFirstProtein> * getKeyStructure() {
        return NULL;
    }

    Bool pullKey(Byte * array, int position) {
        PullStream pull(array, position);
        pull.pullString(this->getAminoAcids());
        return true;
    }

    void pushKey(Byte * array, int position) {
        PushStream push(array, position);
        push.pushString(this->getAminoAcids());
    }

    void setPreservedDistance(Double distance) {
        this->preservedDistance = distance;
    }

    Int sizeOfKey() {
        return Stream::sizeOfString(this->getAminoAcids());
    }


};

} /* protein */
} /* sample */
} /* obinject */
} /* org */

using namespace org::obinject::sample::protein;

#endif	/* ORG_OBINJECT_SAMPLE_PROTEIN_EDITIONFIRSTPROTEIN_H */

