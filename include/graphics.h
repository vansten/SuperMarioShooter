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

#define DRAW_QUAD(q) \
GX_Begin(GX_QUADS, GX_VTXFMT0, 4); \
VERTEX_POS(q.v1) \
VERTEX_COL(q.c1) \
VERTEX_POS(q.v2) \
VERTEX_COL(q.c2) \
VERTEX_POS(q.v3) \
VERTEX_COL(q.c3) \
VERTEX_POS(q.v4) \
VERTEX_COL(q.c4) \
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

typedef struct
{
	Vector v1;
	Vector v2;
	Vector v3;
	Vector v4;
	Color c1;
	Color c2;
	Color c3;
	Color c4;
} Quad;

//METHODS
Triangle GetTriangle(Vector v1, Vector v2, Vector v3)
{
	Triangle t;
	t.v1 = v1;
	t.v2 = v2;
	t.v3 = v3;
	
	return t;
}

Quad GetQuad(Vector v1, Vector v2, Vector v3, Vector v4)
{
	Quad q;
	q.v1 = v1;
	q.v2 = v2;
	q.v3 = v3;
	q.v4 = v4;
	return q;
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

void SetQuadColor(Quad* q, Color c)
{
	if(!q) return;
	
	q->c1 = q->c2 = q->c3 = q->c4 = c;
}

void SetQuadColors(Quad* q, Color c1, Color c2, Color c3, Color c4)
{
	if(!q) return;
	
	q->c1 = c1;
	q->c2 = c2;
	q->c3 = c3;
	q->c4 = c4;
}

#endif