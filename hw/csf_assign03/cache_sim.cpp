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

void print_bits(int num, unsigned len=32) {
    bitset<32> x(num);
    // bitset<len> x(num);
    cout << x << endl;
}
// (this->blockSize/4) * 100 + 1????

cache_sim::cache_sim(cacheSettings cache_settings) {
    this->storeStrat = cache_settings.storeStrat;
    this->writeStrat = cache_settings.writeStrat;
    this->eviction = cache_settings.eviction;
    this->numBlockPerSet = cache_settings.blocks;
    this->numSets = cache_settings.sets;
    this->blockSize = cache_settings.bytes;

    // reset 0
    this->numOffsetBits = 0;
    this->numIndexBits = 0;

    
    // get highest bit set for offset bit count 
    while (cache_settings.bytes >>= 1) {
        this->numOffsetBits++;
    }
    
    // get highest bit set for  index bit count
    // cout << "blocks: " << cache_settings.blocks << endl;
    // while (cache_settings.blocks >>= 1) ++this->numIndexBits;
    while (cache_settings.sets >>= 1) {
        this->numIndexBits++;
    }
    // cout << "num blocks: " << this->numBlockPerSet << endl;

    // initialize empty sets
    block emptyBlk;
    emptyBlk.is_empty = 0;
    emptyBlk.is_dirty = 0;
    // fill empty blocks into set
    set emptySet;
    emptySet.blocks.assign(numBlockPerSet, emptyBlk);
    emptySet.tracker = 0; // initialize tracker to be 0
    // fill empty set into list of sets
    this->sets.assign(numSets, emptySet);

}

cacheAddress cache_sim::get_cache_addr(unsigned hex_addr) {
    cacheAddress cur;
    // Find the bit positions of index 
    unsigned index_bits = (((1 << this->numIndexBits) - 1) << this->numOffsetBits); // 00001110000
    // cout << index_bits << endl;
    // find the values at those posistions, shift back to the index location
    cur.index = (hex_addr & index_bits) >> this->numOffsetBits; // 00010100110000 --> 00010100000 --> 000101
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
            cout << "Set: ";
            print_bits(i, this->numIndexBits); 
            cout << " Block: " << j << endl;
            
            cout << "dirty: " << curBlock.is_dirty << endl;
            cout << "empty: " << curBlock.is_empty << endl;
            cout << "tag: ";
            print_bits(curBlock.cache_address.tag, 32-numIndexBits-numOffsetBits);
            // cout << "tag: " << curBlock.cache_address.tag << endl;
            // cout << "index: " << curBlock.cache_address.index << endl;

        }
    }
} 


void cache_sim::process_ops(vector<traceLine> traces) {
    cache_metrics.load_hits = 0;
    cache_metrics.load_misses = 0;
    cache_metrics.total_loads = 0;
    cache_metrics.store_hits = 0;
    cache_metrics.store_misses = 0;
    cache_metrics.total_stores = 0;
    cache_metrics.total_cycles = 0;
    for (unsigned i = 0; i < traces.size(); i++) {
        // process the curAddress
        cacheAddress curAddress = this->get_cache_addr(traces.at(i).address);

        // raw add
        // cout << "raw add: " << traces.at(i).address << endl;

        // perform the correct operation
        if (traces.at(i).operation == 'l') {
            cache_metrics.total_loads++;
            // if (this->numSets >= 1 && this->numBlockPerSet == 1) {
            //     this->directLoad(curAddress);
            // } else if (this->numSets > 1 && this->numBlockPerSet > 1) {
            //     this->associativeLoad(curAddress);
            // } else if (this->numSets == 1 && this->numBlockPerSet > 1) {
            //     this->fullyAssLoad(curAddress);
            // }
            this->load(curAddress);
        } else if (traces.at(i).operation == 's') {
            cache_metrics.total_stores++;
            // if (this->numSets >= 1 && this->numBlockPerSet == 1) {
            //     this->directSave(curAddress);
            // } else if (this->numSets > 1 && this->numBlockPerSet > 1) {
            //     this->associativeSave(curAddress);
            // } else if (this->numSets == 1 && this->numBlockPerSet > 1) {
            //     this->fullyAssSave(curAddress);
            // }
            this->save(curAddress);
        } else {
            printf("Invalid input");
            exit(1);
        }
    }
    this->print_output();
}

// void cache_sim::directLoad(cacheAddress addr) {
//     set* cur_set = &this->sets.at(addr.index);
//     block* cur_block;
//     cur_block = &cur_set->blocks.at(0);
//     if (cur_block->cache_address.tag == addr.tag) {
//         // hit
//         this->cache_metrics.load_hits++;
//         this->cache_metrics.total_cycles++;
//     } else {
//         this->cache_metrics.load_misses++;
//         this->cache_metrics.total_cycles += ((this->blockSize/4 * 100) + 1);
//         cur_block->cache_address.tag = addr.tag;
//     }
//     // Update LRU
//     // if (this->eviction == LRU) {
//     //     update_lru(cur_set, 0);
//     // }
// }

// void cache_sim::associativeLoad(cacheAddress addr) {

// }
// void cache_sim::fullyAssLoad(cacheAddress addr) {

// }

