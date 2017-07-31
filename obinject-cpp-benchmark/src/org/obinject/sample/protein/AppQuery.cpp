
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
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <org/obinject/meta/Uuid.h>
#include <org/obinject/device/File.h>
#include <org/obinject/device/Memory.h>
#include <org/obinject/storage/Sequential.h>
#include <org/obinject/storage/BTree.h>
#include <org/obinject/storage/MTree.h>
#include <org/obinject/sample/protein/EntityProtein.h>
#include <org/obinject/sample/protein/PrimaryKeyProtein.h>
#include <org/obinject/sample/protein/EditionFirstProtein.h>

using namespace std;

#define maxElement 56519
#define maxSize 32

int main(int argc, char** argv) {
    fstream txtFile("prot.txt", fstream::in);
    string str;
    
    int blockSize = 4096;
    if (argc == 2) {
        istringstream ss(argv[1]);
        if (!(ss >> blockSize)) {
            blockSize = 4096;
        }
    }
    cout << "Using block size: " << blockSize << " bytes" << endl;
    
    //sequential
    //    File * f1 = new File("EntityProtein.dbo", blockSize);
    //    Sequential<EntityProtein> * seq = new Sequential<EntityProtein>(f1);
    //btree
    File * f2 = new File("PrimaryKeyProtein.btree", blockSize);
    //    Memory * f2 = new Memory(1024);
    BTree<PrimaryKeyProtein> * btree = new BTree<PrimaryKeyProtein>(f2);
    //mtree
    File * f3 = new File("EditionFirstProtein.mtree", blockSize);
    //    Memory * f3 = new Memory(1024);
    MTree<EditionFirstProtein> * mtree = new MTree<EditionFirstProtein>(f3);

    EntityProtein * entityProt = new EntityProtein();
    Uuid * uuidResult = NULL;
    EntityProtein * protResult = NULL;
    
    int mtreeErrors = 0, btreeErrors = 0;
    for (int i = 0; i < maxElement; i++) {
        txtFile >> str;
        if (i % 1000 == 0) {
            cerr << i << ", ";
        }
        entityProt->setAminoAcids(str);

        PrimaryKeyProtein * primaryProt = new PrimaryKeyProtein(entityProt);
        uuidResult = btree->find(primaryProt);
        if (uuidResult == NULL) {
            cerr << "[BTREE] protein not found [" << i << "]=" << primaryProt->getAminoAcids() << endl;
            btreeErrors++;
        } else {
            delete uuidResult;
        }
        //clean home
        delete primaryProt;

        EditionFirstProtein * editionProt = new EditionFirstProtein(entityProt);
        uuidResult = mtree->find(editionProt);
        if (uuidResult == NULL) {
            cerr << "[MTREE] protein not found [" << i << "]=" << editionProt->getAminoAcids() << endl;
            mtreeErrors++;
        } else {
            delete uuidResult;
        }
        //clean home
        delete editionProt;
    }

    cout << "FIND" << endl;
    cout << "[BTREE] Comparacoes: " << btree->getAverageForFind()->getTotalVerifications() << endl;
    cout << "[BTREE] Acesso: " << btree->getAverageForFind()->getTotalDiskAcess() << endl;
    cout << "[BTREE] Tempo: " << btree->getAverageForFind()->getTotalTime() << endl;
    cout << "[BTREE] Erros: " << btreeErrors << endl;

    cout << "[MTREE] Distancia: " << mtree->getAverageForFind()->getTotalVerifications() << endl;
    cout << "[MTREE] Acesso: " << mtree->getAverageForFind()->getTotalDiskAcess() << endl;
    cout << "[MTREE] Tempo: " << mtree->getAverageForFind()->getTotalTime() << endl;
    cout << "[MTREE] Erros: " << mtreeErrors << endl;

    //    delete seq;
    //    delete f1;
    delete btree;
    delete f2;
    delete mtree;
    delete f3;
    delete entityProt;
    txtFile.close();

    return 0;
}

