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

#ifndef ORG_OBINJECT_QUERIES_PERFORMANCEMEASUREMENT_H
#define	ORG_OBINJECT_QUERIES_PERFORMANCEMEASUREMENT_H

#include <org/obinject/queries/all.h>

namespace org {
namespace obinject {
namespace queries {

class PerformanceMeasurement {
public:
    virtual void incrementMeasurement() = 0;
    
    virtual void incrementDiskAccess(long diskAccess) = 0;
    
    virtual void incrementVerification(long verifications) = 0;
    
    virtual void incrementTime(long time) = 0;

    virtual Long amountOfMeasurements() = 0;

    virtual Long getTotalDiskAcess() = 0;

    virtual Long getTotalTime() = 0;

    virtual Long getTotalVerifications() = 0;

    virtual double measuredDiskAccess() = 0;

    virtual double measuredTime() = 0;

    virtual double measuredVerifications() = 0;

    virtual void resetMeasurements() = 0;

};

} /* queries */
} /* obinject */
} /* org */

using namespace org::obinject::queries;


#endif	/* ORG_OBINJECT_QUERIES_PERFORMANCEMEASUREMENT_H */

