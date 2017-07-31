/* 
 * File:   CoordGeonet.h
 * Author: system
 *
 * Created on 13 de Maio de 2014, 19:16
 */

#ifndef COORDGEONET_H
#define	COORDGEONET_H

#include <limits>
#include <type_traits>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <stack>

namespace org {
namespace obinject {
namespace sample {
namespace geonet {

class CoordGeonet{

private:
    double latitude;
    double longitude;
    double width, height;
    
public:

    Double getHeight()
    {
        return height;
    }

    void setHeight(Double height)
    {
        this->height = height;
    }

    Double getWidth()
    {
        return width;
    }

    void setWidth(Double width)
    {
        this->width = width;
    }

    Double getLatitude()
    {
        return latitude;
    }

    Double getLongitude()
    {
        return longitude;
    }

    void setLatitude(Double latitude)
    {
        this->latitude = latitude;
    }

    void setLongitude(Double longitude)
    {
        this->longitude = longitude;
    }
};

}
}
}
}


#endif	/* COORDGEONET_H */

