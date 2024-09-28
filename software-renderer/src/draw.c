#include "draw.h"

void setPixel(struct TgaImage *image, size_t x, size_t y, uint32_t colour)
{
	size_t width = image->width; // this line allows for aliasing related compiler optimisations
	if (x < 0 || y < 0 || x >= width || y >= image->height)
		return;
	image->data[x + y*width] = colour;
}

void fill(struct TgaImage *image, uint32_t colour)
{
	uint32_t *pixel = image->data;
	size_t width = image->width; // same here
	size_t height = image->height;
	while (pixel < image->data + width * height)
		*pixel++ = colour;
}

//Bresenham's line algorithm
void line(struct TgaImage *image, int x0, int y0, int x1, int y1, uint32_t colour)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2, e2;

	for (;;) {
		setPixel(image, x0, y0, colour);

		if (x0==x1 && y0==y1) 
			break;

		e2 = err;

		if (e2 >- dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

bool drawModel(struct TgaImage *image, struct ObjModel *model)
{
	if (image == NULL || model == NULL)
		return false;

	for (Vec3z *face = model->faces; face < model->faces + model->num_faces; ++face) {

		//draw lines between all 3 vertices of a face (triangle)
		for (size_t i = 0; i < 3; ++i) { // polygons with higher # of vertices is not implemented
			Vec3f *v0 = model->verts + (*face)[i]; 
			Vec3f *v1 = model->verts + (*face)[(i+1) % 3]; //wrap around


			//wavefront vertices values are within [-1, +1] 
			//so they need to be shifted and scaled to fit the image
			float x0 = ((*v0)[0] + 1.0f) * image->width / 2.0f;
			float y0 = ((*v0)[1] + 1.0f) * image->height / 2.0f;
			float x1 = ((*v1)[0] + 1.0f) * image->width / 2.0f;
			float y1 = ((*v1)[1] + 1.0f) * image->height / 2.0f;
			line(image, x0, y0, x1, y1, WHITE);
		}
	}
	return true;
}

