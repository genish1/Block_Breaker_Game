#include "G2Dfull.h"

#include <map>

/////////////////////////////////////////////////////////////
//
//	    Key Management
//
/////////////////////////////////////////////////////////////

map<Key, bool>  activeKeys;     // les touches en cours d'appui
map<Key, bool>  keyHit;         // ev keyPressed
bool            isPaused = false;
Key keyTable[256];
Key specialKeyTable[256];



void _G2D::initKeys()
{
	for (int i = 0; i < 256; i++) keyTable[i] = specialKeyTable[i] = Key::Unknown;

	keyTable['A'] = keyTable['a'] = Key::A;
	keyTable['B'] = keyTable['b'] = Key::B;
	keyTable['C'] = keyTable['c'] = Key::C;
	keyTable['D'] = keyTable['d'] = Key::D;
	keyTable['E'] = keyTable['e'] = Key::E;
	keyTable['F'] = keyTable['f'] = Key::F;
	keyTable['G'] = keyTable['g'] = Key::G;
	keyTable['H'] = keyTable['h'] = Key::H;
	keyTable['I'] = keyTable['i'] = Key::I;
	keyTable['J'] = keyTable['j'] = Key::J;
	keyTable['K'] = keyTable['k'] = Key::K;
	keyTable['L'] = keyTable['l'] = Key::L;
	keyTable['M'] = keyTable['m'] = Key::M;
	keyTable['N'] = keyTable['n'] = Key::N;
	keyTable['O'] = keyTable['o'] = Key::O;
	keyTable['P'] = keyTable['p'] = Key::P;
	keyTable['Q'] = keyTable['q'] = Key::Q;
	keyTable['R'] = keyTable['r'] = Key::R;
	keyTable['S'] = keyTable['s'] = Key::S;
	keyTable['T'] = keyTable['t'] = Key::T;
	keyTable['U'] = keyTable['u'] = Key::U;
	keyTable['V'] = keyTable['v'] = Key::V;
	keyTable['W'] = keyTable['w'] = Key::W;
	keyTable['X'] = keyTable['x'] = Key::X;
	keyTable['Y'] = keyTable['y'] = Key::Y;
	keyTable['Z'] = keyTable['z'] = Key::Z;
	keyTable['\r'] = Key::ENTER;

	specialKeyTable[GLUT_KEY_LEFT] = Key::LEFT;
	specialKeyTable[GLUT_KEY_DOWN] = Key::DOWN;
	specialKeyTable[GLUT_KEY_RIGHT] = Key::RIGHT;
	specialKeyTable[GLUT_KEY_UP] = Key::UP;
	specialKeyTable[GLUT_KEY_F1] = Key::F1;
	specialKeyTable[GLUT_KEY_F2] = Key::F2;
	specialKeyTable[GLUT_KEY_F3] = Key::F3;
	specialKeyTable[GLUT_KEY_F4] = Key::F4;
	specialKeyTable[GLUT_KEY_F5] = Key::F5;
	specialKeyTable[GLUT_KEY_F6] = Key::F6;
	specialKeyTable[GLUT_KEY_F7] = Key::F7;
	specialKeyTable[GLUT_KEY_F8] = Key::F8;
	specialKeyTable[GLUT_KEY_F9] = Key::F9;
	specialKeyTable[GLUT_KEY_F10] = Key::F10;
	specialKeyTable[GLUT_KEY_F11] = Key::F11;
	specialKeyTable[GLUT_KEY_F12] = Key::F12;
}

 
void startPause();
void endPause();

void _G2D::keyboard(unsigned char key, int x, int y)
{
	if (key == 0x1b) exit(0);
	activeKeys[keyTable[key]] = true;
	keyHit[keyTable[key]] = true;

	if (keyTable[key] == Key::P)
		if (isPaused)
		{
			endPause();
			isPaused = false;
		}
		else
		{
			startPause();
			isPaused = true;
		}
}

void _G2D::keyUp(unsigned char key, int x, int y)
{ 
	activeKeys[keyTable[key]] = false;
}


void _G2D::keySpecial(int key, int x, int y)   { activeKeys[specialKeyTable[key]] = true; }
void _G2D::keySpecialUp(int key, int x, int y) { activeKeys[specialKeyTable[key]] = false; }


///////////////////////////////////////////////////////////////
//
//  exportées dans G2D


bool G2D::isOnPause()
{
	return isPaused;
}



bool G2D::isKeyPressed(Key k)
{
	if ( activeKeys.find(k) == activeKeys.end())      return false;
	return activeKeys[k];
}

bool G2D::keyHasBeenHit(Key k)
{
	if (keyHit.find(k) == keyHit.end())      return false;
	bool hit = keyHit[k];
	keyHit[k] = false;
	return hit;
}

