#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>

#include "includes.h"

#define PROJECTILES_COUNT 50
#define ENEMY_POOL_COUNT 30

#define ENEMY_SPAWN_TIME 5.0f

#define GS_MENU 0
#define GS_GAME 1
#define GS_GAMEOVER 2

/*
 * Graphics variables
 */
static void* frameBuffer[2] = { NULL, NULL };
u8 currentFB = 0;
static GXRModeObj *rmode;
void* gFIFO;
GXColor background = {0, 0, 0, 0xff};

Matrices gMatrices;
ScreenParams gScreenParams;

/*
 * Input variables
 */
Input gInput;
Input gPrevInput;

/*
 * Gameplay variables
 */
Camera gCamera;
World gWorld;
 
Player gPlayer;

Projectile gProjectiles[PROJECTILES_COUNT];
u32 gLastAvailableProjectile;
f32 gShootProjectileTimer = 0.0f;

Enemy gEnemies[ENEMY_POOL_COUNT];
f32 gEnemyTimer = 0.0f;

SpriteObject gFloors[64];
SpriteObject gLives[MAX_LIVES];
SpriteObject gMainMenu;
SpriteObject gGameOver;

s8 gGameState = GS_MENU;

/*
 * Forward declarations for initialization code
 */
void Initialise(f32 orthoSize);
void InitGraphics(f32 orthoSize);

/*
 * Forward declarations for gameplay code
 */
void InitGameplay();
void ResetGameState();

void CeckProjectiles();
bool CanMove(Player* playerPtr, Vector* inputVector);
bool IsEnemyCloseToPlayer(Enemy* e, f32 maxDistance);
bool IsProjectileCloseToEnemy(Projectile* p, Enemy* e, f32 maxDistance);
u32 GetLastAvailableEnemy();
u32 GetEnemyCloseToProjectile(Projectile* p);

void UpdateMenu(f32 deltaTime);
void UpdateGame(f32 deltaTime);
void UpdateGameOver(f32 deltaTime);

void DrawMenu(Mtx* modelView);
void DrawGame(Mtx* modelView);
void DrawGameOver(Mtx* modelView);

/*
 * Entry point
 */
int main()
{
	Initialise(30.0f);
	
	Mtx modelView;
	f32 deltaTime = 1.0f / 60.0f;
	
	while(1)
	{	
		gPrevInput = gInput;
		GatherInput(&gInput);
		
		GX_InvalidateTexAll();
		GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	
		if(gGameState == GS_MENU)
		{
			UpdateMenu(deltaTime);
			DrawMenu(&modelView);
		}
		else if(gGameState == GS_GAME)
		{
			UpdateGame(deltaTime);
			DrawGame(&modelView);
		}
		else if(gGameState == GS_GAMEOVER)
		{
			UpdateGameOver(deltaTime);
			DrawGameOver(&modelView);
		}
		
		GX_DrawDone();
		
		currentFB ^= 1;
		GX_CopyDisp(frameBuffer[currentFB], GX_TRUE);
		
		VIDEO_SetNextFramebuffer(frameBuffer[currentFB]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		
		if(BTN_RELEASED(PAD_BUTTON_START))
		{
			exit(0);
		}
	}
 
	return 0;
}


void Initialise(f32 orthoSize)
{
	InitGraphics(orthoSize);
	InitInput(&gInput);
	gPrevInput = gInput;
	
	InitGameplay();
}

void InitGraphics(f32 orthoSize)
{	
	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);

	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[currentFB]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	gFIFO = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gFIFO, 0, DEFAULT_FIFO_SIZE);
	
	GX_Init(gFIFO, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear(background, 0x00ffffff);
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	f32 yScale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yScale);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2*rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	
	if (rmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[currentFB], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	
	gScreenParams.width = rmode->viWidth;
	gScreenParams.height = rmode->viHeight;
	f32 aspectRatio = (f32)gScreenParams.width / (f32)gScreenParams.height;
	gScreenParams.orthoSize = orthoSize;
	gScreenParams.xExtent = orthoSize * aspectRatio;
	gScreenParams.yExtent = orthoSize;
	
	guOrtho(gMatrices.projection, orthoSize / 2, -orthoSize / 2, aspectRatio * -orthoSize / 2, aspectRatio * orthoSize / 2, 0.1f, 300.0f);
	GX_LoadProjectionMtx(gMatrices.projection, GX_ORTHOGRAPHIC);
		
	GX_InvalidateTexAll();
}

