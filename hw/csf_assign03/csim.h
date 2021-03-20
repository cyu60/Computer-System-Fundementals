//magarw13
//cyu60

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
    int is_empty; // 1 is empty, 0 is not empty 
};

struct set
{
    /* data */
    vector<block> blocks;
    // LRU - previously accessed block
    // FIFO - cur index that is accessed
    unsigned tracker; 
};

class cache_sim {
    public:
    cacheSettings cache_settings;
    // write-allocate or no-write-allocate
    int storeStrat;
    // write-through or write-back
    int writeStrat;
    // lru (least-recently-used) or fifo evictions
    int eviction; 
    unsigned bytePerBlock; 
    unsigned numSets; 
    unsigned numBlockPerSet;
    unsigned numIndexBits; 
    unsigned numOffsetBits;
    vector<set> sets; // stor all the sets
    // metric sim_metric;

    cache_sim(cacheSettings cache_settings);
    
    cacheAddress get_cache_addr(unsigned hex_addr);
    void print_cache(); // print the cache content for debug
    void process_ops(vector<traceLine> traces);
    void print_metrics(); // print the metrics
    int is_hit(cacheAddress cache_address);

};

#endif