#ifndef OBJECTS_H
#define OBJECTS_H

#include "graphics.h"
#include "transform.h"

#define MAX_LIVES 5

typedef struct
{
	Quad q;
	Sprite sprite;
	Transform transform;
	f32 movementSpeed;
	f32 rotateSpeed;
	s8 lives;
} Player;

typedef struct
{
	Quad q;
	Sprite sprite;
	Transform transform;
} Life;

typedef struct
{
	Quad q;
	Sprite sprite;
	Transform transform;
	f32 speed;
	bool bEnabled;
} Projectile;

Player GetPlayer(Quad q, TPLFile* file, s32 textureID, Transform t, f32 movementSpeed, f32 rotateSpeed)
{
	Player p;
	p.q = q;
	p.transform = t;
	p.movementSpeed = movementSpeed;
	p.rotateSpeed = rotateSpeed;
	p.sprite = GetSprite(file, textureID);
	p.lives = MAX_LIVES;
	return p;
}

Life GetLife(Quad q, TPLFile* file, s32 textureID, Transform t)
{
	Life l;
	l.q = q;
	l.sprite = GetSprite(file, textureID);
	l.transform = t;
	return l;
}

Projectile GetProjectile(Quad q, TPLFile* file, s32 textureID, bool enabled)
{
	Projectile p;
	p.q = q;
	p.sprite = GetSprite(file, textureID);
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
	f32 size = shooterQuad->size + p->q.size;
		
	Translatev(&(p->transform), VectorMulf32(&dir, size));
	Vector right = GetVector(-dir.y, dir.x, 0.0f);
	Translatev(&(p->transform), VectorMulf32(&right, 0.35f));
}

#endif