//Mehul Agarwal, magarw13
//Chinat Yu, cyu60

#include <stdlib.h>
#include "image_plugin.h"
#include <string.h>
#include <ctype.h>

// #include <stdio.h>

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
	int counter = 0;
	// Check is all digits
	for (unsigned i = 0; i < strlen(args[0]); i++) {
		if (args[0][i] == '.') {
			if (counter > 1) {
				return NULL;
			}
			counter++;
		}
		if (isdigit(args[0][i]) == 0 && args[0][i] != '.') { // check is digit or floating point
		// printf("%c\n", args[0][i]);
			return NULL;
		}
	}

	struct Arguments * expose_args = calloc(1, sizeof(struct Arguments));
	expose_args->expose_factor = atof(args[0]);
	
	return expose_args;
}

static uint32_t expose(uint32_t pix, double exposeNumber) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);
	uint32_t largeR = r * exposeNumber;
	uint32_t largeG = g * exposeNumber;
	uint32_t largeB = b * exposeNumber;
	
	if (largeR > 255) {
		r = 255;
	} else {
		r = (uint8_t) largeR;
	}
	if (largeG > 255) {
		g = 255;
	} else {
		g = (uint8_t) largeG;
	}
	if (largeB > 255) {
		b = 255;
	} else {
		b = (uint8_t) largeB;
	}
	return img_pack_pixel(r, g, b, a);
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


    for (int i = 0; i < num_pixels; i++) {
		out->data[i] = expose(source->data[i], args->expose_factor);
	}

	free(args);

	return out;
}
