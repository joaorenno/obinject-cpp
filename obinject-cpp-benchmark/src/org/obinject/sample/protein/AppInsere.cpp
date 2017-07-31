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

/*
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

#define maxElement 1001

int main(int argc, char** argv) {
    fstream txtFile("prot.txt", fstream::in);
    string strAmino;
    string strUuid;

    //btree
    remove("PrimaryKeyProtein.btree");
    File * f2 = new File("PrimaryKeyProtein.btree", 4096);
    //    Memory * f2 = new Memory(1024);
    BTree<PrimaryKeyProtein> * btree = new BTree<PrimaryKeyProtein>(f2);
    //mtree
    remove("EditionFirstProtein.mtree");
    File * f3 = new File("EditionFirstProtein.mtree", 4096);
    //    Memory * f3 = new Memory(1024);
    MTree<EditionFirstProtein> * mtree = new MTree<EditionFirstProtein>(f3);

    //objects
    Protein * protein = new Protein();
    EntityProtein * entityProt;
    PrimaryKeyProtein * primaryProt;
    EditionFirstProtein * editionProt;
    Uuid * uuidProt = new Uuid();
    for (int i = 0; i < maxElement; i++) {
        if (i % 1000 == 0) {
            cerr << i << ", ";
        }
        txtFile >> strAmino;
        protein->setAminoAcids(&strAmino);
        txtFile >> strUuid;
        uuidProt->fromString(strUuid);
        
        entityProt = new EntityProtein(protein, uuidProt);
        //add btree                
        primaryProt = new PrimaryKeyProtein(entityProt);
        btree->add(primaryProt);
        //clean home
        delete primaryProt;
        //add mtree
        editionProt = new EditionFirstProtein(entityProt);
        mtree->add(editionProt);
        //clean home
        delete editionProt;
        delete entityProt;
    }

    cout << "ADD" << endl;
    cout << "[BTREE] Comparacoes: " << btree->getAverageForAdd()->getTotalVerifications() << endl;
    cout << "[BTREE] Acesso: " << btree->getAverageForAdd()->getTotalDiskAcess() << endl;
    cout << "[BTREE] Tempo: " << btree->getAverageForAdd()->getTotalTime() << endl << endl;
    //cout << "Tamanho Árvore [BTree]: " << btree->height() << endl;

    cout << "[MTREE] Distancia: " << mtree->getAverageForAdd()->getTotalVerifications() << endl;
    cout << "[MTREE] Acesso: " << mtree->getAverageForAdd()->getTotalDiskAcess() << endl;
    cout << "[MTREE] Tempo: " << mtree->getAverageForAdd()->getTotalTime() << endl;
    //cout << "Tamanho Árvore [MTree]: " << mtree->height() << endl;

    //close txt
    txtFile.close();
    //clean home
    delete btree;
    delete f2;
    delete mtree;
    delete f3;
    

    return 0;
}

*/