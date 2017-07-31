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
#ifndef ORG_OBINJECT_BLOCK_NODE_H
#define	ORG_OBINJECT_BLOCK_NODE_H

#include <cstdlib>
#include <org/obinject/block/all.h>
#include <org/obinject/meta/Uuid.h>

using namespace std;

namespace org {
namespace obinject {
namespace block {

class Node {
private:

    Byte *array;
    Int size;
    Long pageId;

    void moveLeft(Int dest, Int source, Int length) {
        for (int i = 0; i < length; i++) {
            array[dest + i] = array[source + i];
        }
    }

    void moveRight(Int dest, Int source, Int length) {
        for (int i = 0; i < length; i++) {
            array[dest + length - i - 1] = array[source + length - i - 1];
        }
    }

protected:

    Bool readBoolean(Int pos) {
        return (array[pos] == 1);
    }

    Byte readByte(Int pos) {
        return array[pos];
    }

    Double readDouble(Int pos) {
        Double *buff = (Double *) (array + pos);
        return buff[0];
    }

    Float readFloat(Int pos) {
        Float *buff = (Float *) (array + pos);
        return buff[0];
    }

    Int readInt(Int pos) {
        Int *buff = (Int *) (array + pos);
        return buff[0];
    }

    Long readLong(Int pos) {        
        Long *buff = (Long *) (array + pos);
        return buff[0];
    }

    Short readShort(Int pos) {
        Short * buff = (Short *) (array + pos);
        return buff[0];
    }

    Uuid *readUuid(Int pos) {
        Long most = this->readLong(pos);
        Long least = this->readLong(pos + sizeOfLong);
        return new Uuid(most, least);
    }

    void writeNodeType(Int nodeType) {
        this->writeInt(0, nodeType);
    }

    void writeBoolean(Int pos, bool value) {
        array[pos] = value ? (Byte) 1 : (Byte) 0;
    }

    void writeByte(Int pos, Byte value) {
        array[pos] = value;
    }

    void writeDouble(Int pos, double value) {
        double * buff = (double *) (array + pos);
        buff[0] = value;
    }

    void writeFloat(Int pos, Float value) {
        Float * buff = (Float *) (array + pos);
        buff[0] = value;
    }

    void writeInt(Int pos, Int value) {
        Int * buff = (Int *) (array + pos);
        buff[0] = value;
    }

    void writeLong(Int pos, Long value) {
        Long * buff = (Long *) (array + pos);
        buff[0] = value;
    }

    void writeShort(Int pos, Int value) {
        Short * buff = (Short *) (array + pos);
        buff[0] = value;
    }

    void writeUuid(Int pos, Uuid *value) {
        writeLong(pos, value->getMostSignificantBits());
        writeLong(pos + sizeOfLong, value->getLeastSignificantBits());
    }

public:
    static const Int bitsPerByte = 8;
    static const Int mask = 0xff;
    static const Int sizeOfByte = 1;
    static const Int sizeOfDouble = 8;
    static const Int sizeOfFloat = 4;
    static const Int sizeOfInt = 4;
    static const Int sizeOfLong = 8;
    static const Int sizeOfShort = 2;
    static const Int sizeOfUuid = 16;

    Node(Long Id, Byte *buff, Int size) {
        this->size = size;
        this->pageId = Id;
        array = buff;
    }

    virtual ~Node() {
    }

    void destroy() {
        if (array != NULL) {
            delete [] array;
        }
    }

    Byte* getArray() {
        return array;
    }

    Int getSize() {
        return size;
    }

    Long readNextPageId() {
        Int pos = sizeOfInt + sizeOfLong;
        return readLong(pos);
    }

    Int readNodeType() {
        return readInt(0);
    }

    Long getPageId() {
        return this->pageId;
    }

    Long readPreviousPageId() {
        Int pos = sizeOfInt;
        return readLong(pos);
    }

    void move(Int dest, Int source, Int length) {
        if (dest > source) {
            this->moveRight(dest, source, length);
        } else {
            this->moveLeft(dest, source, length);
        }
    }

    void writeNextPageId(Long pageId) {
        Int pos = sizeOfInt + sizeOfLong;
        this->writeLong(pos, pageId);
    }

    void writePreviousPageId(Long pageId) {
        Int pos = sizeOfInt;
        this->writeLong(pos, pageId);
    }

    Int sizeOfArray() {
        return this->size;
    }

    static Int sizeOfHeader() {
        // node type + previous page Id + next page Id
        return sizeOfInt + sizeOfLong + sizeOfLong;
    }
};

} /* block */
} /* obinject */
} /* org */

#endif	/* ORG_OBINJECT_BLOCK_NODE_H */

