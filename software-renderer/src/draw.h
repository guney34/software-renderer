/*******************************************************************
*  Functions for drawing to a 2d pixel array
********************************************************************/

#ifndef DRAW_H
#define DRAW_H

#include "tga.h"
#include "vec.h"
#include "obj.h"

//sets the colour of a single pixel
void setPixel(size_t x, size_t y, uint32_t colour, uint32_t *buffer, size_t width, size_t height);

//fills entire pixel array
void fill(uint32_t colour, uint32_t *buffer, size_t width, size_t height);

//Bresenham's line algorithm
void line(int x0, int y0, int x1, int y1, uint32_t colour, uint32_t *buffer, size_t width, size_t height);

//draw loaded obj file into pixel array (ignoring z values)
void drawModel(float x, float y, float scale, const struct ObjModel *model, uint32_t *buffer, size_t width, size_t height);

//rotates model about an axis
void rotateModelAboutX(float angle, struct ObjModel *model);
void rotateModelAboutY(float angle, struct ObjModel *model);
void rotateModelAboutZ(float angle, struct ObjModel *model);

#endif