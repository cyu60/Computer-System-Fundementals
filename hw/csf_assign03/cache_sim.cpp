#include "csim.h"

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using std::cout;
using std::endl;

cache_sim::cache_sim(cacheSettings cache_settings) {
    this->storeStrat = cache_settings.storeStrat;
    this->writeStrat = cache_settings.writeStrat;
    this->eviction = cache_settings.eviction;
    this->numBlockPerSet = cache_settings.blocks;
    this->numSets = cache_settings.sets;
    
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
    // Find the bit positions of index 
    unsigned index_bits = (((1 << this->numIndexBits) - 1) << this->numOffsetBits);
    // find the values at those posistions, shift back to the index location
    cur.index = (hex_addr & index_bits) >> this->numOffsetBits;
    // Determine the bit positions for tag
    unsigned tag_bits = ~((1 << (this->numIndexBits + this->numOffsetBits)) - 1);
    // find the values at the position, then shift back to correct position
    cur.tag >> (hex_addr & tag_bits) >> (this->numIndexBits + this->numOffsetBits);
    return cur;
} 

void cache_sim::print_cache() {
    for (unsigned i=0; i<this->numSets; i++) {
        for (unsigned j=0; j<this->numBlockPerSet; j++) {
            block curBlock = this->sets.at(i).blocks.at(j);
            cout << i << " " << j << endl;
            
            cout << "dirty: " << curBlock.is_dirty << endl;
            cout << "empty: " << curBlock.is_empty << endl;
            cout << "tag: " << curBlock.cache_address.tag << endl;
            cout << "index: " << curBlock.cache_address.index << endl;

        }
    }
} 

void cache_sim::process_ops(vector<traceLine> traces) {
    for (unsigned i = 0; i < traces.size(); i++) {
        // process the curAddress
        cacheAddress curAddress = this->get_cache_addr(traces.at(i).address);
        // perform the correct operation
        switch (traces.at(i).operation)
        {
        case "l":
            this->load_data(curAddress);
            break;
        case "s":
            this->save_data(curAddress);
            break;
        default: // throw exception? assumes well formed traces
            break;
        }
    }
}