#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <intrin.h>
#include <math.h>
#include "src/draw.h"

#pragma intrinsic(__rdtsc)

int main(void)
{
	struct TgaImage *image = tgaAlloc(1000, 1000);
	if (image == NULL)
		return EXIT_FAILURE;

	fill(image, BLACK);

	struct ObjModel *model = objRead("head.obj");
	if (model == NULL)
		return EXIT_FAILURE;

	printf("num of verts: %zu\n", model->num_verts);
	printf("num of faces: %zu\n", model->num_faces);
	
	size_t iterations = 1;
	uint64_t t = __rdtsc();
	for (size_t i = 0; i < iterations; ++i) {
		drawModel(image, model);
	}
	t = __rdtsc() - t;
	printf("Total ticks: %llu\n", t);
	printf("Ticks/iteration: %g\n", (double)t/iterations);

	objFree(model);
	tgaWrite("test.tga", image);
	tgaFree(image);
}



