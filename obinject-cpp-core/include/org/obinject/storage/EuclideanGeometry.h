/* 
 * File:   EuclideanGeometry.h
 * Author: system
 *
 * Created on 25 de Abril de 2014, 15:37
 */

#include <algorithm> // std::max
#include <org/obinject/meta/Entity.h>
#include <org/obinject/meta/Rectangle.h>
#include <org/obinject/device/all.h>
#include <org/obinject/storage/all.h>
#include <org/obinject/meta/all.h>
#include <org/obinject/storage/Structure.h>

using namespace std;

namespace org {
namespace obinject {
namespace storage {

template <typename K>
class EuclideanGeometry{
    private:
        
        long calculatedOverlap = 0;
        double precisionError = 0.00000001;
        
        K* newGenericType(){
            return new K();
        }
        
    public:
        
        K* intersection(K* rect1, K* rect2);
        
        K* rectUnion(K* rect1, K* rect2);
        
        Double occupancy(K* rect);
        
        Bool isOverlap(K* rectOverlap, K* rectOverlaped);
        
        void resetCalculatedOverlap();
        
        Long getCalculatedOverlap();
        
        
};

//long calculatedOverlap = 0;

template <typename K>
K* EuclideanGeometry<K>::intersection(K* rect1, K* rect2){
    Int dims = rect1->numberOfDimensions();
    Double * minPoint = new double[dims];
    Double * maxPoint = new double[dims];
    Double coord1;
    Double coord2;
    K * mbrIntersec = this->newGenericType();
    calculatedOverlap++;

    for(int i = 0; i < dims; i++){
        coord1 = rect1->getOrigin(i);
        coord2 = rect2->getOrigin(i);
        minPoint[i] = std::max(coord1, coord2);
        maxPoint[i] = std::min(coord1 + rect1->getExtension(i), coord2 + rect2->getExtension(i));
        mbrIntersec->setOrigin(i, minPoint[i]);
        mbrIntersec->setExtension(i, std::max(maxPoint[i] - minPoint[i] + this->precisionError, 0));
        if (mbrIntersec->getExtension(i) < 0){
            return NULL;
        }

    }
    //clean home
    delete [] minPoint;
    delete [] maxPoint;            
    return mbrIntersec;
}

template <typename K>
K* EuclideanGeometry<K>::rectUnion(K* rect1, K* rect2){ //Mudamos nome da funcao -> java = union (palavra reservada)
//    cout << "PASSEI AKEEEEE" << endl;
    Int dims = rect1->numberOfDimensions();
    Double * minPoint = new Double[dims];
    Double * maxPoint = new Double[dims];
    Double coord1;
    Double coord2;
    K * mbrUnion = this->newGenericType();
    calculatedOverlap++;

    for(int i = 0; i < dims; i++){
        coord1 = rect1->getOrigin(i);
        coord2 = rect2->getOrigin(i);
//        cout << "COORD1 ; COORD2 \t" << coord1 << " ; " << coord2 << endl;
        minPoint[i] = std::min(coord1, coord2);
        maxPoint[i] = std::max((coord1 + rect1->getExtension(i)), (coord2 + rect2->getExtension(i)));
        mbrUnion->setOrigin(i, minPoint[i]);
        mbrUnion->setExtension(i, maxPoint[i] - minPoint[i] + this->precisionError);
    }
    //clean home
    delete [] minPoint;
    delete [] maxPoint;            
    return mbrUnion;
}

template <typename K>
Double EuclideanGeometry<K>::occupancy(K* rect){
    Double ocup = 1;
    Int dims = rect->numberOfDimensions();

//    cout << "BEGIN\t";
    for (int i = 0; i < dims; i++){
//        cout << rect->getExtension(i) << "\t";
        ocup *= rect->getExtension(i);
    }
//    cout << "END" << endl;

    return ocup;
}

template <typename K>
Bool EuclideanGeometry<K>::isOverlap(K* rectOverlap, K* rectOverlaped){
    Int dims = rectOverlap->numberOfDimensions();
    calculatedOverlap++;

    for (int i = 0; i < dims; i++){
        if (rectOverlap->getOrigin(i) > rectOverlaped->getOrigin(i)) {
            return false;
        }
        if(rectOverlap->getOrigin(i) + rectOverlap->getExtension(i)
                < rectOverlaped->getOrigin(i) + rectOverlaped->getExtension(i)){
            return false;
        }
    }
    return true;
}

template <typename K>
Long EuclideanGeometry<K>::getCalculatedOverlap(){
    return calculatedOverlap;
}

template <typename K>
void EuclideanGeometry<K>::resetCalculatedOverlap(){
    calculatedOverlap = 0;
}

}
}
}