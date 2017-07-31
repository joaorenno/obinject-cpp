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
#include <org/obinject/meta/DistanceUtil.h>
#include <string>
#include <cmath>
#include <cfloat>

using namespace std;

namespace org {
namespace obinject {
namespace meta {

Double DistanceUtil::damerau(string * string1, string * string2) {
    Double dist;
    Int i, j, cost, ins, upd, del, transp, greaterLen;
    Int sizeI = string1->size() + 1;
    Int sizeJ = string2->size() + 1;
//    unsigned char * str1 = new unsigned char[string1->size()];
//    unsigned char * str2 = new unsigned char[string2->size()];
    const char * str1 = string1->c_str();
    const char * str2 = string2->c_str();
    Int **align = new Int*[sizeI];
    for (i = 0; i < sizeI; ++i) align[i] = new Int[sizeJ];
//    for (i = 0; i < string1->size(); ++i) str1[i] = std::tolower(string1->at(i));
//    for (i = 0; i < string2->size(); ++i) str2[i] = std::tolower(string2->at(i));

    if (sizeI > sizeJ) {
        greaterLen = sizeI - 1;
    } else {
        greaterLen = sizeJ - 1;
    }
    align[0][0] = 0;
    for (i = 1; i < sizeI; i++) {
        align[i][0] = i;
    }//end for
    for (j = 1; j < sizeJ; j++) {
        align[0][j] = j;
    }//end for
    // Without transp
    for (j = 1; j < sizeJ; j++) {
        // Cost
        if (str1[0] == str2[j - 1]) {
            cost = 0;
        } else {
            cost = 1;
        }
        ins = align[0][j] + 1;
        del = align[1][j - 1] + 1;
        upd = align[0][j - 1] + cost;

        if ((ins < del) && (ins < upd)) {
            align[1][j] = ins;
        } else if (del < upd) {
            align[1][j] = del;
        } else {
            align[1][j] = upd;
        }
    }
    // With transp
    for (i = 2; i < sizeI; i++) {

        // Cost
        if (str1[i - 1] == str2[0]) {
            cost = 0;
        } else {
            cost = 1;
        }
        ins = align[i - 1][1] + 1;
        del = align[i][0] + 1;
        upd = align[i - 1][0] + cost;

        if ((ins < del) && (ins < upd)) {
            align[i][1] = ins;
        } else if (del < upd) {
            align[i][1] = del;
        } else {
            align[i][1] = upd;
        }

        for (j = 2; j < sizeJ; j++) {
            // Cost
            if (str1[i - 1] == str2[j - 1]) {
                cost = 0;
            } else {
                cost = 1;
            }
            ins = align[i - 1][j] + 1;
            del = align[i][j - 1] + 1;
            upd = align[i - 1][j - 1] + cost;
            transp = align[i - 2][j - 2] + cost;

            if ((ins < del) && (ins < upd) && (ins < transp)) {
                align[i][j] = ins;
            } else if ((del < upd) && (del < transp)) {
                align[i][j] = del;
            } else if (upd < transp) {
                align[i][j] = upd;
            } else {
                align[i][j] = transp;
            }
        }
    }
    dist = ((Double) align[sizeI - 1][sizeJ - 1] / greaterLen);
    //clean home
//    delete [] str1;
//    delete [] str2;
    for (i = 0; i < sizeI; ++i){
        delete [] align[i];
    }
    delete [] align;
    return dist;
}

template <typename K> Double DistanceUtil::geoPointDistanceDegree(Point<K> *point1, Point<K> *point2, Double rad) {
    Double lat1, lat2, lon1, lon2;
    Double pi = atan(1)*4;

    lat1 = point1->getOrigin(0) * pi / 180;
    lat2 = point2->getOrigin(0) * pi / 180;
    lon1 = point1->getOrigin(1) * pi / 180;
    lon2 = point2->getOrigin(1) * pi / 180;

    if (lat1 == lat2 && lon1 == lon2) {
        return 0;
    }

    return acos(
            (sin(lat1) * sin(lat2)
            + cos(lat1) * cos(lat2)) * cos(abs(lon2 - lon1))) * rad;
}

template <typename K> Double DistanceUtil::geoPointDistanceRadian(Point<K> *point1, Point<K> *point2, Double rad) {
    Double lat1, lat2, lon1, lon2;

    lat1 = point1->getOrigin(0);
    lat2 = point2->getOrigin(0);
    lon1 = point1->getOrigin(1);
    lon2 = point2->getOrigin(1);

    if (lat1 == lat2 && lon1 == lon2) {
        return 0;
    }

    return acos(
            (sin(lat1) * sin(lat2)
            + cos(lat1) * cos(lat2)) * cos(abs(lon2 - lon1))) * rad;
}

template <typename K> Double DistanceUtil::euclidean(Point<K> *point1, Point<K> *point2) {
    Int dims = point1->numberOfDimensions();
    Double dist = 0;

    for (Int i = 0; i < dims; i++) {
        dist += (point1->getOrigin(i) - point2->getOrigin(i)) * (point1->getOrigin(i) - point2->getOrigin(i));
    }

    return sqrt(dist);
}

template <typename K> Double DistanceUtil::euclidean(Rectangle<K> *rect1, Rectangle<K> *rect2) {
    Int dims = rect1->numberOfDimensions();
    Double dist = 0;
    Bool flagEquals = true;
    for (Int i = 0; i < dims; i++) {
        if (flagEquals) {
            if ((rect1->getOrigin(i) != rect2->getOrigin(i)) || (rect1->getExtension(i) != rect2->getExtension(i))) {
                flagEquals = false;
                dist += pow(
                        max(rect1->getOrigin(i) + rect1->getExtension(i), rect2->getOrigin(i) + rect2->getExtension(i))
                        - min(rect1->getOrigin(i), rect2->getOrigin(i)), 2);
            }
        } else {
            dist += pow(
                    max(rect1->getOrigin(i) + rect1->getExtension(i), rect2->getOrigin(i) + rect2->getExtension(i))
                    - min(rect1->getOrigin(i), rect2->getOrigin(i)), 2);
        }
    }

    if (flagEquals)
        return 0;
    return sqrt(dist);
}

Double DistanceUtil::levenshtein(string * string1, string * string2) {
    Double dist;
    Int i, j, cost, ins, upd, del, greaterLen;
    Int sizeI = string1->size() + 1;
    Int sizeJ = string2->size() + 1;
//    unsigned char * str1 = new unsigned char[string1.size()];
//    unsigned char * str2 = new unsigned char[string2.size()];
    const char * str1 = string1->c_str();
    const char * str2 = string2->c_str();
    Int **align = new Int*[sizeI];
    for (i = 0; i < sizeI; ++i) align[i] = new Int[sizeJ];
//    for (i = 0; i < string1.size(); ++i) str1[i] = std::tolower(string1.at(i));
//    for (i = 0; i < string2.size(); ++i) str2[i] = std::tolower(string2.at(i));

    if (sizeI > sizeJ) {
        greaterLen = sizeI - 1;
    } else {
        greaterLen = sizeJ - 1;
    }
    align[0][0] = 0;
    for (i = 1; i < sizeI; i++) {
        align[i][0] = i;
    }//end for
    for (j = 1; j < sizeJ; j++) {
        align[0][j] = j;
    }//end for
    for (i = 1; i < sizeI; i++) {
        for (j = 1; j < sizeJ; j++) {
            // Cost
            if (str1[i - 1] == str2[j - 1]) {
                cost = 0;
            } else {
                cost = 1;
            }
            ins = align[i - 1][j] + 1;
            del = align[i][j - 1] + 1;
            upd = align[i - 1][j - 1] + cost;

            if ((ins < del) && (ins < upd)) {
                align[i][j] = ins;
            } else if (del < upd) {
                align[i][j] = del;
            } else {
                align[i][j] = upd;
            }

        }
    }
    dist = ((Double) align[sizeI - 1][sizeJ - 1] / greaterLen);
    //clean home
//    delete [] str1;
//    delete [] str2;
    for (i = 0; i < sizeI; ++i){
        delete [] align[i];
    }
    delete [] align;
    return dist;
}

template <typename K> Double DistanceUtil::manhattan(Point<K> *point1, Point<K> *point2) {
    return minkowski(point1, point2, 1);
}

template <typename K> Double DistanceUtil::minkowski(Point<K> *point1, Point<K> *point2, Float p) {
    Int n = point1->numberOfDimensions();
    Double distance = 0;

    for (Int i = 0; i < n; i++) {
        distance += pow(abs(point1->getOrigin(i) - point2->getOrigin(i)), p);
    }

    return pow(distance, 1 / p);
}

Char DistanceUtil::amino[] = {
    'a', 'r', 'n', 'd', 'c', 'q', 'e', 'g', 'h', 'i', 'l', 'k', 'm', 'f', 'p', 's', 't', 'w', 'y', 'v'
}; // b x j o u z

Int DistanceUtil::aminoAcidToInt(Char c) {
    // TODO Sizeof estático
    for (Int i = 0; i < 20; i++) {
        if (c == amino[i]) {
            return i;
        }
    }

    return -1;
}
   
Char DistanceUtil::mpam[][20] = {
    {
        0, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 5, 4, 2
    },
    {
        2, 0, 2, 2, 4, 2, 2, 2, 2, 3, 3, 2, 2, 4, 2, 2, 2, 4, 4, 3
    },
    {
        2, 2, 0, 2, 4, 2, 2, 2, 2, 3, 3, 2, 2, 4, 2, 2, 2, 5, 4, 2
    },
    {
        2, 2, 2, 0, 4, 2, 2, 2, 2, 3, 3, 2, 3, 4, 2, 2, 2, 6, 4, 2
    },
    {
        3, 4, 4, 4, 0, 4, 4, 3, 4, 3, 4, 4, 4, 4, 3, 3, 3, 7, 3, 3
    },
    {
        2, 2, 2, 2, 4, 0, 2, 2, 2, 3, 3, 2, 2, 4, 2, 2, 2, 5, 4, 3
    },
    {
        2, 2, 2, 2, 4, 2, 0, 2, 2, 3, 3, 2, 3, 4, 2, 2, 2, 6, 4, 2
    },
    {
        2, 2, 2, 2, 3, 2, 2, 0, 2, 2, 3, 2, 2, 4, 2, 2, 2, 6, 4, 2
    },
    {
        2, 2, 2, 2, 4, 2, 2, 2, 0, 3, 3, 2, 3, 3, 2, 2, 2, 5, 3, 3
    },
    {
        2, 3, 3, 3, 3, 3, 3, 2, 3, 0, 1, 3, 2, 2, 2, 2, 2, 5, 3, 2
    },
    {
        2, 3, 3, 3, 4, 3, 3, 3, 3, 1, 0, 3, 1, 2, 3, 3, 2, 4, 2, 1
    },
    {
        2, 2, 2, 2, 4, 2, 2, 2, 2, 3, 3, 0, 2, 4, 2, 2, 2, 4, 4, 3
    },
    {
        2, 2, 2, 3, 4, 2, 3, 2, 3, 2, 1, 2, 0, 2, 2, 2, 2, 4, 3, 2
    },
    {
        3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 2, 4, 2, 0, 4, 3, 3, 3, 1, 2
    },
    {
        2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 3, 2, 2, 4, 0, 2, 2, 5, 4, 2
    },
    {
        2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 3, 2, 2, 3, 2, 0, 2, 5, 4, 2
    },
    {
        2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 0, 5, 3, 2
    },
    {
        5, 4, 5, 6, 7, 5, 6, 6, 5, 5, 4, 4, 4, 3, 5, 5, 5, 0, 4, 5
    },
    {
        4, 4, 4, 4, 3, 4, 4, 4, 3, 3, 2, 4, 3, 1, 4, 4, 3, 4, 0, 3
    },
    {
        2, 3, 2, 2, 3, 3, 2, 2, 3, 2, 1, 3, 2, 2, 2, 2, 2, 5, 3, 0
    }
};    

Double DistanceUtil::protein(string * string1, string * string2) {
    Double dist;
    Int i, j, idxI, idxJ, cost, ins, upd, del, transp, greaterLen;
    Int sizeI = string1->size() + 1;
    Int sizeJ = string2->size() + 1;
//    unsigned char * str1 = new unsigned char[string1.size()];
//    unsigned char * str2 = new unsigned char[string2.size()];
    const char * str1 = string1->c_str();
    const char * str2 = string2->c_str();
    Int **align = new Int*[sizeI];
    for (i = 0; i < sizeI; ++i) align[i] = new Int[sizeJ];
//    for (i = 0; i < string1.size(); ++i) str1[i] = std::tolower(string1.at(i));
//    for (i = 0; i < string2.size(); ++i) str2[i] = std::tolower(string2.at(i));

    if (sizeI > sizeJ) {
        greaterLen = sizeI - 1;
    } else {
        greaterLen = sizeJ - 1;
    }
    align[0][0] = 0;
    for (i = 1; i < sizeI; i++) {
        align[i][0] = i * 7;
    }//end for
    for (j = 1; j < sizeJ; j++) {
        align[0][j] = j * 7;
    }//end for

    // Without transp
    for (j = 1; j < sizeJ; j++) {
        // Cost			
        idxI = aminoAcidToInt((Char) str1[0]);
        idxJ = aminoAcidToInt((Char) str2[j - 1]);
        cost = mpam[idxI][idxJ];

        ins = align[0][j] + cost + 1;
        del = align[1][j - 1] + cost + 1;
        upd = align[0][j - 1] + cost;

        if ((ins < del) && (ins < upd)) {
            align[1][j] = ins;
        } else if (del < upd) {
            align[1][j] = del;
        } else {
            align[1][j] = upd;
        }
    }

    // With transp
    for (i = 2; i < sizeI; i++) {
        // Cost			
        idxI = aminoAcidToInt((Char) str1[i - 1]);
        idxJ = aminoAcidToInt((Char) str2[0]);
        cost = mpam[idxI][idxJ];

        ins = align[i - 1][1] + cost + 1;
        del = align[i][0] + cost + 1;
        upd = align[i - 1][0] + cost;

        if ((ins < del) && (ins < upd)) {
            align[i][1] = ins;
        } else if (del < upd) {
            align[i][1] = del;
        } else {
            align[i][1] = upd;
        }

        for (j = 2; j < sizeJ; j++) {
            // Cost				
            idxI = aminoAcidToInt((Char) str1[i - 1]);
            idxJ = aminoAcidToInt((Char) str2[j - 1]);
            cost = mpam[idxI][idxJ];

            ins = align[i - 1][j] + cost + 1;
            del = align[i][j - 1] + cost + 1;
            upd = align[i - 1][j - 1] + cost;
            transp = align[i - 2][j - 2] + cost;

            if ((ins < del) && (ins < upd) && (ins < transp)) {
                align[i][j] = ins;
            } else if ((del < upd) && (del < transp)) {
                align[i][j] = del;
            } else if (upd < transp) {
                align[i][j] = upd;
            } else {
                align[i][j] = transp;
            }
        }
    }
    dist = ((Double) align[sizeI - 1][sizeJ - 1] / greaterLen);
    //clean home
//    delete [] str1;
//    delete [] str2;
    for (i = 0; i < sizeI; ++i){
        delete [] align[i];
    }
    delete [] align;
    return dist;
}

Double veryLeastDistance(Float **matrix1, Float **matrix2) {
    //the very least function
    Double min = DBL_MAX;
    Double delta;
    Bool isEqual = true;
    Bool minIsEqual = false;

    for (int i = 0; i < sizeof (matrix1) / sizeof (Float); i++) // para cada descritor na matrix1
    {
        for (int j = 0; j < sizeof (matrix2) / sizeof (Float); j++) // para cada descritor na matrix2
        {
            delta = 0;
            for (int k = 0; k < sizeof (matrix1[i]) / sizeof (Float); k++) // para cada feature
            {
                isEqual = (isEqual && matrix1[i][k] == matrix2[j][k]);

                //					delta += Math.abs(matrix1[i][k] - matrix2[j][k]);
                delta += (matrix1[i][k] - matrix2[j][k]) * (matrix1[i][k] - matrix2[j][k]);

                if (delta > min) {
                    break;
                }
            }
            if (delta < min) {
                min = delta;
                minIsEqual = isEqual;
            }
        }
    }

    if (minIsEqual)
        return 0;
    return sqrt(min);
}

} /* meta */
} /* obinject */
} /* org */