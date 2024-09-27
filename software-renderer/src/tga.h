/*******************************************************************
*  Targa .tga file parser
********************************************************************/

#ifndef TGA_H
#define TGA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#pragma pack(push,1) // to prevent padding
struct TgaHeader {
	uint8_t   IDLength, colourmapType, imageType;
	uint16_t  colourmapIndex, colourmapLength;
	uint8_t   colourmapDepth;
	uint16_t  xOrigin, yOrigin, width, height;
	uint8_t   bitsPerPixel, imageDescriptor;
};
#pragma pack(pop)

struct TgaImage {
	size_t width, height;
	uint32_t *data;
};

enum {
	WHITE = 0xFFFFFFFFu, //0xAARRBBGG
	RED =   0xFFFF0000u,
	BLUE =  0xFF00FF00u,
	GREEN = 0xFF0000FFu,
	BLACK = 0xFF000000u
};

struct TgaImage *tgaRead(const char *filename); //reads tga file into array of 32bit pixels
bool tgaWrite(const char *filename, const struct TgaImage *image); //writes 32 bpp tga file from memory
struct TgaImage *tgaAlloc(size_t width, size_t height);  // dynamic memory allocation
void tgaFree(struct TgaImage *image); // frees allocated memory

#endif