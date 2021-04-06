//main file
//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu

#include <string.h>
#include <cassert>
#include <iostream>

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <iomanip>
#include "image.h"
#include "pnglite.h"

using std::cout;
using std::endl; 
using std::string;   

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

void runtime_error(string error_message) {
    cout << "Error: " << error_message << endl;
}

void list(Plugin plugin_list[], int num_plugins) {
    cout << "Loaded " << num_plugins << " plugin(s)" << endl;
    for (int i = 0; i < num_plugins; i++) {
        cout << std::setw (10) << plugin_list[i].get_plugin_name() << ": " << plugin_list[i].get_plugin_desc() << endl;
    }
}

int find_plugin(char* plugin_name, Plugin plugin_list[], int num_plugins) {
    for (int i = 0; i < num_plugins; i++) {
        if (0 == strcmp(plugin_name, plugin_list[i].get_plugin_name())) {
            return i;
        }
        // cout << plugin_list[i].get_plugin_name() << ": " << plugin_list[i].get_plugin_desc() << endl;
    }
    return -1;
}

void read_plugins(Plugin plugin_list[], Plugin* cur_plugin_details, int* &num_plugin) {
        string cur_name = cur_plugin->d_name;
        if (cur_name.size() > 3) {
            string file_extension = cur_name.substr(cur_name.size() - 3);
            if (file_extension.compare(".so") == 0) { // check .so
                // cout << "cur plugin: " << cur_name << endl; 
                cur_plugin_details = &plugin_list[num_plugin];
                
                string plugin_dir_string = plugin_dir;
                cur_plugin_details->handle = dlopen((plugin_dir_string + "/" + cur_name).c_str(), RTLD_LAZY); // lazy loading
                // cout << "cur plugin: " << cur_plugin_details->handle << endl; // check exists
                // cout << "path: " << (plugin_dir_string + "/" + cur_name).c_str() << endl; // check .so

                *(void **) (&cur_plugin_details->get_plugin_name) = dlsym(cur_plugin_details->handle, "get_plugin_name");
                *(void **) (&cur_plugin_details->get_plugin_desc) = dlsym(cur_plugin_details->handle, "get_plugin_desc");
                *(void **) (&cur_plugin_details->parse_arguments) = dlsym(cur_plugin_details->handle, "parse_arguments");
                *(void **) (&cur_plugin_details->transform_image) = dlsym(cur_plugin_details->handle, "transform_image");
                *num_plugin++; // move to the next plugin
            }
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
    // cout << "plugin dir: " << plugin_dir << endl;
    
    DIR * dir_content = opendir(plugin_dir);
    dirent * cur_plugin;

    Plugin plugin_list[15]; // max 15 according to piazza
    Plugin * cur_plugin_details;
    int num_plugin = 0;

    while ((cur_plugin = readdir(dir_content))) {
        read_plugins(plugin_list, cur_plugin_details, &num_plugin);
    }
    // while ((cur_plugin = readdir(dir_content))) {
    //     string cur_name = cur_plugin->d_name;
    //     if (cur_name.size() > 3) {
    //         string file_extension = cur_name.substr(cur_name.size() - 3);
    //         if (file_extension.compare(".so") == 0) { // check .so
    //             // cout << "cur plugin: " << cur_name << endl; 
    //             cur_plugin_details = &plugin_list[num_plugin];
                
    //             string plugin_dir_string = plugin_dir;
    //             cur_plugin_details->handle = dlopen((plugin_dir_string + "/" + cur_name).c_str(), RTLD_LAZY); // lazy loading
    //             // cout << "cur plugin: " << cur_plugin_details->handle << endl; // check exists
    //             // cout << "path: " << (plugin_dir_string + "/" + cur_name).c_str() << endl; // check .so

    //             *(void **) (&cur_plugin_details->get_plugin_name) = dlsym(cur_plugin_details->handle, "get_plugin_name");
    //             *(void **) (&cur_plugin_details->get_plugin_desc) = dlsym(cur_plugin_details->handle, "get_plugin_desc");
    //             *(void **) (&cur_plugin_details->parse_arguments) = dlsym(cur_plugin_details->handle, "parse_arguments");
    //             *(void **) (&cur_plugin_details->transform_image) = dlsym(cur_plugin_details->handle, "transform_image");
    //             num_plugin++; // move to the next plugin
    //         }
    //     }
    // }

    closedir(dir_content);
    
    // List
    if (0 == strcmp(argv[1], "list")) {
        list(plugin_list, num_plugin);
    }

    // error
    if (args < 4) {
        error("insufficient args");
        return 1;
    }
    // exec command -- need to watch out for ERROR handling!!
    if (0 == strcmp(argv[1], "exec")) {
        Image* input = img_read_png(argv[3]);
        // check for null
        if (input == NULL) {
            runtime_error("input image not found");
            return 1;
        }

        // find the plugin
        int selected_plugin_idx = find_plugin(argv[2], plugin_list, num_plugin); // should return a shallow copy?
        
        // check for error
        if (selected_plugin_idx == -1) {
            error("plugin not found");
            return 1;
        } 

        Plugin selected_plugin = plugin_list[selected_plugin_idx]; // should return a shallow copy?
        // cout << selected_plugin.get_plugin_name() << endl;

        // parse args
        int num_non_args = 5;
        for (int i = num_non_args; i < args; i++) {
            argv[i-num_non_args] = argv[i]; // copy to earlier entries
        }
        // cout << args - num_non_args << endl;
        void* arg_object = selected_plugin.parse_arguments(args - num_non_args, argv);

        // check for null
        if (arg_object == NULL) {
            error("argments malformed or not recognized");
            return 1;
        }

        // transform image
        Image* output = selected_plugin.transform_image(input, arg_object);
        
        // check for null
        if (output == NULL) {
            error("image cannot be generated");
            return 1;
        }

        // return data
        int image_output_success = img_write_png(output, argv[4]);

        // check for error
        if (image_output_success == 0) { // 0 is not successuful
            error("image cannot be created");
            return 1;
        }
    }
    // else not recognized 

    return 0;
}