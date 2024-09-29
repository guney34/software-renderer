#include "draw.h"

void setPixel(uint32_t *buffer, size_t width, size_t height, size_t x, size_t y, uint32_t colour)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
		return;
	buffer[x + y*width] = colour;
}

void fill(uint32_t *buffer, size_t width, size_t height, uint32_t colour)
{
	uint32_t *pixel = buffer;
	while (pixel < buffer + width * height)
		*pixel++ = colour;
}

//Bresenham's line algorithm
void line(uint32_t *buffer, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t colour)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2, e2;

	for (;;) {

		//boundary check
		if (x0 >= 0 && y0 >= 0 && x0 < width && y0 < height)
			buffer[x0 + y0*width] = colour;

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

void drawModel(uint32_t *buffer, size_t width, size_t height, const struct ObjModel *model, float x, float y, float scale)
{
	for (Vec3z *face = model->faces; face < model->faces + model->num_faces; ++face) {

		//draw lines between all 3 vertices of a face (triangle)
		for (size_t i = 0; i < 3; ++i) { // polygons with higher # of vertices is not implemented
			Vec3f *v0 = model->verts + (*face)[i]; 
			Vec3f *v1 = model->verts + (*face)[(i+1) % 3]; //wrap around


			float x0 = ((*v0)[0] + x) * scale;
			float x1 = ((*v1)[0] + x) * scale;
			float y0 = ((*v0)[1] + y) * scale;
			float y1 = ((*v1)[1] + y) * scale;


			line(buffer, width, height, x0, y0, x1, y1, WHITE);
		}
	}
}

// using rotation matrix
void rotateModelAboutX(const struct ObjModel *model, float angle)
{
	for (Vec3f *vert = model->verts; vert < model->verts + model->num_verts; ++vert) {
		(*vert)[1] = (*vert)[1]*cosf(angle) - (*vert)[2]*sinf(angle);
		(*vert)[2] = (*vert)[1]*sinf(angle) + (*vert)[2]*cosf(angle);
	}
}

void rotateModelAboutY(const struct ObjModel *model, float angle)
{
	for (Vec3f *vert = model->verts; vert < model->verts + model->num_verts; ++vert) {
		(*vert)[0] = (*vert)[0]*cosf(angle) + (*vert)[2]*sinf(angle);
		(*vert)[2] = -(*vert)[0]*sinf(angle) + (*vert)[2]*cosf(angle);
	}
}

// float error causes shrinking
void rotateModelAboutZ(const struct ObjModel *model, float angle)
{
	for (Vec3f *vert = model->verts; vert < model->verts + model->num_verts; ++vert) {
		(*vert)[0] = (*vert)[0]*cosf(angle) - (*vert)[1]*sinf(angle);
		(*vert)[1] = (*vert)[0]*sinf(angle) + (*vert)[1]*cosf(angle);
	}
}

