/* 
 * File:   RectLatLongCoordGeonet.h
 * Author: system
 *
 * Created on 13 de Maio de 2014, 20:07
 */

#ifndef RECTLATLONGCOORDGEONET_H
#define	RECTLATLONGCOORDGEONET_H

#include <iomanip>
#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <stack>
#include <math.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/storage/AbstractStructure.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/meta/Key.h>
#include <org/obinject/storage/KeyStructure.h>
#include <org/obinject/device/File.h>
#include <org/obinject/meta/PullStream.h>
#include <org/obinject/meta/PushStream.h>
#include <org/obinject/meta/Rectangle.h>
#include <org/obinject/meta/Stream.h>
#include <org/obinject/storage/RTree.h>
#include <org/obinject/meta/Uuid.h>
#include <org/obinject/meta/Order.h>
#include <org/obinject/meta/Comparable.h>
#include <org/obinject/sample/geonet/EntityCoordGeonet.h>

using namespace std;

namespace org {
namespace obinject {
namespace sample {
namespace geonet {
    
//class RectLatLongCoordGeonet: public EntityCoordGeonet, Rectangle<RectLatLongCoordGeonet>, Comparable<RectLatLongCoordGeonet> {
class RectLatLongCoordGeonet: public EntityCoordGeonet, public Rectangle<RectLatLongCoordGeonet> {
private:
    
public:
    
    static const RTree<RectLatLongCoordGeonet> * keyStructure;

    RectLatLongCoordGeonet()
    {
    }

    RectLatLongCoordGeonet(CoordGeonet * coord) : EntityCoordGeonet(coord)
    {        
    }

    virtual ~RectLatLongCoordGeonet(){
        
    }
    
    Double distanceTo(RectLatLongCoordGeonet * metric)
    {
        return sqrt(
                pow(this->getLatitude() - metric->getLatitude(), 2)
                + pow(this->getLongitude() - metric->getLongitude(), 2));
    }

    Double getOrigin(Int idx)
    {
        if (idx == 0)
        {
            return this->getLatitude();
        } else
        {
            return this->getLongitude();
        }
    }
    
    Int bla()
    {
//        cout << "TO AKI MEMU" << endl;
        return 10;
    }

    Int numberOfDimensions()
    {
        return 2;
    }

    bool pullKey(Byte * array, Int position)
    {
        PullStream pull (array, position);
        this->setLatitude(pull.pullDouble());
        this->setLongitude(pull.pullDouble());
        this->setWidth(pull.pullDouble());
        this->setHeight(pull.pullDouble());
        return true;
    }

    void pushKey(Byte * array, Int position)
    {
        PushStream push (array, position);
        push.pushDouble(this->getLatitude());
        push.pushDouble(this->getLongitude());
        push.pushDouble(this->getWidth());
        push.pushDouble(this->getHeight());
    }

    void setOrigin(Int idx, Double value)
    {
        if (idx == 0)
        {
            this->setLatitude(value);
        } else
        {
            this->setLongitude(value);
        }
    }

    Int sizeOfKey()
    {
        return Stream::sizeOfDouble * 4;
    }

    Double getExtension(Int axis)
    {
        if (axis == 0)
        {
//            cout.setf(ios::fixed);
//            cout << std::setprecision(17) << "getWidth() - " << getWidth() << endl;
            return this->getWidth();
        } else
        {
            return this->getHeight();
        }
    }

    void setExtension(Int axis, Double value)
    {
        if (axis == 0)
        {
            this->setWidth(value);
        } else
        {
            this->setHeight(value);
        }
    }

    KeyStructure<RectLatLongCoordGeonet> *getKeyStructure() {
        return NULL;
    }

    Double getPreservedDistance() {
        return 0;
    }

    void setPreservedDistance(Double distance) {
        
    }

    Int compareTo(RectLatLongCoordGeonet * obj) {
        return 0;
    }
    
    string toString() {
//        cout << endl;
//        cout << "MAHOEE\t" << getLatitude() << " \t " << getLongitude() << endl;
        return to_string(getLatitude()) + "\t\t" + to_string(getLongitude());
    }
};
const RTree<RectLatLongCoordGeonet> * RectLatLongCoordGeonet::keyStructure = new RTree<RectLatLongCoordGeonet>(new File("rectlatlongcity.rtree", 1024));

}
}
}
}

#endif	/* RECTLATLONGCOORDGEONET_H */

