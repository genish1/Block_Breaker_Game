#include "G2Dfull.h"

#include "GlutImport.h"

/////////////////////////////////////////////////////////////
//
//	    Mouse
//
/////////////////////////////////////////////////////////////


bool mousePressed[16];
bool mouseClicked[16];
int  MouseX = 0;
int  MouseY = 0;


void G2D::getMousePos(int& x, int& y)
{
	x = MouseX;
	y = MouseY;
}


bool G2D::detectLeftClick()
{
	bool r = mouseClicked[GLUT_LEFT_BUTTON];
	mouseClicked[GLUT_LEFT_BUTTON] = false; // on efface l'ev click
	return r;
}


bool G2D::detectRightClick()
{
	bool r = mouseClicked[GLUT_RIGHT_BUTTON];
	mouseClicked[GLUT_RIGHT_BUTTON] = false; // on efface l'ev click
	return r;
}


bool G2D::isAnyMouseButtonPressed()
{
	return mousePressed[0] | mousePressed[1] | mousePressed[2];
}

bool G2D::isMouseLeftButtonPressed()
{
	return mousePressed[GLUT_LEFT_BUTTON];
}

bool G2D::isMouseRightButtonPressed()
{
	return mousePressed[GLUT_RIGHT_BUTTON];
}

void _G2D::mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) { mousePressed[button] = true;   mouseClicked[button] = true; }
	if (state == GLUT_UP)   { mousePressed[button] = false;  mouseClicked[button] = false; }
}




void _G2D::mouseMove(int x, int y)
{
	MouseX = x;
	MouseY = _G2D::getWindowHeight() - y;
}
