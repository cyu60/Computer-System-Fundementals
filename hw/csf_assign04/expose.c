
#include <stdlib.h>
#include "image_plugin.h"
#include <string.h>
#include <ctype.h>

#include <stdio.h>

struct Arguments {
	double expose_factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {

	if (num_args != 1) {
		return NULL;
	}
	// Check is all digits
	for (unsigned i = 0; i < strlen(args[0]); i++) {
		if (isdigit(args[0][i]) == 0 && args[0][i] != '.') { // check is digit or floating point
		printf("%c\n", args[0][i]);
			return NULL;
		}
	}

	struct Arguments * expose_args = calloc(1, sizeof(struct Arguments));
	expose_args->expose_factor = atof(args[0]);
	
	// // Check true 0 -- atof would return 0.0 if not recognized
	// if (args[0][0] != '0' && expose_args->expose_factor == 0) {
	// 	return NULL;
	// }

	return expose_args;
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