/************************************************************************
 * GAMEPLAY CODE
 */
void InitGameplay()
{
	TPLFile tplFile;
	TPL_OpenTPLFromMemory(&tplFile, (void*)textures_tpl, textures_tpl_size);
	
	Quad spriteObjectQuad = GetQuad(10.0f, CWhite);
	gMainMenu = GetSpriteObject(spriteObjectQuad,
								&tplFile,
								mainmenu,
								GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f)
								);
								
	
	gGameOver = GetSpriteObject(spriteObjectQuad,
								&tplFile,
								gameover,
								GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f)
								);
	
	gPlayer = GetPlayer(GetQuad(2.5f, CWhite),
						&tplFile,
						player,
						GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f),
						3.0f, 
						60.0f
						);
				
	Quad projectileQuad = GetQuad(1.0f, CWhite);
	for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		gProjectiles[i] = GetProjectile(projectileQuad, 
										&tplFile,
										projectile, 
										false
										);
	}
	gLastAvailableProjectile = 0;
	
	Quad lifeQuad = GetQuad(1.0f, CRed);
	f32 y = gScreenParams.yExtent * 0.5f - lifeQuad.size - 0.25f;
	f32 initX = gScreenParams.xExtent * 0.5f - lifeQuad.size;
	f32 xOffset = -lifeQuad.size;
	for(u32 i = 0; i < MAX_LIVES; ++i)
	{
		gLives[i] = GetSpriteObject(lifeQuad,
									&tplFile,
									life,
									GetTransform4f32(initX + xOffset * i, y, 0.0f, 0.0f)
									);
	}
	
	Quad enemyQuad = GetQuad(3.0f, CRed);
	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		gEnemies[i] = GetEnemy(enemyQuad,
								&tplFile,
								enemy,
								GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f),
								1.0f
								);
	}
	
	gCamera = GetCamera(GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f),
						-gScreenParams.orthoSize
						);
	
	guLookAt(gMatrices.view, &gCamera.transform.position, &YAxis, &ZeroVector);
	guLookAt(gMatrices.viewGUI, &gCamera.transform.position, &YAxis, &ZeroVector);
	
	Quad floorQuad = GetQuad(20.0f, CWhite);
	f32 offset = floorQuad.size;
	initX = -4 * offset;
	f32 initY = -4 * offset;
	for(u32 i = 0; i < 8; ++i)
	{
		for(int j = 0; j < 8; ++j)
		{
			gFloors[i * 8 + j] = GetSpriteObject(floorQuad,
												  &tplFile,
												  floor,
												  GetTransform4f32(initX + i * offset, initY + j * offset, 0.0f, 0.0f)
												  );
		}
	}
	
	gWorld = GetWorld(-floorQuad.size * 4.5f, floorQuad.size * 3.5f, -floorQuad.size * 4.5f, floorQuad.size * 3.5f, gScreenParams.xExtent, gScreenParams.yExtent);
	
	srand(time(0));
}

void ResetGameState()
{
	ResetPlayer(&gPlayer);
	
	for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		gProjectiles[i].bEnabled = false;
	}
	
	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		gEnemies[i].bEnabled = false;
	}
	
	gLastAvailableProjectile = 0;
	gShootProjectileTimer = 0.0f;
	gEnemyTimer = 0.0f;
}
 
void CheckProjectiles()
{
	for(s32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		if(gProjectiles[i].bEnabled)
		{
			if(gProjectiles[i].transform.position.x > (0.5f * gScreenParams.xExtent + gCamera.transform.position.x)
				||
				gProjectiles[i].transform.position.x < (-0.5f * gScreenParams.xExtent + gCamera.transform.position.x)
				||
				gProjectiles[i].transform.position.y > (0.5f * gScreenParams.yExtent + gCamera.transform.position.y)
				||
				gProjectiles[i].transform.position.y < (-0.5f * gScreenParams.yExtent + gCamera.transform.position.y))
			{
				gProjectiles[i].bEnabled = false;
				if(gLastAvailableProjectile > i)
				{
					gLastAvailableProjectile = i;
				}
			}
		}
	}
}

