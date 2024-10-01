#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <intrin.h>
#include <math.h>
#include "software-renderer/src/draw.h"

#pragma intrinsic(__rdtsc)

bool writeModel(char *obj_file, char *output_file, float x, float y, float scale);

int main(void)
{
	renderModel("teapot.obj", "teapot.tga", 3, 0, 300);
	renderModel("teddy.obj", "teddy.tga", 25, 25, 20);
	renderModel("head.obj", "head.tga", 1, 1, 500);
	renderModel("cow-nonormals.obj", "cow-nonormals.tga", 5, 4, 150);
	renderModel("pumpkin_tall_10k.obj", "pumpkin_tall_10k.tga", 50, 50, 10);
}

bool writeModel(char *obj_file, char *output_file, float x, float y, float scale) {
	struct TgaImage *image = tgaAlloc(1920, 1080);
	if (image == NULL)
		return NULL;

	fill(BLACK, image->data, image->width, image->height);

	struct ObjModel *model = objRead(obj_file);
	if (model == NULL) {
		tgaFree(image);
		return NULL;
	}

	drawModel(x, y, scale, model, image->data, image->width, image->height);
	objFree(model);
	tgaWrite(output_file, image);
	tgaFree(image);
	return true;
}