/* 
 * File:   EntityCoordGeonet.h
 * Author: system
 *
 * Created on 13 de Maio de 2014, 19:19
 */

#ifndef ENTITYCOORDGEONET_H
#define	ENTITYCOORDGEONET_H

#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <stack>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Key.h>
#include <org/obinject/device/File.h>
#include <org/obinject/meta/PullStream.h>
#include <org/obinject/meta/PushStream.h>
#include <org/obinject/meta/Rectangle.h>
#include <org/obinject/meta/Stream.h>
#include <org/obinject/storage/AbstractKeyStructure.h>
#include <org/obinject/storage/AbstractEntityStructure.h>
#include <org/obinject/storage/RTree.h>
#include <org/obinject/meta/Uuid.h>

#include "CoordGeonet.h"

using namespace std;
using namespace std::chrono;
 
namespace org {
namespace obinject {
namespace sample {
namespace geonet {

class EntityCoordGeonet : public CoordGeonet, public Entity<EntityCoordGeonet>{
private: 
    Uuid * uuid;
    
public:    
    
    static Uuid * classId;

    EntityCoordGeonet(){
       uuid = new Uuid();
    }

    EntityCoordGeonet(CoordGeonet * coord)
    {
        this->setLatitude(coord->getLatitude());
        this->setLongitude(coord->getLongitude());
        this->setWidth(coord->getWidth());
        this->setHeight(coord->getHeight());
    }
    
    virtual ~EntityCoordGeonet(){
//        delete uuid;
    }


    Bool isEqual(EntityCoordGeonet * object)
    {
        return this->getLatitude() == object->getLatitude()
                && this->getLongitude() == object->getLongitude();
    }


    Uuid* getUuid()
    {
        return uuid;
    }


    Bool pullEntity(Byte * array, int position)
    {
        PullStream pull (array, position);
        Uuid * storedClass = new Uuid(0, 0);
        pull.pullUuid(storedClass);
        if (classId->equals(storedClass) == true)
        {
            pull.pullUuid(uuid);
            this->setLatitude(pull.pullDouble());
            this->setLongitude(pull.pullDouble());
            //clean home
            delete storedClass;
            return true;
        }
        //clean home
        delete storedClass;
        return false;
    }


    void pushEntity(Byte * array, int position)
    {
        PushStream push (array, position);
        push.pushUuid(classId);
        push.pushUuid(uuid);
        push.pushDouble(this->getLatitude());
        push.pushDouble(this->getLongitude());
    }


    int sizeOfEntity()
    {
        return Stream::sizeOfUuid
                + Stream::sizeOfUuid
                + Stream::sizeOfDouble
                + Stream::sizeOfDouble;
    }


    bool inject() {
        return false;
    }
    


    EntityStructure<EntityCoordGeonet> *getEntityStructure() {
        return NULL;
    }
    
};
Uuid * EntityCoordGeonet::classId = new Uuid("22CF318D-9CD7-C47C-D062-D1CD32AE732D");

}
}
}
}

#endif	/* ENTITYCOORDGEONET_H */