bool CanMove(Player* playerPtr, Vector* inputVector)
{
	if(!playerPtr || !inputVector) return true;
	
	Vector newPos = playerPtr->transform.position;
	newPos.x += inputVector->x;
	newPos.y += inputVector->y;
	newPos.z += inputVector->z;
	
	return !(newPos.x > (gWorld.xMax - playerPtr->q.size * 0.5f)
			||
			newPos.x < (gWorld.xMin + playerPtr->q.size * 0.5f)
			||
			newPos.y > (gWorld.yMax - playerPtr->q.size * 0.5f)
			||
			newPos.y < (gWorld.yMin + playerPtr->q.size * 0.5f));
}

bool IsEnemyCloseToPlayer(Enemy* e, f32 maxDistance)
{
	if(!e) return false;
	
	Vector diff = VectorSub(&(e->transform.position), &(gPlayer.transform.position));
	f32 distance = VectorLength(&diff);
	
	return distance < maxDistance;
}

bool IsProjectileCloseToEnemy(Projectile* p, Enemy* e, f32 maxDistance)
{
	if(!p || !e) return false;
	
	Vector diff = VectorSub(&(e->transform.position), &(p->transform.position));
	f32 distance = VectorLength(&diff);
	
	return distance < maxDistance;
}

u32 GetLastAvailableEnemy()
{
	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		if(!gEnemies[i].bEnabled)
		{
			return i;
		}
	}
	
	return (u32) - 1;
}

u32 GetEnemyCloseToProjectile(Projectile* p)
{
	if(!p) return (u32) - 1;

	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		if(gEnemies[i].bEnabled)
		{
			if(IsProjectileCloseToEnemy(p, &gEnemies[i], gEnemies[i].q.size * 0.5f))
			{
				return i;
			}
		}
	}
	return (u32) - 1;
}

void UpdateGame(f32 deltaTime)
{
	//Update player
	Vector dir = GetDirection(&gPlayer.transform);
	dir = VectorMulf32(&dir, deltaTime * gPlayer.movementSpeed * gInput.stickY);
	if(CanMove(&gPlayer, &dir))
	{
		Translatev(&gPlayer.transform, dir);
	}
	Rotate1f32(&gPlayer.transform, gInput.stickX * deltaTime * gPlayer.rotateSpeed);
	
	//Update camera
	gCamera.transform.position = gPlayer.transform.position;
	//gCamera.transform.position.x = max(min(gCamera.transform.position.x, gWorld.maxCameraPosition.x), gWorld.minCameraPosition.x);
	//gCamera.transform.position.y = max(min(gCamera.transform.position.y, gWorld.maxCameraPosition.y), gWorld.minCameraPosition.y);
	VectorClamp(&gCamera.transform.position, &gWorld.minCameraPosition, &gWorld.maxCameraPosition);
	gCamera.transform.position.z = gCamera.zOffsetToPlayer;
	guMtxIdentity(gMatrices.view);
	Vector lookAt = gCamera.transform.position;
	lookAt.z += 1.0f;
	guLookAt(gMatrices.view, &gCamera.transform.position, &YAxis, &lookAt);
	
	//Update projectiles
	for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		if(gProjectiles[i].bEnabled)
		{
			Vector dir = GetDirection(&gProjectiles[i].transform);
			Translatev(&gProjectiles[i].transform, VectorMulf32(&dir, deltaTime * gProjectiles[i].speed));
			u32 hitEnemy = GetEnemyCloseToProjectile(&gProjectiles[i]);
			if(hitEnemy < ENEMY_POOL_COUNT)
			{
				gEnemies[hitEnemy].bEnabled = false;
			}
		}
	}
		
	CheckProjectiles();
	
	//Update enemy timer
	gEnemyTimer += deltaTime;
	u32 lastAvailableEnemy = GetLastAvailableEnemy();
	if(gEnemyTimer > ENEMY_SPAWN_TIME && lastAvailableEnemy < ENEMY_POOL_COUNT)
	{
		SpawnEnemyRandom(&(gEnemies[lastAvailableEnemy]), gScreenParams.xExtent + gCamera.transform.position.x, gScreenParams.yExtent + gCamera.transform.position.y);
		gEnemyTimer = 0.0f;
	}
	
	//Update enemies
	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		if(gEnemies[i].bEnabled)
		{
			SetEnemyDirection(&(gEnemies[i].transform), &(gPlayer.transform));
			Vector dir = GetDirection(&gEnemies[i].transform);
			Translatev(&gEnemies[i].transform, VectorMulf32(&dir, gEnemies[i].movementSpeed * deltaTime));
			
			if(IsEnemyCloseToPlayer(&gEnemies[i], gPlayer.q.size * 0.5f))
			{
				gPlayer.lives -= 1;
				gEnemies[i].bEnabled = false;
			}
		}
	}
	
	//Update shooting projectiles
	gShootProjectileTimer += deltaTime;
	if(BTN_PRESSED(PAD_BUTTON_A) && gShootProjectileTimer > 0.1f && gLastAvailableProjectile < PROJECTILES_COUNT)
	{
		gShootProjectileTimer = 0.0f;
		ShootProjectile(&(gProjectiles[gLastAvailableProjectile]), &(gPlayer.transform), &(gPlayer.q), gPlayer.movementSpeed * 2.0f);
		gLastAvailableProjectile += 1;
	}
	
	//Should player be dead already?
	if(gPlayer.lives <= 0)
	{
		gGameState = GS_GAMEOVER;
		ResetGameState();
	}
}

