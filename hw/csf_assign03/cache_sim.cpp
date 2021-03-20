//magarw13
//cyu60

#include "csim.h"
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include <bitset>


using std::bitset;
using std::cout;
using std::endl;

void print_bits(int num) {
    bitset<32> x(num);
    cout << x << endl;
}

cache_sim::cache_sim(cacheSettings cache_settings) {
    this->storeStrat = cache_settings.storeStrat;
    this->writeStrat = cache_settings.writeStrat;
    this->eviction = cache_settings.eviction;
    this->numBlockPerSet = cache_settings.blocks;
    this->numSets = cache_settings.sets;
    
    // reset 0
    this->numOffsetBits = 0;
    this->numIndexBits = 0;

    
    // get highest bit set for offset bit count 
    cout << "bytes: " << cache_settings.bytes << endl;
    while (cache_settings.bytes >>= 1) {
        this->numOffsetBits++;
        cout << "num offset bits: " << this->numOffsetBits << endl;
    }
    // get highest bit set for  index bit count
    cout << "blocks: " << cache_settings.blocks << endl;
    // while (cache_settings.blocks >>= 1) ++this->numIndexBits;
    while (cache_settings.blocks >>= 1) {
        this->numIndexBits++;
        cout << "num index bits: " << this->numIndexBits << endl;
    }
    cout << "num blocks: " << this->numBlockPerSet << endl;
    cout << "num sets: " << this->numSets << endl;

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
    cout << index_bits << endl;
    // find the values at those posistions, shift back to the index location
    cur.index = (hex_addr & index_bits) >> this->numOffsetBits;
    // Determine the bit positions for tag
    unsigned tag_bits = ~((1 << (this->numIndexBits + this->numOffsetBits)) - 1);
    // find the values at the position, then shift back to correct position
    cur.tag = (hex_addr & tag_bits) >> (this->numIndexBits + this->numOffsetBits);
    return cur;
} 

void cache_sim::print_cache() {
    for (unsigned i=0; i<this->numSets; i++) {
        for (unsigned j=0; j<this->numBlockPerSet; j++) {
            block curBlock = this->sets.at(i).blocks.at(j);
            cout << "Set: " << i << " Block: " << j << endl;
            
            cout << "dirty: " << curBlock.is_dirty << endl;
            cout << "empty: " << curBlock.is_empty << endl;
            cout << "tag: " << curBlock.cache_address.tag << endl;
            // cout << "index: " << curBlock.cache_address.index << endl;

        }
    }
} 


void cache_sim::process_ops(vector<traceLine> traces) {
    for (unsigned i = 0; i < traces.size(); i++) {
        // process the curAddress
        cacheAddress curAddress = this->get_cache_addr(traces.at(i).address);

        // raw add
        // cout << "raw add: " << traces.at(i).address << endl;
        cout << "raw add: ";
        print_bits(traces.at(i).address);

        // perform the correct operation
        switch (traces.at(i).operation)
        {
        case 'l':
            this->load(curAddress);
            break;
        case 's':
            this->save(curAddress);
            break;
        default: // throw exception? assumes well formed traces
            break;
        }
    }
    this->print_output();
}

void cache_sim::load(cacheAddress addr)
{
    // cout << "loading index: " << addr.index << " tag: " << addr.tag << endl;
    cout << "loading index: ";
    print_bits(addr.index);
    cout << "tag: ";
    print_bits(addr.tag);

    // Increase total loads
    this->cache_metrics.total_loads++;
    // NEED TO THINK ABOUT CYCLES???
    cache_metrics.total_cycles++;
    // Check if hit or miss
        // check the set
        set* cur_set = &this->sets.at(addr.index);
        // check all blocks -- direct mapping
        block* cur_block = &cur_set->blocks.at(0);
        if (cur_block->cache_address.tag == addr.tag) {
            // hit
            this->cache_metrics.load_hits++;
            return;
        } else {
            // miss
            this->cache_metrics.load_misses++;
            // Assume store immediately -- write back
            cur_block->cache_address.tag = addr.tag;

        }
        cout << "----------------" << endl;
        cout << "cur: " << cur_block->cache_address.tag << endl;
        cout << "----------------" << endl;
        this->print_cache();
        // for (unsigned i = 0; i < this->numBlockPerSet; i++) {
        //     block cur_block = cur_set.at(i);

        //     if (cur_block.cache_address.tag) {

        //     }
        // }




}
void cache_sim::save(cacheAddress addr)
{
    return;


}

void cache_sim::print_output() {
    cout << "Total loads: " << this->cache_metrics.total_loads << endl;
    cout << "Total stores: " << this->cache_metrics.total_stores << endl;
    cout << "Load hits: " << this->cache_metrics.load_hits << endl;
    cout << "Load misses: " << this->cache_metrics.load_misses << endl;
    cout << "Store hits: " << this->cache_metrics.store_hits << endl;
    cout << "Store misses: " << this->cache_metrics.store_misses << endl;
    cout << "Total cycles: " << this->cache_metrics.total_cycles << endl;
}
// std::pair<int, int> cache_sim::fetch_block(cacheAddress addr, char operation)
// {
//     // write-allocate we bring the relevant memory block into the cache before the store proceed
//     if (operation == "l") {
//         // Check to see if it exists in the cache
//         // if tag is not in cache
//         // Then add it to the cache

//         // otherwise it is a hit
//     }
// }