//main file

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>


int main(int args, char* argv[]) {
    using std::cout;
    using std::endl; 
    using std::string;   
    using std::vector;
    using std::map;

    int *loads, *stores, *loadHits, *loadMisses, *storeHits, *storeMisses, totalCycles;

    if (args > 7) {
        fprintf(stderr, "%s\n", "Not enough arguments");
        return 1;
    }    
    int *sets = (int*) argv[1];
    int *blocks = (int*) argv[2];
    int *bytes = (int*) argv[3];
    string function1 = argv[4];
    string function2 = argv[5];
    string mappingType = argv[6];
}

