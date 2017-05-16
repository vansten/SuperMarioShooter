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

#endif