// void cache_sim::directSave(cacheAddress addr) {
//     set* cur_set = &this->sets.at(addr.index);
//     block* cur_block;
//     cur_block = &cur_set->blocks.at(0);
//     if (cur_block->cache_address.tag == addr.tag) {
//         // hit
//         this->cache_metrics.store_hits++;
//         if (!this->writeStrat) {
//             this->cache_metrics.total_cycles += ((this->blockSize/4 * 100) + 1);
//         } else {
//             this->cache_metrics.total_cycles++;
//             cur_block->is_dirty = 1;
//         }
//     } else {
//         this->cache_metrics.store_misses++;
//         this->cache_metrics.total_cycles += ((this->blockSize/4 * 100) + 1);
//         cur_block->cache_address.tag = addr.tag;
        
//     }
// }

// void cache_sim::associativeSave(cacheAddress addr) {

// }
// void cache_sim::fullyAssSave(cacheAddress addr) {

// }

void cache_sim::load(cacheAddress addr)
{
    // cout << "loading index: " << addr.index << " tag: " << addr.tag << endl;
    // cout << "loading index: ";
    // print_bits(addr.index);
    // cout << "tag: ";
    // print_bits(addr.tag);

    // Increase total loads
    // NEED TO THINK ABOUT CYCLES???
    
    // Check if hit or miss
    // check the set
    set* cur_set = &this->sets.at(addr.index);
    
    // check all blocks 
    block* cur_block;
    for (unsigned i = 0; i < this->numBlockPerSet; i++) {
        cur_block = &cur_set->blocks.at(i);
        if (cur_block->cache_address.tag == addr.tag) {
            // hit
            this->cache_metrics.load_hits++;
            this->cache_metrics.total_cycles++;
            //Update LRU
            if (this->eviction == LRU) {
                update_lru(cur_set, i);
            }
            return;
        }
    }
    // miss
    this->cache_metrics.load_misses++;
    // 4 bytes (32 mem address) are transfered to cache
    this->cache_metrics.total_cycles+=(this->blockSize/4) * 100; 
    cur_block->cache_address.tag = addr.tag;
        if (this->eviction == FIFO) {
        cur_set->blocks.erase(cur_set->blocks.begin());
        cur_set->blocks.push_back(*cur_block);
    } else if (this->eviction == LRU) {
        int maxCounter = 0;
        int index = 0;
        for (int i = 0; i < this->numBlockPerSet; i++) {
            if (maxCounter < cur_set->blocks.at(i).counter) {
                maxCounter = cur_set->blocks.at(i).counter;
                index = i;
            }
        }
        cur_set->blocks.erase(cur_set->blocks.begin() + index);
        cur_set->blocks.push_back(*cur_block);
    }
    //UPdate LRU
    unsigned recent_update_index = cur_set->tracker; 
    update_lru(cur_set, recent_update_index); // the new loaded block needs to update counter

    // cout << "----------------" << endl;
    // cout << "cur: " << cur_block->cache_address.tag << endl;
    // cout << "----------------" << endl;
    // this->print_cache();
}

void cache_sim::update_lru(set* cur_set, unsigned cur_block_index) {
    
    // increase counter for all
    for (unsigned i = 0; i < this->numBlockPerSet; i++) {
        cur_set->blocks.at(i).counter++;
        // update tracker
        if (cur_set->blocks.at(cur_set->tracker).counter < cur_set->blocks.at(i).counter) {
            cur_set->tracker = i;
        }
    }
    // set cur block counter to 0
    cur_set->blocks.at(cur_block_index).counter = 0;

}

// void cache_sim::process_dirty(block* cur_block) {
//     if (cur_block->is_dirty == 1) {
//         // load the existing back 
//         this->cache_metrics.total_cycles+=(this->blockSize/4) * 100;
//         cur_block->is_dirty = 0;
//     }
// }
// // block cache_sim::find_evict_block(&set cur_set) {
// //     // Assumes LRU


// // }
void cache_sim::save(cacheAddress addr)
{
    // cout << "loading index: ";
    // print_bits(addr.index);
    // cout << "tag: ";
    // print_bits(addr.tag);
    
    // check the set
    set* cur_set = &this->sets.at(addr.index);
    

    
    // check if hit or miss
    block* cur_block;
    for (unsigned i = 0; i < this->numBlockPerSet; i++) {
        cur_block = &cur_set->blocks.at(i);
        if (cur_block->cache_address.tag == addr.tag) {
            // hit
            this->cache_metrics.store_hits++;
            // Update LRU
            if (this->eviction == LRU) {
                update_lru(cur_set, i);
            }
            
            // update write through
            if ( WRITE_THRU == this->writeStrat) {
                this->cache_metrics.total_cycles += (this->blockSize / 4 * 100 + 1); // transfer straight to main hardrive
            } else { 
                this->cache_metrics.total_cycles ++;
                cur_block->is_dirty = true;
            }
            
            return;
        }
    }
    //miss
    cache_metrics.store_misses++;
    if (this->writeStrat == WRITE_THRU) {
        //check if need to evict
        this->cache_metrics.total_cycles += (this->blockSize/4) * 100; // transfer straight to main hardrive
        block* replace_block = &cur_set->blocks.at(0); // assume direct mapping
        replace_block->cache_address.tag = addr.tag; // update tag
    }
    // cout << "----------------" << endl;
    // this->print_cache();
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