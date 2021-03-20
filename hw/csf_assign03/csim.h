#ifndef CSIM_H
#define CSIM_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <math.h>

using std::string;
using std::vector;

#define LRU 1
#define FIFO 0
#define WRITE_ALLOC 1
#define NO_WRITE_ALLOC 0
#define WRITE_BACK 1
#define WRITE_THRU 0
#define IS_EMPTY 0
#define IS_NOT_EMPTY 1

struct traceLine 
{
    /* The first field is either l or s depending on whether the processor is “loading” from or “storing” to memory. */
    char operation;
    /* The second field is a 32-bit memory address given in hexadecimal; 
        the 0x at the beginning means “the following is hexadecimal” and is not itself part of the address */
    unsigned address;
};

struct cacheSettings
{
    /*number of sets in the cache (a positive power-of-2) */
    unsigned sets;
    // /* number of blocks in each set (a positive power-of-2)
    unsigned blocks;
    // number of bytes in each block (a positive power-of-2, at least 4)
    unsigned bytes;
    // write-allocate or no-write-allocate
    int storeStrat;
    // write-through or write-back
    int writeStrat;
    // lru (least-recently-used) or fifo evictions
    int eviction; 
};

struct cacheAddress
{
    unsigned index;
    unsigned tag; 
    // Offset is not used and is ignored
};

struct block {
    cacheAddress cache_address;
    int is_dirty; // 1 is dirty, 0 is not dirty
    int is_empty; // 0 is empty, 1 is not empty 
    int counter; // LRU tracking/FIFO trakcing
};

struct set
{
    /* data */
    vector<block> blocks;
    // LRU - previously accessed block
    // FIFO - cur index that is accessed
    unsigned tracker; 
};

struct metrics {
    unsigned total_loads;
    unsigned total_stores;
    unsigned load_hits;
    unsigned load_misses;
    unsigned store_hits;
    unsigned store_misses;
    unsigned total_cycles;
};

class cache_sim {
    public:
    cacheSettings cache_settings;
    // write-allocate or no-write-allocate
    int writeStrat;
    // write-through or write-back
    int storeStrat;
    // lru (least-recently-used) or fifo evictions
    int eviction; 
    unsigned bytePerBlock; 
    unsigned numSets; 
    unsigned numBlockPerSet;
    unsigned numIndexBits; 
    unsigned numOffsetBits;
    unsigned blockSize;
    vector<set> sets; // store all the sets
    metrics cache_metrics;
    // metric sim_metric;

    cache_sim(cacheSettings cache_settings);
    
    cacheAddress get_cache_addr(unsigned hex_addr);
    void print_cache(); // print the cache content for debug
    void process_ops(vector<traceLine> traces);
    void print_output(); // print the metrics
    void directLoad(cacheAddress addr);
    void associativeLoad(cacheAddress addr);
    void fullyAssLoad(cacheAddress addr);
    void directSave(cacheAddress addr);
    void associativeSave(cacheAddress addr);
    void fullyAssSave(cacheAddress addr);
    void load(cacheAddress addr);
    void save(cacheAddress addr);
    int is_hit(cacheAddress cache_address);
    void find_evict_block(set* cur_set);
    void process_dirty(block* cur_block);
    void update_lru(set* cur_set, unsigned cur_block_index);
    // // given a memory address, returns the corresponding set index, tag, etc
    // // given a set index and block id, evict the block
    // std::pair<int, int> fetch_block(cacheAddress addr, char operation); // save/load
    // // given a set index and tag, returns true if it is a hit.
    // void flush_cache();
    
    // void restart_cache();
    // // get/sets for debugging tec
    // metric get_metrics();
};

#endif