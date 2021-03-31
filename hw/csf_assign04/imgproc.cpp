//main file
//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu
//handles inputs and calls functions to read traces

#include <string.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <sstream> 

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

// using std::cin;
using std::cout;
using std::endl; 
using std::string;   
// using std::vector;
// using std::map;
// using std::tuple;
// using std::stringstream;
// using std::hex;

// int check_power_of_two(int n);
// int handle_errors(cacheSettings cache_settings);
struct Plugin {
    void *handle;
    const char *(*get_plugin_name)(void);
    const char *(*get_plugin_desc)(void);
    void *(*parse_arguments)(int num_args, char *args[]);
    struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

int main(int args, char* argv[]) {
    // No message

    // load plugins -- https://piazza.com/class/kkblbt3kbvz36i?cid=656
    const char* plugin_dir = getenv("PLUGIN_DIR");
    if (plugin_dir == NULL) {
    // use default plugin directory
    plugin_dir = "./plugins";
    }
    cout << "plugin dir:" << plugin_dir << endl;
    
    DIR * dir_content = opendir(plugin_dir);
    dirent * cur_plugin;

    Plugin plugin_list[5]; // max 5
    int num_plugin = 0;
    Plugin cur_plugin_details;
    while (cur_plugin = readdir(dir_content)) {
        string cur_name = cur_plugin->d_name;
        if (cur_name.size() > 3) {
            string file_extension = cur_name.substr(cur_name.size() - 3);
            if (file_extension.compare(".so") == 0) {
                cout << "cur plugin: " << cur_name << endl; // check .so
                cur_plugin_details = plugin_list[num_plugin];
                // const char* plugin_path = (cur_name).c_str();
                string plugin_dir_string = plugin_dir;
                const char* plugin_path = (plugin_dir_string + "/" + cur_name).c_str();
                // const char* plugin_path = (string(plugin_dir) + "/" + cur_name).c_str();
                cur_plugin_details.handle = dlopen("./plugins/swapbg.so"); // lazy loading
                // cur_plugin_details.handle = dlopen(plugin_path, RTLD_LAZY); // lazy loading
                cout << "cur plugin: " << cur_plugin_details.handle << endl; // check .so
                cout << "path: " << plugin_path << endl; // check .so
                cout << "path: " << plugin_dir_string << endl; // check .so
                cout << "path: " << (plugin_dir_string + "/" + cur_name).c_str() << endl; // check .so
            }
        }
    }

    closedir(dir_content);
    
    // List
    // exec command
//     if (args > 7 || args < 6) {
//         fprintf(stderr, "%s\n", "Invalid number of arguments");
//         return 1;
//     }
//     cacheSettings cache_settings;
//     cache_settings.sets = atoi(argv[1]); 
//     cache_settings.blocks = atoi(argv[2]);
//     cache_settings.bytes = atoi(argv[3]);
//     if (0 == strcmp(argv[4], "write-allocate")) {
//         cache_settings.store_strat = WRITE_ALLOC;
//     } else if(0 == strcmp(argv[4], "no-write-allocate")) {
// 	    cache_settings.store_strat = NO_WRITE_ALLOC;
//     } else {
//         cout << "unrecognized input" << endl;
//         return 1;
//     }
//     if (0 == strcmp(argv[5], "write-back")) {
//         cache_settings.write_strat = WRITE_BACK;
//     } else if(0 == strcmp(argv[5], "write-through")) {
// 	    cache_settings.write_strat = WRITE_THRU;
//     } else {
//         cout << "unrecognized input" << endl;
//         return 1;
//     }
//     if (0 == strcmp(argv[6], "lru")) {
//         cache_settings.eviction = LRU;
//     } else if(0 == strcmp(argv[6], "fifo")) {
// 	    cache_settings.eviction = FIFO;
//     } else {        
//         cout << "unrecognized input" << endl;
//         return 1;
//     }
//     if (handle_errors(cache_settings) == 1) {
//         fprintf(stderr, "%s\n", "Invalid Input");
//         return 1;
//     }
//     // Gather the cache Data
//     vector<traceLine> cacheData;
//     traceLine curTraceLine; 
//     int dummy; // for the 3rd input in the lines
//     while (cin >> curTraceLine.operation >> hex >> curTraceLine.address >> dummy) {
//         cacheData.push_back(curTraceLine);
//     }
//     cache_sim cacheSimulator = cache_sim(cache_settings);
//     cacheSimulator.process_ops(cacheData);
    return 0;
}

// //checks if a number is a power of 2
// //returns 1 if true, otherwise returns 0.
// int check_power_of_two(int n) {
//     if(n<=0) { return 0; }
//     return (n & (n - 1)) == 0; // Compare the bits should all be different
//     while(n != 1) {
//         n = n/2;
//         if(n%2 != 0 && n != 1){ return 0; }
//     }
//     return 1;
// }

// int handle_errors(cacheSettings cache_settings) {
//     if (check_power_of_two(cache_settings.sets) == 0) {
//         fprintf(stderr, "%s\n", "Sets is not a power of 2.");
//         return 1;
//     }
//     if (check_power_of_two(cache_settings.blocks) == 0) {
//         fprintf(stderr, "%s\n", "blocks is not a power of 2.");
//         return 1;
//     }
//     if (check_power_of_two(cache_settings.bytes) == 0 || cache_settings.bytes < 4) {
//         fprintf(stderr, "%s\n", "Bytes is not a power of or is less than 4");
//         return 1;
//     }
//     // Check for conflicting stradegies
//     if (cache_settings.store_strat == NO_WRITE_ALLOC && cache_settings.write_strat == WRITE_BACK) {
//         fprintf(stderr, "%s\n", "No-write-allocate and write-back called together.");
//         return 1;
//     }
//     return 0;
// }
