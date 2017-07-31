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
// valgrind --leak-check=full ./obinject-cpp-benchmark
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <org/obinject/meta/Uuid.h>
#include <org/obinject/device/File.h>
#include <org/obinject/device/Memory.h>
#include <org/obinject/storage/Sequential.h>
#include <org/obinject/storage/RTree.h>
#include <org/obinject/sample/geonet/RectLatLongCoordGeonet.h>
#include <org/obinject/sample/geonet/CoordGeonet.h>
#include <org/obinject/sample/geonet/EntityCoordGeonet.h>
#include <org/obinject/block/Node.h>

using namespace std;
using namespace org::obinject::sample::geonet;

static int sizeOfNode = 104*5;
static int lines = 50;    //16 lines com 104*5 sizeOfNode = pau
static int findAdd = 0;

int main(int argc, char** argv) {
//    for(int k=0; k<1000; k++){
    if(argc >= 2) lines = atoi(argv[1]);
    if(argc >= 3) sizeOfNode = atoi(argv[2]);
    if(argc >= 4) findAdd = (argv[3][0] == 'f' ? 1 : 2);
    cout.setf(ios::fixed);
    
    RectLatLongCoordGeonet* metric = new RectLatLongCoordGeonet();
    string datafile = "rtreeGeonet-" + to_string(sizeOfNode) + ".dat";

    File* workspace = new File(datafile, sizeOfNode);     
    RTree<RectLatLongCoordGeonet>* rtree = new RTree<RectLatLongCoordGeonet>(workspace);
    
    fstream txtFile("base.txt", fstream::in);
    
    struct timeval start;
    struct timeval end2;
    long mtime = 0, seconds = 0, useconds = 0;

    char lat[30];
    char lon[30];
    int count = lines;
    while(txtFile >> lat >> lon)
    {
        if(count == 1) rtree->setTest(true);
        if(findAdd == 1 || count-- == 0) break;
//        cout << lat << "\t" << lon << endl;
        metric->setOrigin(0,atof(lat));
        metric->setOrigin(1,atof(lon));
        
        gettimeofday(&start, NULL);
        rtree->add(metric);
        gettimeofday(&end2, NULL);

        seconds  += end2.tv_sec  - start.tv_sec;
        useconds += end2.tv_usec - start.tv_usec;
        
//        rtree->showAll();
//        cout << endl << endl;
    }
    //rtree->showAll();
    
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    
//    cout << metric->toString() << endl;
    
    txtFile.close();
    if(findAdd != 1)
    {
        cout << endl;
        cout << "tamanho da paginacao:" << workspace->sizeOfArray() << endl;
        printf("Elapsed Time Insert: %ld milliseconds\n", mtime);
        cout << "calculos de distancia:" << rtree->getAverageForAdd()->measuredVerifications() << endl;
        cout << "acesso a blocos:" << rtree->getAverageForAdd()->measuredDiskAccess() << endl;
    }
    
//    
    RTreeIndex<RectLatLongCoordGeonet>::resetNumMbrs();
    
    fstream check("base.txt", fstream::in);
    int notFound = 0;
    
    mtime = 0;
    seconds = 0;
    useconds = 0;
    
    count = lines;
    while (check >> lat >> lon)
    {
        if(findAdd == 2 || count-- == 0) break;
        metric->setOrigin(0,atof(lat));
        metric->setOrigin(1,atof(lon));
////        cout << metric->toString() << endl;
        
        gettimeofday(&start, NULL);
        Uuid* uuidFind = rtree->find(metric);
        gettimeofday(&end2, NULL);
        
        seconds  += end2.tv_sec  - start.tv_sec;
        useconds += end2.tv_usec - start.tv_usec;
        
        if (uuidFind == NULL)
        {
            notFound++;
        }
        delete uuidFind;
    }
    
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    
    check.close();
    if(findAdd != 2)
    {
        cout << endl;
        cout << "nao encontrados:" << notFound << " objetos" << endl;
        printf("Elapsed Time Find: %ld milliseconds\n", mtime);
        cout << "calculos de distancia:" << rtree->getAverageForFind()->measuredVerifications() << endl;
        cout << "acesso a blocos:" << rtree->getAverageForFind()->measuredDiskAccess() << endl;   
    }
    
    delete metric;
    delete workspace;
    delete rtree;
//    }
    
    return 0;
}

