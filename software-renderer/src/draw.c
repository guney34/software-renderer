#include "draw.h"

void setPixel(size_t x, size_t y, uint32_t colour, uint32_t *buffer, size_t width, size_t height)
{
	if (x >= 0 && y >= 0 && x < width && y < height)
		buffer[x + y*width] = colour;
}

void fill(uint32_t colour, uint32_t *buffer, size_t width, size_t height)
{
	uint32_t *pixel = buffer;
	while (pixel < buffer + width * height)
		*pixel++ = colour;
}

//Bresenham's line algorithm
void line(int x0, int y0, int x1, int y1, uint32_t colour, uint32_t *buffer, size_t width, size_t height)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2, e2;

	for (;;) {

		//boundary check
		if (x0 >= 0 && y0 >= 0 && x0 < width && y0 < height)
			buffer[x0 + y0*width] = colour;

		if (x0 == x1 && y0 == y1) 
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

void drawModel(float x, float y, float scale, const struct ObjModel *model, uint32_t *buffer, size_t width, size_t height)
{
	float A = model->rot[Z]; //alpha
	float B = model->rot[Y]; //beta
	float C = model->rot[X]; //gamma

	for (Vec3z *face = model->faces; face < model->faces + model->num_faces; ++face) {

		//draw lines between all 3 vertices of a face (triangle)
		for (size_t i = 0; i < 3; ++i) { // polygons with higher # of vertices is not implemented

			Vec3f v0, v1;

			//keep the model constant to prevent float error build up
			memcpy(v0, model->verts + (*face)[i], sizeof(float)*3);
			memcpy(v1, model->verts + (*face)[(i+1) % 3], sizeof(float)*3);

			//general 3d rotation matrix
			float temp = v0[X];
			v0[X] = v0[X]*cosf(A)*cosf(B) + v0[Y]*(cosf(A)*sinf(B)*sinf(C) - sinf(A)*cosf(C)) + v0[Z]*(cosf(A)*sinf(B)*cosf(C) + sinf(A)*sinf(C));
			v0[Y] = temp*sinf(A)*cosf(B) + v0[Y]*(sinf(A)*sinf(B)*sinf(C) + cosf(A)*cosf(C)) + v0[Z]*(sinf(A)*sinf(B)*cosf(C) - cosf(A)*sinf(C));

			temp = v1[X];
			v1[X] = v1[X]*cosf(A)*cosf(B) + v1[Y]*(cosf(A)*sinf(B)*sinf(C) - sinf(A)*cosf(C)) + v1[Z]*(cosf(A)*sinf(B)*cosf(C) + sinf(A)*sinf(C));
			v1[Y] = temp*sinf(A)*cosf(B) + v1[Y]*(sinf(A)*sinf(B)*sinf(C) + cosf(A)*cosf(C)) + v1[Z]*(sinf(A)*sinf(B)*cosf(C) - cosf(A)*sinf(C));

			//shift and scale before drawing
			float x0 = (v0[X] + x) * scale;
			float x1 = (v1[X] + x) * scale;
			float y0 = (v0[Y] + y) * scale;
			float y1 = (v1[Y] + y) * scale;


			line(x0, y0, x1, y1, WHITE, buffer, width, height);
		}
	}
}

// using rotation matrix
void rotateModelAboutX(float angle, struct ObjModel *model)
{
	model->rot[X] = fmod(model->rot[X] + angle, 2.0f * M_PI);
}	

void rotateModelAboutY(float angle, struct ObjModel *model)
{
	model->rot[Y] = fmod(model->rot[Y] + angle, 2.0f * M_PI);
}

void rotateModelAboutZ(float angle, struct ObjModel *model)
{
	model->rot[Z] = fmod(model->rot[Z] + angle, 2.0f * M_PI);
}

