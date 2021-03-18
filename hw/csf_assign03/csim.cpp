//main file

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <sstream> 


#include "csim.h"

using std::cin;
using std::cout;
using std::endl; 
using std::string;   
using std::vector;
using std::map;
using std::tuple;
using std::stringstream;
using std::hex;

int checkPowerOfTwo(int n);
std::tuple<uint32_t, bool> readLine(string inputLine, string func1, string func2);

int main(int args, char* argv[]) {
    // int *loads, *stores, *loadHits, *loadMisses, *storeHits, *storeMisses, *totalCycles;

    if (args > 7) {
        fprintf(stderr, "%s\n", "Too many arguments.");
        return 1;
    } else if (args < 6) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
    }
    
    cacheSettings cache_settings;
    cache_settings.sets = atoi(argv[1]);
    if (checkPowerOfTwo(cache_settings.sets) == 0) {
        fprintf(stderr, "%s\n", "Sets is not a power of 2.");
        return 1;
    }
    cache_settings.blocks = atoi(argv[2]);
    if (checkPowerOfTwo(cache_settings.blocks) == 0) {
        fprintf(stderr, "%s\n", "blocks is not a power of 2.");
        return 1;
    }
    cache_settings.bytes = atoi(argv[3]);
    if (checkPowerOfTwo(cache_settings.bytes) == 0 || cache_settings.bytes < 4) {
        fprintf(stderr, "%s\n", "Bytes is not a power of or is less than 4");
    }

    if (0==strcmp(argv[4], "write-allocate")) {
        cache_settings.storeStrat = WRITE_ALLOC;
    } else if(0==strcmp(argv[4], "no-write-allocate")){
	    cache_settings.storeStrat = NO_WRITE_ALLOC;
    } else{
        cout << "unrecognized input" << endl;
        return 1;
    }
    
    if (0==strcmp(argv[5], "write-back")) {
        cache_settings.writeStrat = WRITE_BACK;
    } else if(0==strcmp(argv[5], "write-through")){
	    cache_settings.writeStrat = WRITE_THRU;
    } else{
        cout << "unrecognized input" << endl;
        return 1;
    }

    // Check for conflicting stradegies
    if (cache_settings.storeStrat == NO_WRITE_ALLOC && cache_settings.writeStrat == WRITE_BACK) {
        fprintf(stderr, "%s\n", "No-write-allocate and write-back called together.");
    }
    
    if (0==strcmp(argv[6], "lru")) {
        cache_settings.eviction = LRU;
    } else if(0==strcmp(argv[6], "fifo")){
	    cache_settings.eviction = FIFO;
    } else{        
        cout << "unrecognized input" << endl;
        return 1;
    }

    // Gather the cache Data
    vector<traceLine> cacheData;
    traceLine curTraceLine; 
    int dummy; // for the 3rd input in the lines
    while (cin >> curTraceLine.operation >> hex >> curTraceLine.address >> dummy) {
        // getline(cin, inputLine);
        // cacheData.push_back(readLine(inputLine, function1, function2));
        cacheData.push_back(curTraceLine);
        cout << curTraceLine.operation << curTraceLine.address << endl;
    }

    // Set up cache Sim 
    // while (cin) {
    //     getline(cin, inputLine, '\n');
    // }

    // Trying to test the input read of the memory? Planning to use string stream?
    // string test_write;
    // stringstream input;
    // while (cin) {
    //     cin >> input; 
    //     cout << input.str();
    // }
    
    return 0; 
}

//checks if a number is a power of 2
//returns 1 if true, otherwise returns 0.
int checkPowerOfTwo(int n) {
    if(n<=0) { return 0; }
    return (n & (n - 1)) == 0; // Compare the bits should all be different
    // while(n != 1) {
    //     n = n/2;
    //     if(n%2 != 0 && n != 1){ return 0; }
    // }
    // return 1;
}

//read one line of trace data.
//returns a tuple that has the memory address and boolean value for the dirty bit
//also calls other functions to operate on the line. 
// std::tuple<uint32_t, bool> readLine(string inputLine, char* func1, char* func2) {
//     // if(inputLine[0] == 's') {
//     //     //     if (func2.compare("write-back") == 0) {
//     //     //         //int result = writeBack()
//     //     //     } else if (func2 == "write-through" ) {
//     //     //         //int result = writeThrough(inputLine)
//     //     //     } else {
//     //     //         //fprintf(stderr, "%s\n", "Invalid input for function type");
//     //     //     }
//     //     // } else if (inputLine[0] == 'l') {
//     //     //     if(func1 == "write-allocate") {
//     //     //         //int result = writeAllocate()
//     //     //     } else if (func1 == "no-write-allocate") {
//     //     //         //int result = noWriteAllocate()
//     //     //     } else {
//     //     //         //fprintf(stderr, "%s\n", "Invalid input for function");
//     //     //     }
//     //     // } else {
//     //     //     // fprintf(stderr, "%s\n", "Invalid input");
//     //     //     // exit 1;
//         // }
//         return NULL;
// }
// int writeBack() {
//     return NULL;
// }

// int writeThrough() {
//     return NULL;
// }

// int writeAllocate() {
//     return NULL;
// }

// int noWriteAllocate() {
//     return NULL;
// }

void print_output(int total_loads, int total_stores, int load_hits, int load_misses, int store_hits,int store_misses, int total_cycles) {
    cout << "Total loads: " << total_loads << endl;
    cout << "Total stores: " << total_stores << endl;
    cout << "Load hits: " << load_hits << endl;
    cout << "Load misses: " << load_misses << endl;
    cout << "Store hits: " << store_hits << endl;
    cout << "Store misses: " << store_misses << endl;
    cout << "Total cycles: " << total_cycles << endl;
}

