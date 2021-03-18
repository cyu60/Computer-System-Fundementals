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

int checkPowerOfTwo(int n);
void readLine(string inputLine, string func1, string func2);

int main(int args, char* argv[]) {
    using std::cin; using std::cout; using std::endl; using std::string; using std::vector; using std::map; using std::tuple;
    // int *loads, *stores, *loadHits, *loadMisses, *storeHits, *storeMisses, *totalCycles;
    if (args > 7) {
        fprintf(stderr, "%s\n", "Too many arguments.");
        return 1;
    } else if (args < 6) {
        fprintf(stderr, "%s\n", "Not enough arguments.");
    }
    int sets = atoi(argv[1]);
    if (checkPowerOfTwo(sets) == 0) {
        fprintf(stderr, "%s\n", "Sets is not a power of 2.");
        return 1;
    }
    int blocks = atoi(argv[2]);
    if (checkPowerOfTwo(blocks) == 0  || blocks < 4) {
        fprintf(stderr, "%s\n", "blocks is not a power of 2 or is less than 4.");
        return 1;
    }
    int bytes = atoi(argv[3]);
    if (checkPowerOfTwo(bytes) == 0 || bytes < 4) {
        fprintf(stderr, "%s\n", "Bytes is not a power of 2 or is less than 4");
    }
    string function1 = argv[4];
    string function2 = argv[5];
    string mappingType = argv[6];
    string inputLine;

    if (function1 == "no-write-allocate" && function2 == "write-back") {
        fprintf(stderr, "%s\n", "No-write-allocate and write-back called together.");
    }
    vector<string> rawData;
    vector<tuple<uint32_t, bool>> cacheData;
    while (cin) {
        getline(cin, inputLine);
        rawData.push_back(inputLine);
    }
    for (int i = 0; i < rawData.size(); i++) {
        readLine()
    }
    return 0; 
}

//checks if a number is a power of 2
//returns 1 if true, otherwise returns 0.
int checkPowerOfTwo(int n) {
    if(n==0) { return 0; }
    while(n != 1) {
        n = n/2;
        if(n%2 != 0 && n != 1){ return 0; }
    }
    return 1;
}

//read one line of trace data.
//returns a tuple that has the memory address and boolean value for the dirty bit
//also calls other functions to operate on the line. 

void readLine(string inputLine, string func1, string func2) { //TODO: change return type to a tuple
    if(inputLine[0] == 's') {
            if (func2.compare("write-back") == 0) {
                //int result = writeBack()
            } else if (func2.compare("write-through") == 0) {
                //int result = writeThrough(inputLine)
            } else {
                //fprintf(stderr, "%s\n", "Invalid input for function type");
            }
        } else if (inputLine[0] == 'l') {
            if(func1.compare("write-allocate") == 0) {
                //int result = writeAllocate()
            } else if (func1.compare("no-write-allocate") == 0) {
                //int result = noWriteAllocate()
            } else {
                //fprintf(stderr, "%s\n", "Invalid input for function");
            }
        } else {
            // fprintf(stderr, "%s\n", "Invalid input");
            // exit 1;
        }
}

int writeBack() {
    return 0;
}

int writeThrough() {
    return 0;
}

int writeAllocate() {
    return 0;
}

int noWriteAllocate() {
    return 0;
}

void print_output(int total_loads, int total_stores, int load_hits, int load_misses, int store_hits,int store_misses, int total_cycles) {
    cout << "Total loads: " << total_loads << endl;
    cout << "Total stores: " << total_stores << endl;
    cout << "Load hits: " << load_hits << endl;
    cout << "Load misses: " << load_misses << endl;
    cout << "Store hits: " << store_hits << endl;
    cout << "Store misses: " << store_misses << endl;
    cout << "Total cycles: " << total_cycles << endl;
}