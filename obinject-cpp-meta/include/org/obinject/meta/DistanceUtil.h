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
#ifndef ORG_OBINJECT_META_DISTANCEUTIL_H
#define	ORG_OBINJECT_META_DISTANCEUTIL_H

#include <string>
#include <cmath>
#include <cfloat>
#include <org/obinject/meta/all.h>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

class DistanceUtil {
private:
    static Char amino[20];    
    static Char mpam[20][20];

    static Int aminoAcidToInt(Char c);

    template <typename K>
    static Double minkowski(Point<K> *point1, Point<K> *point2, Float p);

    DistanceUtil() {
    }

public:
    static Double damerau(string * str1, string * str2);

    static Double levenshtein(string * str1, string * str2);

    template <typename K>
    static Double manhattan(Point<K> *point1, Point<K> *point2);

    static Double protein(string * str1, string * str2);

    template <typename K>
    static Double geoPointDistanceDegree(Point<K> *point1, Point<K> *point2, Double rad);

    template <typename K>
    static Double geoPointDistanceRadian(Point<K> *point1, Point<K> *point2, Double rad);

    template <typename K>
    static Double euclidean(Point<K> *point1, Point<K> *point2);

    template <typename K>
    static Double euclidean(Rectangle<K> *rect1, Rectangle<K> *rect2);

    static Double veryLeastDistance(Float **matrix1, Float **matrix2);

    virtual ~DistanceUtil() {
    }

};

} /* meta */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_META_DISTANCEUTIL_H */

