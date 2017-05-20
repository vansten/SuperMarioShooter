#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gccore.h>
#include <ogc\gx.h>

#include "utility.h"
#include "colors.h"

#include "textures_tpl.h"
#include "textures.h"

//DEFINES
#define VERTEX_POS(v) GX_Position3f32(v.x, v.y, v.z);
#define VERTEX_COL(c) GX_Color4u8(c.r, c.g, c.b, c.a);
#define VERTEX_UV(uv) GX_TexCoord2f32(uv.x, uv.y);

#define DRAW_TRIANGLE(t)\
GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3); \
VERTEX_POS(t.v1) \
VERTEX_COL(t.c) \
VERTEX_POS(t.v2) \
VERTEX_COL(t.c) \
VERTEX_POS(t.v3) \
VERTEX_COL(t.c) \
GX_End();

#define DRAW_QUAD_COLOR(q) \
GX_Begin(GX_QUADS, GX_VTXFMT0, 4); \
VERTEX_POS(q.v1) \
VERTEX_COL(q.c)\
VERTEX_POS(q.v2) \
VERTEX_COL(q.c) \
VERTEX_POS(q.v3) \
VERTEX_COL(q.c) \
VERTEX_POS(q.v4) \
VERTEX_COL(q.c) \
GX_End();

#define DRAW_QUAD_SPRITE(quad, sprite) \
GX_LoadTexObj(&sprite.textureObj, GX_TEXMAP0); \
GX_Begin(GX_QUADS, GX_VTXFMT0, 4); \
VERTEX_POS(quad.v1) \
VERTEX_COL(quad.c) \
VERTEX_UV(quad.uv1) \
VERTEX_POS(quad.v2) \
VERTEX_COL(quad.c) \
VERTEX_UV(quad.uv2) \
VERTEX_POS(quad.v3) \
VERTEX_COL(quad.c) \
VERTEX_UV(quad.uv3) \
VERTEX_POS(quad.v4) \
VERTEX_COL(quad.c) \
VERTEX_UV(quad.uv4) \
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
	Color c;
} Triangle;

typedef struct
{
	Vector v1;
	Vector v2;
	Vector v3;
	Vector v4;
	Color c;
	UV uv1;
	UV uv2;
	UV uv3;
	UV uv4;
	f32 size;
} Quad;

typedef struct
{
	GXTexObj textureObj;
} Sprite;

//METHODS
Triangle GetTriangle(Vector v1, Vector v2, Vector v3, Color c)
{
	Triangle t;
	t.v1 = v1;
	t.v2 = v2;
	t.v3 = v3;
	t.c = c;
	return t;
}

Quad GetQuad(f32 size, Color c)
{
	Quad q;
	
	f32 halfSize = 0.5f * size;
	q.v1 = GetVector(-halfSize, halfSize, 0.0f);
	q.v2 = GetVector(halfSize, halfSize, 0.0f);;
	q.v3 = GetVector(halfSize, -halfSize, 0.0f);;
	q.v4 = GetVector(-halfSize, -halfSize, 0.0f);;
	
	q.uv1 =	GetUV(1.0f, 0.0f);
	q.uv2 = GetUV(0.0f, 0.0f);
	q.uv3 = GetUV(0.0f, 1.0f);
	q.uv4 = GetUV(1.0f, 1.0f);
	
	q.c = c;
	
	q.size = size;
	
	return q;
}

Sprite GetSprite(TPLFile* file, s32 textureID)
{
	Sprite s;
	
	TPL_GetTexture(file, textureID, &s.textureObj);
	GX_LoadTexObj(&s.textureObj, GX_TEXMAP0);
	
	return s;
}

void SetTriangleColor(Triangle* t, Color c)
{
	if(!t) return;
	
	t->c = c;
}

void SetQuadColor(Quad* q, Color c)
{
	if(!q) return;
	
	q->c = c;
}

#endif