void UpdateMenu(f32 deltaTime)
{
	if(BTN_PRESSED(PAD_BUTTON_A))
	{
		ResetPlayer(&gPlayer);
		gGameState = GS_GAME;
	}
}

void UpdateGameOver(f32 deltaTime)
{
	
	if(BTN_PRESSED(PAD_BUTTON_A))
	{
		gGameState = GS_MENU;
	}
}

void DrawGame(Mtx* modelView)
{
	if(!modelView) return;
	
	for(u32 i = 0; i < 64; ++i)
	{
		PrepareMatrix(modelView, &gFloors[i].transform, &gMatrices.view);
		GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
		DRAW_QUAD_SPRITE(gFloors[i].q, gFloors[i].sprite);
	}
	
	for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		if(gProjectiles[i].bEnabled)
		{	
			PrepareMatrix(modelView, &gProjectiles[i].transform, &gMatrices.view);
			GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
			DRAW_QUAD_SPRITE(gProjectiles[i].q, gProjectiles[i].sprite);
		}
	}
	
	for(u32 i = 0; i < ENEMY_POOL_COUNT; ++i)
	{
		if(gEnemies[i].bEnabled)
		{	
			PrepareMatrix(modelView, &gEnemies[i].transform, &gMatrices.view);
			GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
			DRAW_QUAD_SPRITE(gEnemies[i].q, gEnemies[i].sprite);
		}
	}
	
	PrepareMatrix(modelView, &gPlayer.transform, &gMatrices.view);
	GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
	DRAW_QUAD_SPRITE(gPlayer.q, gPlayer.sprite);
	
	Mtx identity;
	guMtxIdentity(identity);
	
	for(u32 i = 0; i < gPlayer.lives; ++i)
	{
		PrepareMatrix(modelView, &gLives[i].transform, &gMatrices.viewGUI);
		GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
		DRAW_QUAD_SPRITE(gLives[i].q, gLives[i].sprite);
	}
}

void DrawMenu(Mtx* modelView)
{
	PrepareMatrix(modelView, &gMainMenu.transform, &gMatrices.viewGUI);
	GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
	DRAW_QUAD_SPRITE(gMainMenu.q, gMainMenu.sprite);	
}

void DrawGameOver(Mtx* modelView)
{
	PrepareMatrix(modelView, &gGameOver.transform, &gMatrices.viewGUI);
	GX_LoadPosMtxImm(*modelView, GX_PNMTX0);
	DRAW_QUAD_SPRITE(gGameOver.q, gGameOver.sprite);	
}