
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
    this->numBlockPerSet = cache_settings.blocks;
    this->sets = cache_settings.sets;
    
    // get highest bit set for offset bit count 
    while (cache_settings.bytes >>= 1) {
        this->numOffsetBits++;
    }
    // get highest bit set for  index bit count
    while (cache_settings.blocks >>= 1) {
        this->numIndexBits++;
    }

    // initialize empty sets
    block emptyBlk;
    emptyBlk.is_empty = 0;
    emptyBlk.is_dirty = 0;
    // fill empty blocks into set
    set emptySet;
    emptySet.blocks.assign(numBlockPerSet, emptyBlk);
    // fill empty set into list of sets
    this->sets.assign(numSets, emptySet);

}

cacheAddress cache_sim::get_cache_addr(unsigned hex_addr) {
    cacheAddress cur;
    // Find the most sig of index 
    unsigned index_bits = (((1 << this->numIndexBits) - 1) << this->numOffsetBits);
    // shift back to the index location
    cur.index = (hex_addr & index_bits) >> this->numOffsetBists;
    // Determine the bits for tag
    unsigned tag_bits = ~((1 << (this->numIndexBits + this->numOffsetBits)) - 1);
    // find the values, then shift back to correct position
    cur.tag >> (raw_addr & tag_bits) >> (this->numIndexBits + this->numOffsetBits)
    return cur;
} 