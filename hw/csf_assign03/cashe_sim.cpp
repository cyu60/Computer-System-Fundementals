
#include "csim.h"

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

cache_sim::cache_sim(cacheSettings cache_settings) {
    this->storeStrat = cache_settings.storeStrat;
    this->writeStrat = cache_settings.writeStrat;
    this->eviction = cache_settings.eviction;

    this
}
