//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	int num_tiles;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}

	struct Arguments * tile_args = calloc(1, sizeof(struct Arguments));
	tile_args->num_tiles = atoi(args[0]);
	// Need to perform error check??? atof would return 0.0

	return tile_args;
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
