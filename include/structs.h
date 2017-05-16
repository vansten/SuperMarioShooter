#include <gccore.h>
#include <ogc\gx.h>

//DEFINES
#define BTN_PRESSED(button) ((gInput.buttons & button) != 0 && (gPrevInput.buttons & button) == 0)
#define BTN_RELEASED(button) ((gInput.buttons & button) == 0 && (gPrevInput.buttons & button) != 0)
#define BTN_HELD(button) ((gInput.buttons & button) != 0)

#define DEFAULT_FIFO_SIZE (256 * 1024)

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define clamp(a, min_val, max_val) min(max(a, min_val), max_val)

#define VERTEX_POS(v) GX_Position3f32(v.x, v.y, v.z);
#define VERTEX_COL(c) GX_Color4u8(c.r, c.g, c.b, c.a);
#define DRAW_TRIANGLE(t)\
VERTEX_POS(t.v1) \
VERTEX_COL(t.c1) \
VERTEX_POS(t.v2) \
VERTEX_COL(t.c2) \
VERTEX_POS(t.v3) \
VERTEX_COL(t.c3)

//TYPEDEFS
typedef GXColor Color;
typedef guVector Vector;
typedef guQuaternion Quaternion;

//STRUCTS DEFINITION
typedef struct
{
	f32 x;
	f32 y;
	f32 z;
} Transform;

typedef struct
{
	u16 buttons;
	f32 stickX;
	f32 stickY;
} Input;

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

Transform GetTransform(f32 x, f32 y, f32 z)
{
	Transform t;
	t.x = x;
	t.y = y;
	t.z = z;
	return t;
}

void Translate3f32(Transform* t, f32 x, f32 y, f32 z)
{
	if(!t) return;
	t->x += x;
	t->y += y;
	t->z += z;
}

void Translatev(Transform* t, Vector v)
{
	if(!t) return;
	t->x += v.x;
	t->y += v.y;
	t->z += v.z;
}

Input GetInput()
{
	Input i;
	i.buttons = 0;
	i.stickX = 0;
	i.stickY = 0;
	return i;
}

Vector GetVector(f32 x, f32 y, f32 z)
{
	Vector v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

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

Color GetColor(f32 r, f32 g, f32 b, f32 a)
{
	Color c;
	c.r = clamp(r, 0.0f, 1.0f) * 255;
	c.g = clamp(g, 0.0f, 1.0f) * 255;
	c.b = clamp(b, 0.0f, 1.0f) * 255;
	c.a = clamp(a, 0.0f, 1.0f) * 255;
	return c;
}