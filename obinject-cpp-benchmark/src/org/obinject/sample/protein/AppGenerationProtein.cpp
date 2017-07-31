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

#define maxElement 100000
#define maxSize 32

int main(int argc, char** argv) {
    Char amino[20] = {
        'a', 'r', 'n', 'd', 'c', 'q', 'e', 'g', 'h', 'i', 'l', 'k', 'm', 'f', 'p', 's', 't', 'w', 'y', 'v'
    }; // b x j o u z
    int size;
    char * vect;
    Uuid * uuid;
    fstream txtFile("prot.txt", fstream::out);
    for (int i = 0; i < maxElement; i++) {
        size = (rand() % (maxSize - 10)) + 10;
        vect = new char[size + 1];
        for (int i = 0; i < size; i++) {
            vect[i] = amino[rand() % 20];
        }
        vect[size] = '\0';
        uuid = Uuid::generator();
        txtFile << vect << '\t' << uuid->toString() << endl;
        //clean home
        delete uuid;
        delete vect;
    }
    txtFile.close();
 }