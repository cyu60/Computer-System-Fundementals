
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
    
    // get highest bit set for offset bit count 
    while (cache_settings.bytes >>= 1) {
        this->numOffsetBits++;
    }
    // get highest bit set for  index bit count
    while (cache_settings.blocks >>= 1) {
        this->numIndexBits++;
    }

    // initialize an empty set
    block empty_block;
    for 

}
