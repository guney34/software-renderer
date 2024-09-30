#ifndef VEC_H
#define VEC_H

#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

enum VecComponents {
	X,
	Y,
	Z,
};

typedef float Vec3f[3];
typedef int Vec3i[3];
typedef size_t Vec3z[3];
typedef float Vec2f[2];
typedef int Vec2i[2];

//TODO: vec operations

#endif