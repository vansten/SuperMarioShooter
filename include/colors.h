#ifndef COLORS_H
#define COLORS_H

#include <gccore.h>
#include <ogc\gx.h>

#include "utility.h"

//TYPEDEFS
typedef GXColor Color;

//GLOBALS
Color CWhite = {255,255,255,255};
Color CRed = {255,0,0,255};
Color CGreen = {0,255,0,255};
Color CBlue = {0,0,255,255};
Color CYellow = {255,255,0,255};
Color CMagenta = {255,0,255,255};
Color CCyan = {0,255,255,255};
Color CBlack = {0,0,0,255};

//METHODS
Color GetColor4f32(f32 r, f32 g, f32 b, f32 a)
{
	Color c;
	c.r = clamp(r, 0.0f, 1.0f) * 255;
	c.g = clamp(g, 0.0f, 1.0f) * 255;
	c.b = clamp(b, 0.0f, 1.0f) * 255;
	c.a = clamp(a, 0.0f, 1.0f) * 255;
	return c;
}

Color GetColor4u8(u8 r, u8 g, u8 b, u8 a)
{
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

#endif