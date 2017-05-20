#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "includes.h"

#define PROJECTILES_COUNT 50

static void* frameBuffer[2] = { NULL, NULL };
u8 currentFB = 0;
static GXRModeObj *rmode;

void* gFIFO;

GXColor background = {0, 0, 0, 0xff};

Matrices gMatrices;
ScreenParams gScreenParams;
Input gInput;
Input gPrevInput;

Player gPlayer;
Projectile gProjectiles[PROJECTILES_COUNT];
u32 gLastAvailableProjectile;
Life gLives[MAX_LIVES];

s32 gOrthoSize = 30.0f;
f32 gShootProjectileTimer = 0.0f;

void CheckProjectiles()
{
	f32 xExtent = gOrthoSize * gScreenParams.width / gScreenParams.height;
	f32 yExtent = gOrthoSize;
	for(s32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		if(gProjectiles[i].bEnabled)
		{
			if(gProjectiles[i].transform.position.x > (0.5f * xExtent)
				||
				gProjectiles[i].transform.position.x < (0.5f * -xExtent)
				||
				gProjectiles[i].transform.position.y > (0.5f * yExtent)
				||
				gProjectiles[i].transform.position.y < (0.5f * -yExtent))
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
	
	f32 xExtent = gOrthoSize * gScreenParams.width / gScreenParams.height;
	f32 yExtent = gOrthoSize;
	
	return !(newPos.x > (0.5f * xExtent - playerPtr->q.size * 0.6f)
			||
			newPos.x < (0.5f * -xExtent + playerPtr->q.size * 0.6f)
			||
			newPos.y > (0.5f * yExtent - playerPtr->q.size * 0.6f)
			||
			newPos.y < (0.5f * -yExtent + playerPtr->q.size * 0.6f));
}

void GatherInput()
{
	static const f32 stickMultiplier = 1.0f / 127.0f;

	gPrevInput = gInput;		
	
	PAD_ScanPads();
	
	gInput.buttons = PAD_ButtonsHeld(0);
	gInput.stickX = min(1.0f, PAD_StickX(0) * stickMultiplier);
	gInput.stickY = min(1.0f, PAD_StickY(0) * stickMultiplier);
}

void Initialise()
{
	VIDEO_Init();
	PAD_Init();
 
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
	
	guVector cam = {0.0f, 0.0f, -gOrthoSize};
	guLookAt(gMatrices.view, &cam, &YAxis, &ZeroVector);
	
	gScreenParams.width = rmode->viWidth;
	gScreenParams.height = rmode->viHeight;
	f32 aspectRatio = (f32)gScreenParams.width / (f32)gScreenParams.height;
	guOrtho(gMatrices.projection, gOrthoSize / 2, -gOrthoSize / 2, aspectRatio * -gOrthoSize / 2, aspectRatio * gOrthoSize / 2, 0.1f, 300.0f);
	GX_LoadProjectionMtx(gMatrices.projection, GX_ORTHOGRAPHIC);
	
	gInput = GetInput();
	gPrevInput = gInput;
	
	GX_InvalidateTexAll();
	TPLFile tplFile;
	TPL_OpenTPLFromMemory(&tplFile, (void*)textures_tpl, textures_tpl_size);
	
	gPlayer = GetPlayer(GetQuad(2.5f, CWhite),
						&tplFile,
						player,
						GetTransform4f32(0.0f, 0.0f, 0.0f, 0.0f),
						15.0f, 
						360.0f
						);
				
	Quad projectileQuad = GetQuad(0.5f, CWhite);
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
	f32 y = gOrthoSize * 0.5f - lifeQuad.size - 0.25f;
	f32 initX = gOrthoSize * gScreenParams.width / gScreenParams.height * 0.5f - lifeQuad.size;
	f32 xOffset = -lifeQuad.size;
	for(u32 i = 0; i < MAX_LIVES; ++i)
	{
		gLives[i] = GetLife(lifeQuad,
							&tplFile,
							life,
							GetTransform4f32(initX + xOffset * i, y, 0.0f, 0.0f)
							);
	}
}

void Update(float deltaTime)
{ 
	//Update projectiles
	for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
	{
		if(gProjectiles[i].bEnabled)
		{
			Vector dir = GetDirection(&gProjectiles[i].transform);
			Translatev(&gProjectiles[i].transform, VectorMulf32(&dir, deltaTime * gProjectiles[i].speed));
		}
	}
		
	CheckProjectiles();
	
	//Update player
	Vector dir = GetDirection(&gPlayer.transform);
	dir = VectorMulf32(&dir, deltaTime * gPlayer.movementSpeed * gInput.stickY);
	if(CanMove(&gPlayer, &dir))
	{
		Translatev(&gPlayer.transform, dir);
	}
	Rotate1f32(&gPlayer.transform, gInput.stickX * deltaTime * gPlayer.rotateSpeed);
	
	//Update shooting projectiles
	gShootProjectileTimer += deltaTime;
	if(BTN_PRESSED(PAD_BUTTON_A) && gShootProjectileTimer > 0.1f && gLastAvailableProjectile < PROJECTILES_COUNT)
	{
		gShootProjectileTimer = 0.0f;
		ShootProjectile(&(gProjectiles[gLastAvailableProjectile]), &(gPlayer.transform), &(gPlayer.q), gPlayer.movementSpeed * 2.0f);
		gLastAvailableProjectile += 1;
	}
}

int main()
{
	Initialise();
	
	Mtx modelView;
	
	f32 deltaTime = 1.0f / 60.0f;
	
	while(1)
	{
		GatherInput();
		
		Update(deltaTime);
		
		//Temporary
		if(BTN_PRESSED(PAD_BUTTON_B))
		{
			gPlayer.lives -= 1;
		}
		
		GX_InvalidateTexAll();
		GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	
		PrepareMatrix(&modelView, &gPlayer.transform, &gMatrices.view);
		GX_LoadPosMtxImm(modelView, GX_PNMTX0);
		
		DRAW_QUAD_SPRITE(gPlayer.q, gPlayer.sprite);
		
		for(u32 i = 0; i < PROJECTILES_COUNT; ++i)
		{
			if(gProjectiles[i].bEnabled)
			{	
				PrepareMatrix(&modelView, &gProjectiles[i].transform, &gMatrices.view);
				GX_LoadPosMtxImm(modelView, GX_PNMTX0);
				DRAW_QUAD_SPRITE(gProjectiles[i].q, gProjectiles[i].sprite);
			}
		}
		
		for(u32 i = 0; i < gPlayer.lives; ++i)
		{
			PrepareMatrix(&modelView, &gLives[i].transform, &gMatrices.view);
				GX_LoadPosMtxImm(modelView, GX_PNMTX0);
				DRAW_QUAD_SPRITE(gLives[i].q, gLives[i].sprite);
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
