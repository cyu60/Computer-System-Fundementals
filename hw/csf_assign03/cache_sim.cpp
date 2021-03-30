//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu
//seondary file
//has all member functions to execute trace data and handle all counts. 

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


/*
 * Initializes all sets and block to zero. 
 * Sets all data in the cache_sim struct to the respective input data. 
 *
 * Parameters:
 *   cache_settings - Struct with all the input data provided by the user.  
 */
cache_sim::cache_sim(cacheSettings cache_settings) {
    this->store_strat = cache_settings.store_strat;
    this->write_strat = cache_settings.write_strat;
    this->eviction = cache_settings.eviction;
    this->num_block_per_set = cache_settings.blocks;
    this->num_sets = cache_settings.sets;
    this->block_size = cache_settings.bytes;
    this->num_offset_bits = 0;
    this->num_index_bits = 0;
    while (cache_settings.bytes >>= 1) {
        this->num_offset_bits++;
    }
    while (cache_settings.sets >>= 1) {
        this->num_index_bits++;
    }
    block emptyBlk;
    set emptySet;
    emptySet.blocks.assign(num_block_per_set, emptyBlk);
    emptySet.tracker = 0; // initialize tracker to be 0
    this->sets.assign(num_sets, emptySet);
}

/*
 * Handles hex memory address and finds the index and tag of the address.
 * Creates a new cacheAddress to return. 
 *
 * Parameters:
 *   hex_addr - Hex address provided in the cache memory trace. 
 * 
 * Returns:
 *   the cacheAddress that was created from the hex_addr. 
 */
cacheAddress cache_sim::get_cache_addr(unsigned hex_addr) {
    cacheAddress cur;
    // Find the bit positions of index 
    unsigned index_bits = (((1 << this->num_index_bits) - 1) << this->num_offset_bits); // 00001110000
    // cout << index_bits << endl;
    // find the values at those posistions, shift back to the index location
    cur.index = (hex_addr & index_bits) >> this->num_offset_bits; // 00010100110000 --> 00010100000 --> 000101
    // Determine the bit positions for tag
    unsigned tag_bits = ~((1 << (this->num_index_bits + this->num_offset_bits)) - 1);
    // find the values at the position, then shift back to correct position
    cur.tag = (hex_addr & tag_bits) >> (this->num_index_bits + this->num_offset_bits);
    return cur;
} 

/*
 * Runs a for loop to check every trace line of the cache. 
 * Checks if it is load or hit and calls the respective functions to perform the tasks. 
 * Prints output after reading every traceline.
 *
 * Parameters:
 *   traces - tracelines from the cache_memory trace.  
 */
void cache_sim::process_ops(vector<traceLine> traces) {
    set_metric_to_zero();
    for (unsigned i = 0; i < traces.size(); i++) {
        // process the curAddress
        cacheAddress curAddress = this->get_cache_addr(traces.at(i).address);
        // perform the correct operation
        if (traces.at(i).operation == 'l') {
            cache_metrics.total_loads++;
            this->load(curAddress);
        } else if (traces.at(i).operation == 's') {
            cache_metrics.total_stores++;
            this->save(curAddress);
        } else {
            printf("Invalid input");
            exit(1);
        }
    }
    this->print_output();
}

/*
 * Initialises all metrics to be equal to 0 so that there are no intialization problems in the future.  
 *
 */
void cache_sim::set_metric_to_zero() {
    this->cache_metrics.load_hits = 0;
    this->cache_metrics.load_misses = 0;
    this->cache_metrics.total_loads = 0;
    this->cache_metrics.store_hits = 0;
    this->cache_metrics.store_misses = 0;
    this->cache_metrics.total_stores = 0;
    this->cache_metrics.total_cycles = 0;
}

/*
 * Handles load. 
 * Updates cycles. 
 * Checks if hit/miss and acts accordingly. 
 *
 * Parameters:
 *   addr - The address of the cache_memory trace. 
 */
void cache_sim::load(cacheAddress addr) {
    set* cur_set = &this->sets.at(addr.index);
    block* cur_block;
    for (unsigned i = 0; i < this->num_block_per_set; i++) {
        if(!(cur_set->blocks.at(i).is_empty)) {
            continue;
        }
        cur_block = &cur_set->blocks.at(i);
        if (cur_block->cache_address.tag == addr.tag) {
            // hit
            this->cache_metrics.load_hits++;
            this->cache_metrics.total_cycles++;
            //Update LRU
            update_lru(cur_set, i);
            return;
        }
    }
    this->load_miss(addr, cur_set);
}

/*
 * Increments load misses. Checks the writing and storing methods and handles the cycles accordingly. 
 * Updates LRU and updates the set.
 *
 * Parameters:
 *   addr - The address of the cache_memory trace.
 *   cur_set - the set that is at the index of the address.
 */
void cache_sim::load_miss(cacheAddress addr, set* cur_set) {
    // miss
    this->cache_metrics.load_misses++;
    // 4 bytes (32 mem address) are transfered to cache
    this->cache_metrics.total_cycles += (this->block_size * 25) + 1; 
    block cur2_block(addr, 0, 1, 0);
    handle_eviction(cur_set, cur2_block);
    update_lru(cur_set, this->num_block_per_set - 1); // the new loaded block needs to update counter
}

