//Mehul Agarwal, magarw13
//Chinat Yu, cyu60

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

void print_instructions();
void error(string error_message);
void runtime_error(string error_message);
void list(Plugin plugin_list[], int num_plugins);
int find_plugin(char* plugin_name, Plugin plugin_list[], int num_plugins);
void read_plugins(const char* plugin_dir, dirent * cur_plugin, Plugin plugin_list[], int* num_plugin);

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
    DIR * dir_content = opendir(plugin_dir);
    dirent * cur_plugin;
    Plugin plugin_list[15]; // max 15 according to piazza
    int num_plugin = 0;
    while ((cur_plugin = readdir(dir_content))) {
        read_plugins(plugin_dir, cur_plugin, plugin_list, &num_plugin);
    }
    closedir(dir_content);
    // List
    if (0 == strcmp(argv[1], "list") && args == 2) {
        list(plugin_list, num_plugin);
        return 0;
    }
    // exec command -- need to watch out for ERROR handling!!
    if (0 == strcmp(argv[1], "exec") && args >= 5) {
        Image* input = img_read_png(argv[3]);
        // check for null
        if (input == NULL) {
            runtime_error("input image not found");
        }
        // find the plugin
        int selected_plugin_idx = find_plugin(argv[2], plugin_list, num_plugin); // should return a shallow copy?   
        // check for error
        if (selected_plugin_idx == -1) {
            error("plugin not found");
        } 
        Plugin selected_plugin = plugin_list[selected_plugin_idx]; // should return a shallow copy?
        // parse args
        int num_non_args = 5;
        for (int i = num_non_args; i < args; i++) {
            argv[i-num_non_args] = argv[i]; // copy to earlier entries
        }
        void* arg_object = selected_plugin.parse_arguments(args - num_non_args, argv);
        // check for null
        if (arg_object == NULL) {
            error("argments malformed or not recognized");
        }
        // transform image
        Image* output = selected_plugin.transform_image(input, arg_object); 
        // check for null
        if (output == NULL) {
            error("image cannot be generated");
        }
        // return data
        int image_output_success = img_write_png(output, argv[4]);
        // check for error
        if (image_output_success == 0) { // 0 is not successuful
            error("image cannot be created");
        }
        // success return 0
        return 0;
    }
    // else invalid args
    error("invalid args");
}

void print_instructions() {
    cout << "Usage: imgproc <command> [<command args...>]\nCommands are:\n  list\n  exec <plugin> <input img> <output img> [<plugin args...>]" << endl;
}

void error(string error_message) {
    cout << "Error: " << error_message << endl;
    print_instructions();
    exit(1);
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

void read_plugins(const char* plugin_dir, dirent * cur_plugin, Plugin plugin_list[], int* num_plugin) {
    Plugin * cur_plugin_details;
    string cur_name = cur_plugin->d_name;
    if (cur_name.size() > 3) {
        string file_extension = cur_name.substr(cur_name.size() - 3);
        if (file_extension.compare(".so") == 0) { // check .so
            // cout << "cur plugin: " << cur_name << endl; 
            cur_plugin_details = &plugin_list[*num_plugin];
            string plugin_dir_string = plugin_dir;
            cur_plugin_details->handle = dlopen((plugin_dir_string + "/" + cur_name).c_str(), RTLD_LAZY); // lazy loading
            // cout << "cur plugin: " << cur_plugin_details->handle << endl; // check exists
            // cout << "path: " << (plugin_dir_string + "/" + cur_name).c_str() << endl; // check .so

            *(void **) (&cur_plugin_details->get_plugin_name) = dlsym(cur_plugin_details->handle, "get_plugin_name");
            *(void **) (&cur_plugin_details->get_plugin_desc) = dlsym(cur_plugin_details->handle, "get_plugin_desc");
            *(void **) (&cur_plugin_details->parse_arguments) = dlsym(cur_plugin_details->handle, "parse_arguments");
            *(void **) (&cur_plugin_details->transform_image) = dlsym(cur_plugin_details->handle, "transform_image");
            *num_plugin = *num_plugin+1; // move to the next plugin
        }
    }
}