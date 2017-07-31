/* 
 * File:   PrimaryKeyProtein.h
 * Author: enzo
 *
 * Created on 30 de Novembro de 2013, 15:06
 */

#ifndef ORG_OBINJECT_SAMPLE_PROTEIN_PRIMARYKEYPROTEIN_H
#define	ORG_OBINJECT_SAMPLE_PROTEIN_PRIMARYKEYPROTEIN_H

#include <org/obinject/meta/all.h>
#include <org/obinject/sample/protein/EntityProtein.h>
#include <org/obinject/meta/Order.h>
#include <org/obinject/meta/Comparable.h>
#include <org/obinject/meta/PullStream.h>
#include <org/obinject/meta/PushStream.h>
#include <org/obinject/storage/AbstractKeyStructure.h>

using namespace std;

namespace org {
namespace obinject {
namespace sample {
namespace protein {  

class PrimaryKeyProtein: public EntityProtein, Comparable<PrimaryKeyProtein>, Order<PrimaryKeyProtein> {
public:
    PrimaryKeyProtein():EntityProtein() {
    }//PrimaryKeyProtein
    
    PrimaryKeyProtein(EntityProtein * obj):EntityProtein(obj) {
    }//PrimaryKeyProtein
    
    virtual ~PrimaryKeyProtein(){
        
    }

    Int compareTo(PrimaryKeyProtein * obj) {
        return this->getAminoAcids()->compare(obj->getAminoAcids()->c_str());
    }
    
    KeyStructure<PrimaryKeyProtein> * getKeyStructure() {
        return NULL;
    }
    
     Bool pullKey(Byte* array, Int position) {
        PullStream pull(array, position);
        pull.pullString(this->getAminoAcids());
        return true;
    }

    void pushKey(Byte * array, Int position) {
        PushStream push(array, position);
        push.pushString(this->getAminoAcids());
    }

    Int sizeOfKey() {
        return Stream::sizeOfUuid + Stream::sizeOfUuid + Stream::sizeOfString(this->getAminoAcids());
    }
    
};

} /* protein */
} /* sample */
} /* obinject */
} /* org */

using namespace org::obinject::sample::protein;

#endif	/* ORG_OBINJECT_SAMPLE_PROTEIN_PRIMARYKEYPROTEIN_H */

