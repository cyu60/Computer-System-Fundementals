//Mehul Agarwal, magarw13
//Chinat Yu, cyu60

#include <stdlib.h>
#include "image_plugin.h"
#include <string.h>
#include <ctype.h>

static int cordinate_conversion(int point, int limit_point, int tile_size, int factor);

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


	// Check is all digits
	for (unsigned i = 0; i < strlen(args[0]); i++) {
		if (isdigit(args[0][i]) == 0) { // check is digit
			return NULL;
		}
	}

	struct Arguments * tile_args = calloc(1, sizeof(struct Arguments));
	tile_args->num_tiles = atoi(args[0]);

	return tile_args;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
    // Allocate a result Image
    struct Image *out = img_create(source->width, source->height);
    if (!out) {
        free(args);
        return NULL;
    }
    int limitX = (source->width % args->num_tiles) * (source->width / args->num_tiles + 1);
    int limitY = (source->height % args->num_tiles) * (source->height / args->num_tiles + 1);
    int src1, src2;
	int i, j = 0;
    for (i = 0; i < (int)source->height; i++) {
      for (j = 0; j < (int)source->width; j++){
		src1 = cordinate_conversion(i, limitY, source->height / args->num_tiles, args->num_tiles);
		src2 = cordinate_conversion(j, limitX, source->width / args->num_tiles, args->num_tiles);
		out->data[i*source->width+j] = source->data[src1*source->width+src2];
      }
    }
    free(args);
    return out;
}

static int cordinate_conversion(int point, int limit_point, int tile_size, int factor){
  int source;
	if(point >= limit_point) {
		source = ((point - limit_point) % (tile_size)) * factor;
    } else {
    	source = (point % (tile_size + 1)) * factor;
    }
	return source;
}
