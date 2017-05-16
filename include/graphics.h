#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gccore.h>
#include <ogc\gx.h>

#include "utility.h"
#include "colors.h"

//DEFINES
#define VERTEX_POS(v) GX_Position3f32(v.x, v.y, v.z);
#define VERTEX_COL(c) GX_Color4u8(c.r, c.g, c.b, c.a);
#define DRAW_TRIANGLE(t)\
GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3); \
VERTEX_POS(t.v1) \
VERTEX_COL(t.c1) \
VERTEX_POS(t.v2) \
VERTEX_COL(t.c2) \
VERTEX_POS(t.v3) \
VERTEX_COL(t.c3) \
GX_End();

//STRUCTS DEFINITION
typedef struct
{
	u16 width;
	u16 height;
} ScreenParams;

typedef struct
{
	Mtx view;
	Mtx44 projection;
} Matrices;

typedef struct
{
	Vector v1;
	Vector v2;
	Vector v3;
	Color c1;
	Color c2;
	Color c3;
} Triangle;

//METHODS
Triangle GetTriangle(Vector v1, Vector v2, Vector v3)
{
	Triangle t;
	t.v1 = v1;
	t.v2 = v2;
	t.v3 = v3;
	
	return t;
}

void SetTriangleColor(Triangle* t, Color c)
{
	if(!t) return;
	
	t->c1 = t->c2 = t->c3 = c;
}

void SetTriangleColors(Triangle* t, Color c1, Color c2, Color c3)
{
	if(!t) return;
	
	t->c1 = c1;
	t->c2 = c2;
	t->c3 = c3;
}

#endif