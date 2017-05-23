#ifndef OBJECTS_H
#define OBJECTS_H

#include "graphics.h"
#include "transform.h"

#define MAX_LIVES 5

#define SIDES 4
#define PLUS_X 0
#define MINUS_X 1
#define PLUS_Y 2
#define MINUS_Y 3

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
} SpriteObject;

typedef struct
{
	Quad q;
	Sprite sprite;
	Transform transform;
	f32 speed;
	bool bEnabled;
} Projectile;

typedef struct
{
	Quad q;
	Sprite sprite;
	Transform transform;
	f32 movementSpeed;
	bool bEnabled;
} Enemy;

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

SpriteObject GetSpriteObject(Quad q, TPLFile* file, s32 textureID, Transform t)
{
	SpriteObject so;
	so.q = q;
	so.sprite = GetSprite(file, textureID);
	so.transform = t;
	return so;
}

Projectile GetProjectile(Quad q, TPLFile* file, s32 textureID, bool enabled)
{
	Projectile p;
	p.q = q;
	p.sprite = GetSprite(file, textureID);
	p.bEnabled = enabled;
	return p;
}

Enemy GetEnemy(Quad q, TPLFile* file, s32 textureID, Transform t, f32 movementSpeed)
{
	Enemy e;
	e.q = q;
	e.sprite = GetSprite(file, textureID);
	e.transform = t;
	e.movementSpeed = movementSpeed;
	e.bEnabled = false;
	return e;
}

void ResetPlayer(Player* p)
{
	if(!p) return;
	p->transform = GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f);
	p->lives = MAX_LIVES;
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

void SpawnEnemyRandom(Enemy* e, f32 xExtent, f32 yExtent)
{
	if(!e) return;
	
	int side = rand() % SIDES;
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 randomValue = (rand() % 1000) * 0.001f;
	
	if(side == PLUS_X)
	{
		x = e->q.size + xExtent * 0.5f;
		y = -yExtent * (0.5f + randomValue);
	}
	else if(side == MINUS_X)
	{
		x = -e->q.size - xExtent * 0.5f;
		y = -yExtent * (0.5f + randomValue);
	}
	else if(side == PLUS_Y)
	{
		x = -xExtent * (0.5f + randomValue);
		y = e->q.size + yExtent * 0.5f;
	}
	else if(side == MINUS_Y)
	{
		x = -xExtent * (0.5f + randomValue);
		y = -e->q.size - yExtent * 0.5f;
	}
	else
	{
		return;
	}
	
	e->transform.position = GetVector(x, y, 0.0f);
	e->bEnabled = true;
}

void SetEnemyDirection(Transform* enemyTransform, Transform* target)
{
	if(!enemyTransform || !target) return;
	
	Vector direction = VectorSub(&(target->position), &(enemyTransform->position));
	VectorNormalize(&direction);
	f32 angle = 0.0f;
	angle = arctan2(direction.y, direction.x);
	enemyTransform->angle = angle - 90.0f;
}

#endif