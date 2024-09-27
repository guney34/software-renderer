/*******************************************************************
*  Functions for drawing to a 2d pixel array
********************************************************************/

#ifndef DRAW_H
#define DRAW_H

#include "tga.h"
#include "vec.h"
#include "obj.h"

//sets the colour of a single pixel
void setPixel(struct TgaImage *image, size_t x, size_t y, uint32_t colour);

//fills entire pixel array
void fill(struct TgaImage *image, uint32_t colour);

//Bresenham's line algorithm
void line(struct TgaImage *image, size_t x0, int y0, int x1, int y1, uint32_t colour);

//draw loaded obj file into pixel array (ignoring z values)
bool drawModel(struct TgaImage *image, struct ObjModel *model);

#endif