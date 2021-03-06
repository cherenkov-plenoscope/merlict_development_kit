// Copyright 2014 Sebastian A. Mueller
#include "merlict_portal_plenoscope/SimulationTruthHeader.h"
#include <sstream>
#include "merlict_corsika/corsika.h"
#include <cstring>

namespace plenoscope {

SimulationTruthHeader::SimulationTruthHeader() {
    for (unsigned int i = 0; i < raw.size(); i++) raw[i]=0.0;
    raw[  1-1] = corsika::str2float("MTST");
}

void SimulationTruthHeader::set_random_seed_of_run(const uint32_t seed) {
    float f;
    memcpy(&f, &seed, sizeof f);
    raw[  1-1] = f;
}

}  // namespace plenoscope
