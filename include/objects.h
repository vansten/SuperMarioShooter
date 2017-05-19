#ifndef OBJECTS_H
#define OBJECTS_H

#include "graphics.h"
#include "transform.h"

typedef struct
{
	Quad q;
	Transform transform;
	f32 movementSpeed;
	f32 rotateSpeed;
} Player;

typedef struct
{
	Quad q;
	Transform transform;
	f32 speed;
	bool bEnabled;
} Projectile;

Player GetPlayer(Quad q, Color c, Transform t, f32 movementSpeed, f32 rotateSpeed)
{
	Player go;
	go.q = q;
	SetQuadColor(&(go.q), c);
	go.transform = t;
	go.movementSpeed = movementSpeed;
	go.rotateSpeed = rotateSpeed;
	return go;
}

Projectile GetProjectile(Quad q, bool enabled)
{
	Projectile p;
	p.q = q;
	p.bEnabled = enabled;
	return p;
}

void ShootProjectile(Projectile* p, Transform* shooterTransform, Quad* shooterQuad, f32 speed)
{
	if(!p || !shooterTransform || !shooterQuad) return;
	
	p->transform = (*shooterTransform);
	p->speed = speed;
	p->bEnabled = true;
	
	Vector dir = GetDirection(shooterTransform);
	f32 size;
	if(shooterQuad->v1.y - shooterQuad->v2.y != 0)
	{
		size = abs(shooterQuad->v1.y - shooterQuad->v2.y);
	}
	else
	{
		size = abs(shooterQuad->v1.y - shooterQuad->v3.y);
	}
	if(p->q.v1.y - p->q.v2.y != 0)
	{
		size += abs(p->q.v1.y - p->q.v2.y);
	}
	else
	{
		size += abs(p->q.v1.y - p->q.v3.y);
	}
	//size += 0.1f;
	Translatev(&(p->transform), VectorMulf32(&dir, size));
}

#endif