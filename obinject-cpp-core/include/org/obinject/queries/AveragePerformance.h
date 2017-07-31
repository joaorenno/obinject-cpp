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

#ifndef ORG_OBINJECT_QUERIES_AVERAGEPERFORMANCE_H
#define	ORG_OBINJECT_QUERIES_AVERAGEPERFORMANCE_H

#include <org/obinject/queries/all.h>
#include <org/obinject/queries/PerformanceMeasurement.h>

namespace org {
namespace obinject {
namespace queries {

class AveragePerformance : public PerformanceMeasurement {
private:
    Long measurements;
    Long totalDiskAcess;
    Long totalTime;
    Long totalVerifications;

public:

    AveragePerformance() {
        resetMeasurements();
    }

    Long amountOfMeasurements() {
        return this->measurements;
    }

    Long getTotalDiskAcess() {
        return this->totalDiskAcess;
    }

    Long getTotalTime() {
        return this->totalTime;
    }

    Long getTotalVerifications() {
        return this->totalVerifications;
    }

    Double measuredDiskAccess() {
        if (this->measurements == 0) {
            return 0;
        }
        return ((Double)this->totalDiskAcess) / ((Double)this->measurements);
    }

    Double measuredTime() {
        if (this->measurements == 0) {
            return 0;
        }
        return ((Double)this->totalTime) / ((Double)this->measurements);
    }

    Double measuredVerifications() {
        if (this->measurements == 0) {
            return 0;
        }
        return ((Double)this->totalVerifications) / ((Double)this->measurements);
    }

    void resetMeasurements() {
        this->measurements = 0;
        this->totalDiskAcess = 0;
        this->totalTime = 0;
        this->totalVerifications = 0;
    }
    
    void incrementMeasurement() {
        measurements++;
    }

    void incrementDiskAccess(long diskAccess) {
        totalDiskAcess += diskAccess;
    }

    void incrementVerification(long verifications) {
        totalVerifications += verifications;
    }

    void incrementTime(long time) {
        totalTime += time;
    }

};

} /* queries */
} /* obinject */
} /* org */

using namespace org::obinject::queries;

#endif	/* ORG_OBINJECT_QUERIES_AVERAGEPERFORMANCE_H */

