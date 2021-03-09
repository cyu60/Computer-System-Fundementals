//main file

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>

int checkPowerOfTwo(int n);

int main(int args, char* argv[]) {
    using std::cin;
    using std::cout;
    using std::endl; 
    using std::string;   
    using std::vector;
    using std::map;
    using std::tuple;

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
    int blocks =  atoi(argv[2]);
    if (checkPowerOfTwo(blocks) == 0  || blocks < 4) {
        fprintf(stderr, "%s\n", "blocks is not a power of 2 or is less than 4.");
        return 1;
    }
    int bytes = atoi(argv[3]);
    string function1 = argv[4];
    string function2 = argv[5];
    string mappingType = argv[6];
    char* inputLine;

    if (function1 == "write-back" && function2 == "no-write-allocate") {
        fprintf(stderr, "%s\n", "No-write-allocate and write-back called together.");
    }
    vector<tuple<char, uint32_t, bool>> cacheData;
    while (cin) {
        getline(cin, inputLine);
    }
    return 0;
    
}

int checkPowerOfTwo(int n) {
    if(n==0) { return 0; }
    while(n != 1) {
        n = n/2;
        if(n%2 != 0 && n != 1){ return 0; }
    }
    return 1;
}

std::tuple<char, uint32_t, bool> readLine(char* inputLine) {
    
    // std::tuple<char, uint32_t, bool> result = ;
    
}
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