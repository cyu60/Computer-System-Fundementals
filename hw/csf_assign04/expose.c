
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;
	// just define a single dummy field.
	double expose_factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	// (void) args; // this is just to avoid a warning about an unused parameter

	if (num_args != 1) {
		return NULL;
	}

	// Arguments* expose_args = (Arguments *) malloc(sizeof(struct Arguments));
	struct Arguments * expose_args = calloc(1, sizeof(struct Arguments));
	expose_args->expose_factor = args[0];

	return expose_args;
	// return calloc(1, sizeof(struct Arguments));
}

static uint32_t expose(uint32_t pix, double exposeNumber) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);
	return img_pack_pixel(exposeNumber * r, exposeNumber * g, exposeNumber * b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;

	int num_pixels = source->height * source->width;
	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}


    for (unsigned i = 0; i < num_pixels; i++) {
		out->data[i] = expose(source->data[i], args->expose_factor);
	}

	free(args);

	return out;
}
