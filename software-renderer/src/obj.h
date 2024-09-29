/*******************************************************************
*  Wavefront .obj file parser
********************************************************************/

#ifndef OBJ_H
#define OBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"

#define OBJ_MAX_LINE_SIZE 100

struct ObjModel {
	size_t num_verts, num_faces;
	Vec3f *verts;
	Vec3z *faces; // currently, only triangle faces are supported; each vertex of a triangle is an element of the Vec3z array
	Vec3f rot;
};

struct ObjModel *objRead(const char *filename); //reads obj file into ObjModel struct
void objFree(struct ObjModel *model); // frees allocated memory

#endif