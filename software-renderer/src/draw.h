/*******************************************************************
*  Functions for drawing to a 2d pixel array
********************************************************************/

#ifndef DRAW_H
#define DRAW_H

#include "tga.h"
#include "vec.h"
#include "obj.h"

//sets the colour of a single pixel
void setPixel(uint32_t *buffer, size_t width, size_t height, size_t x, size_t y, uint32_t colour);

//fills entire pixel array
void fill(uint32_t *buffer, size_t width, size_t height, uint32_t colour);

//Bresenham's line algorithm
void line(uint32_t *buffer, size_t width, size_t height, int x0, int y0, int x1, int y1, uint32_t colour);

//draw loaded obj file into pixel array (ignoring z values)
void drawModel(uint32_t *buffer, size_t width, size_t height, const struct ObjModel *model, float x, float y, float scale);

//rotates model about an axis
void rotateModelAboutX(struct ObjModel *model, float angle);
void rotateModelAboutY(struct ObjModel *model, float angle);
void rotateModelAboutZ(struct ObjModel *model, float angle);

#endif