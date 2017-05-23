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

Vector VectorSub(Vector* v1, Vector* v2)
{
	if(!v1 || !v2) return ZeroVector;
	
	Vector v;
	v.x = v1->x - v2->x;
	v.y = v1->y - v2->y;
	v.z = v1->z - v2->z;
	
	return v;
}

void VectorNormalize(Vector* v)
{
	if(!v) return;
	f32 length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	if(abs(length) < 0.000001f)
	{
		return;
	}
	v->x /= length;
	v->y /= length;
	v->z /= length;
}

void VectorClamp(Vector* vectorToClamp, Vector* minVector, Vector* maxVector)
{
	if(!vectorToClamp || !minVector || !maxVector) return;
	
	vectorToClamp->x = min(max(minVector->x, vectorToClamp->x), maxVector->x);
	vectorToClamp->y = min(max(minVector->y, vectorToClamp->y), maxVector->y);
	vectorToClamp->z = min(max(minVector->z, vectorToClamp->z), maxVector->z);
}

f32 VectorLength(Vector* v)
{
	if(!v) return 0.0f;
	
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

f32 VectorDot(Vector* v1, Vector* v2)
{
	if(!v1 || !v2) return 0.0f;
	
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

void PrepareMatrix(Mtx* resultM, Transform* t, Mtx* view)
{
	if(!resultM || !t || !view) return;
	
	Mtx model;
	guMtxIdentity(model);
	guMtxRotAxisDeg(model, &ZAxis, t->angle);
	guMtxTransApply(model, model, t->position.x, t->position.y, t->position.z);
	guMtxConcat(*view, model, *resultM);
}

#endif