/*
 * Handles save. 
 * Updates cycles. 
 * Checks if hit/miss and acts accordingly. 
 *
 * Parameters:
 *   addr - The address of the cache_memory trace. 
 */
void cache_sim::save(cacheAddress addr) {   
    set* cur_set = &this->sets.at(addr.index);
    block* cur_block;
    for (unsigned i = 0; i < this->num_block_per_set; i++) {
        if (!(cur_set->blocks.at(i).is_empty)) {
            continue;
        }
        cur_block = &cur_set->blocks.at(i);
        if (cur_block->cache_address.tag == addr.tag) {
            this->cache_metrics.store_hits++;
            update_lru(cur_set, i);
            if ( WRITE_THRU == this->write_strat) {
                this->cache_metrics.total_cycles += 101; // transfer straight to main hardrive
            } else { 
                this->cache_metrics.total_cycles++;
                cur_block->is_dirty = 1;
            }
            return;
        }
    }
    this->store_miss(addr, cur_set);
}

/*
 * Increments store misses. Checks the writing and storing methods and handles the cycles accordingly. 
 * Updates LRU and updates the set.
 *
 * Parameters:
 *   addr - The address of the cache_memory trace.
 *   cur_set - the set that is at the index of the address.
 */
void cache_sim::store_miss(cacheAddress addr, set* cur_set) {
    cache_metrics.store_misses++;
    block cur2_block(addr, 0, 1, 0);
    if (this->store_strat == WRITE_ALLOC) {
        cache_metrics.total_cycles++;
        handle_eviction(cur_set, cur2_block);
        update_lru(cur_set, this->num_block_per_set - 1);
        if (this->write_strat == WRITE_THRU) {
            this->cache_metrics.total_cycles += (25 * this->block_size) + 101;
        } else {
            this->cache_metrics.total_cycles += (25 * this->block_size);
        }
    } else {
        cache_metrics.total_cycles += 100;
    }
}

/*
 * Checks if the block is dirty, updates the cycles accordingly and marks the block not dirty.
 *
 * Parameters:
 *   cur_block - the block which needs to be checked if it is dirty or not. 
 *
 * 
 */
void cache_sim::process_dirty(block* cur_block) {
    if (cur_block->is_dirty == 1) {
        // load the existing back 
        this->cache_metrics.total_cycles+=(this->block_size/4) * 100;
        cur_block->is_dirty = 0;
    }
}

/*
 * Evicts the block based on the eviction method.
 * Replaces the new block that needs to be added to the set.
 *
 * Parameters:
 *   cur_set - the set in which a block needs to be evicted.
 *   cur_block - the block that needs to replace the evicted block. 
 *
 * Returns:
 *   Integer that represents the index of the block evicted. 
 */
int cache_sim::handle_eviction(set* cur_set, block cur_block) {
    if (this->eviction == FIFO) {
            process_dirty(&cur_set->blocks.at(0));
            cur_set->blocks.erase(cur_set->blocks.begin());
            cur_set->blocks.push_back(cur_block);
            return 0;
    } else if (this->eviction == LRU) { // Should use tracker that has the index???
        unsigned maxCounter, index = 0;
        for (unsigned i = 0; i < this->num_block_per_set; i++) {
            if (maxCounter < cur_set->blocks.at(i).counter) {
                maxCounter = cur_set->blocks.at(i).counter;
                index = i;
            }
        }
        process_dirty(&cur_set->blocks.at(index));
        cur_set->blocks.erase(cur_set->blocks.begin() + index);
        cur_set->blocks.push_back(cur_block);
        return index;
    }
    return -1;
}

/*
 * Updates LRU. 
 * Increases the counter for every block of the set by 1 which was not used.
 * Sets the counter of the block that was just updated to 0.
 *
 * Parameters:
 *   cur_set - The set for which blocks need to be updated. 
 *   cur_block_index - Index of the block that was just updated. 
 *
 */
void cache_sim::update_lru(set* cur_set, unsigned cur_block_index) {
    
    // increase counter for all
    for (unsigned i = 0; i < this->num_block_per_set; i++) {
        cur_set->blocks.at(i).counter++;
        // update tracker
        if (cur_set->blocks.at(cur_set->tracker).counter < cur_set->blocks.at(i).counter) {
            cur_set->tracker = i;
        }
    }
    // set cur block counter to 0
    cur_set->blocks.at(cur_block_index).counter = 0;

}

/*
 * Prints out the required output in the desired form. 
 */
void cache_sim::print_output() {
    cout << "Total loads: " << this->cache_metrics.total_loads << endl;
    cout << "Total stores: " << this->cache_metrics.total_stores << endl;
    cout << "Load hits: " << this->cache_metrics.load_hits << endl;
    cout << "Load misses: " << this->cache_metrics.load_misses << endl;
    cout << "Store hits: " << this->cache_metrics.store_hits << endl;
    cout << "Store misses: " << this->cache_metrics.store_misses << endl;
    cout << "Total cycles: " << this->cache_metrics.total_cycles << endl;
}
