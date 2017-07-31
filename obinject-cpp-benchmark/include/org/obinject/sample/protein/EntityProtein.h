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
#ifndef ORG_OBINJECT_SAMPLE_PROTEIN_ENTITYPROTEIN_H
#define	ORG_OBINJECT_SAMPLE_PROTEIN_ENTITYPROTEIN_H

#include <org/obinject/sample/protein/Protein.h>
#include <org/obinject/meta/Entity.h>
#include <org/obinject/meta/PullStream.h>
#include <org/obinject/meta/PushStream.h>

using namespace std;

namespace org {
namespace obinject {
namespace sample {
namespace protein {
    
class EntityProtein : public Protein, public Entity<EntityProtein> {
private:
    Uuid uuid;
public:
    static const Uuid classId;
    
    EntityProtein(){
    }

    EntityProtein(Protein * protein){
        this->setAminoAcids(protein->getAminoAcids());
    }

    EntityProtein(Protein * protein, Uuid * uuid){
        this->setAminoAcids(protein->getAminoAcids());
        this->uuid.assign(uuid->getMostSignificantBits(), uuid->getLeastSignificantBits());
    }
    
    EntityProtein(EntityProtein * entity){
        this->setAminoAcids(entity->getAminoAcids());
        this->uuid.assign(entity->getUuid()->getMostSignificantBits(), 
                entity->getUuid()->getLeastSignificantBits());
    }

    virtual ~EntityProtein(){
    }

    Bool isEqual(EntityProtein *entity) {
        return (this->getAminoAcids()->compare(entity->getAminoAcids()->c_str()) == 0);
    }

    Uuid *getUuid() {
        return &uuid;
    }

    Bool inject() {
        return false;
    }

    Bool pullEntity(Byte array[], Int position) {
        PullStream pull(array, position);
        Uuid storedClass;
        pull.pullUuid(&storedClass);
        if (((Uuid *)&classId)->equals(&storedClass) == true)
        {
            pull.pullUuid(&uuid);
            pull.pullString(this->getAminoAcids());
            return true;
        }
        return false;        
    }

    void pushEntity(Byte array[], Int position) {
        PushStream push(array, position);
        push.pushUuid((Uuid *)&classId);
        push.pushUuid(&uuid);
        push.pushString(this->getAminoAcids());
    }

    Int sizeOfEntity() {
        return 2 * Stream::sizeOfUuid
                + Stream::sizeOfString(this->getAminoAcids());
    }

    EntityStructure<EntityProtein> *getEntityStructure() {
        return NULL;
    }

};

const Uuid EntityProtein::classId = Uuid("5151145E-6155-4E28-88A0-A227207F2C02");

using namespace org::obinject::sample::protein;

} /* protein */
} /* sample */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_SAMPLE_PROTEIN_ENTITYPROTEIN_H */

