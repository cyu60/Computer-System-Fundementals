//Mehul Agarwal, magarw13
//Chinat Yu, cyu60

//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;
	// just define a single dummy field.
	int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorv";
}

const char *get_plugin_desc(void) {
	return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter

	if (num_args != 0) {
		return NULL;
	}
	return calloc(1, sizeof(struct Arguments));
}


struct Image *transform_image(struct Image *source, void *arg_data) {
	void *args = arg_data;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	unsigned i, j = 0;

    for (i = 0; i < source->height/2; i++) {
        for (j = 0; j < source->width; j++) {
            out->data[i*source->width + j] = source->data[(source->height - i - 1) * source->width + j];
            out->data[(source->height - i - 1)*source->width + j] = source->data[i * source->width + j];
        }
    }

	free(args);

	return out;
}
