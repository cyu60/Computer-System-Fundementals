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
#include <iomanip>
#include "image.h"
#include "pnglite.h"

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

void print_instructions() {
    cout << "Usage: imgproc <command> [<command args...>]\nCommands are:\n  list\n  exec <plugin> <input img> <output img> [<plugin args...>]" << endl;
}

void error(string error_message) {
    cout << "Error: " << error_message << endl;
    print_instructions();
}

void list(Plugin plugin_list[], int num_plugins) {
    cout << "Loaded " << num_plugins << " plugin(s)" << endl;
    for (int i = 0; i < num_plugins; i++) {
        cout << std::setw (10) << plugin_list[i].get_plugin_name() << ": " << plugin_list[i].get_plugin_desc() << endl;
    }
}

int main(int args, char* argv[]) {
    // No message
    if (args == 1) {
        print_instructions();
        return 0; // missing command line arg return non-0??
    }
    // load plugins -- https://piazza.com/class/kkblbt3kbvz36i?cid=656
    const char* plugin_dir = getenv("PLUGIN_DIR");
    if (plugin_dir == NULL) {
    // use default plugin directory
    plugin_dir = "./plugins";
    }
    cout << "plugin dir: " << plugin_dir << endl;
    
    DIR * dir_content = opendir(plugin_dir);
    dirent * cur_plugin;

    Plugin plugin_list [5]; // max 5
    int num_plugin = 0;
    Plugin * cur_plugin_details;
    while ((cur_plugin = readdir(dir_content))) {
        string cur_name = cur_plugin->d_name;
        if (cur_name.size() > 3) {
            string file_extension = cur_name.substr(cur_name.size() - 3);
            if (file_extension.compare(".so") == 0) { // check .so
                cout << "cur plugin: " << cur_name << endl; 
                cur_plugin_details = &plugin_list[num_plugin];
                
                string plugin_dir_string = plugin_dir;
                cur_plugin_details->handle = dlopen((plugin_dir_string + "/" + cur_name).c_str(), RTLD_LAZY); // lazy loading
                cout << "cur plugin: " << cur_plugin_details->handle << endl; // check exists
                cout << "path: " << (plugin_dir_string + "/" + cur_name).c_str() << endl; // check .so

                *(void **) (&cur_plugin_details->get_plugin_name) = dlsym(cur_plugin_details->handle, "get_plugin_name");
                // cout << cur_plugin_details->get_plugin_name() << endl;
                *(void **) (&cur_plugin_details->get_plugin_desc) = dlsym(cur_plugin_details->handle, "get_plugin_desc");
                // cout << cur_plugin_details->get_plugin_desc() << endl;
                *(void **) (&cur_plugin_details->parse_arguments) = dlsym(cur_plugin_details->handle, "parse_arguments");
                *(void **) (&cur_plugin_details->transform_image) = dlsym(cur_plugin_details->handle, "transform_image");
                num_plugin++; // move to the next plugin
            }
        }
    }

    closedir(dir_content);
    
    // List
    if (0 == strcmp(argv[1], "list")) {
        list(plugin_list, num_plugin);
    }
    // exec command -- need to watch out for ERROR handling!!
    if (0 == strcmp(argv[1], "exec")) {
        Image* input = img_read_png(argv[3]);
        // check for null
        if (input == NULL) {
            error("input image not found");
            return 1;
        }

        // find the plugin

        // parse args

        // transform image

        // return data

        // img_write_png(argv[4])
    }
    return 0;
}

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
