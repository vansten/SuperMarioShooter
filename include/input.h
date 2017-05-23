#ifndef INPUT_H
#define INPUT_H

#include <gccore.h>
#include <ogc\gx.h>

#define BTN_PRESSED(button) ((gInput.buttons & button) != 0 && (gPrevInput.buttons & button) == 0)
#define BTN_RELEASED(button) ((gInput.buttons & button) == 0 && (gPrevInput.buttons & button) != 0)
#define BTN_HELD(button) ((gInput.buttons & button) != 0)

typedef struct
{
	u16 buttons;
	f32 stickX;
	f32 stickY;
} Input;

Input GetInput()
{
	Input i;
	i.buttons = 0;
	i.stickX = 0;
	i.stickY = 0;
	return i;
}

void InitInput(Input* input)
{
	PAD_Init();
	
	if(!input) return;
	
	*input = GetInput();
}

void GatherInput(Input* input)
{
	static const f32 stickMultiplier = 1.0f / 127.0f;
	
	if(!input) return;
	
	PAD_ScanPads();
	
	input->buttons = PAD_ButtonsHeld(0);
	input->stickX = min(1.0f, PAD_StickX(0) * stickMultiplier);
	input->stickY = min(1.0f, PAD_StickY(0) * stickMultiplier);
}

#endif