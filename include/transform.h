#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <gccore.h>
#include <ogc\gx.h>

#include "utility.h"

//TYPEDEFS
typedef guVector Vector;
typedef guQuaternion Quaternion;

//GLOBALS
Vector XAxis = {1.0f, 0.0f, 0.0f};
Vector YAxis = {0.0f, 1.0f, 0.0f};
Vector ZAxis = {0.0f, 0.0f, 1.0f};
Vector ZeroVector = {0.0f, 0.0f, 0.0f};
Vector OneVector = {1.0f, 1.0f, 1.0f};

//STRUCTS DEFINITION
typedef struct
{
	Vector position;
	f32 angle;
} Transform;

//METHODS
Transform GetTransform4f32(f32 posX, f32 posY, f32 posZ, f32 zAngle)
{
	Transform t;
	t.position.x = posX;
	t.position.y = posY;
	t.position.z = posZ;
	t.angle = zAngle;
	return t;
}

Transform GetTransform1v1f32(Vector position, f32 zAngle)
{
	Transform t;
	t.position = position;
	t.angle = zAngle;
	return t;
}

void Translate3f32(Transform* t, f32 x, f32 y, f32 z)
{
	if(!t) return;
	t->position.x += x;
	t->position.y += y;
	t->position.z += z;
}

void Translatev(Transform* t, Vector v)
{
	if(!t) return;
	t->position.x += v.x;
	t->position.y += v.y;
	t->position.z += v.z;
}

void Rotate1f32(Transform* t, f32 z)
{
	if(!t) return;
	
	t->angle += z;
}

Vector GetVector(f32 x, f32 y, f32 z)
{
	Vector v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

Vector GetDirection(Transform* t)
{
	if(!t) return ZeroVector;
	Vector dir;
	f32 theta = t->angle * DEG2RAD;

	f32 cs = cos(theta);
	f32 sn = sin(theta);

	dir.x = YAxis.x * cs - YAxis.y * sn;
	dir.y = YAxis.x * sn + YAxis.y * cs;
	dir.z = 0.0f;
	return dir;
}

Vector VectorMulf32(Vector* v, f32 scale)
{
	if(!v) return ZeroVector;
	
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
	return *v;
}

#endif
