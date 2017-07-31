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
#ifndef ORG_OBINJECT_TYPES_H
#define	ORG_OBINJECT_TYPES_H

#include <climits>

namespace org {
namespace obinject {

typedef bool Bool;
typedef char Byte;
typedef char Char;
typedef double Double;
typedef float Float;
typedef int Int;
typedef short Short;
typedef unsigned int UInt;
typedef unsigned short UShort;

#if ULONG_MAX == 0xFFFFFFFF
//architecture 32 bits
#if defined(__GNUC__)
typedef long long Long;
typedef unsigned long long ULong;
#elif defined(_MSC_VER)
#define __int64 Long;
#define __uint64 ULong;
#else
#define __int64 Long;
#define __uint64 ULong;
#endif
#else
//architecture 64 bits
#if defined(__GNUC__)
typedef long Long;
typedef unsigned long ULong;
#elif defined(_MSC_VER)
#define __int64 Long;
#define __uint64 ULong;
#else
#define __int64 Long;
#define __uint64 ULong;
#endif
#endif

} /* obinject */
} /* org */

using namespace org::obinject;

#endif	/* ORG_OBINJECT_TYPES_H */