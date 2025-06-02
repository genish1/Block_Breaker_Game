#pragma once

#include "G2D.h"
#include "GlutImport.h"
#include <map>

using namespace std;

namespace _G2D  // internal data of G2D
{
	// Key management
 
	void resetKeyPressed();
	void initKeys();
	void keyboard(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);
	void keySpecial(int key, int x, int y);
	void keySpecialUp(int key, int x, int y);
 

	// Mouse management
	void mouseMove(int x, int y);
	void mouse(int button, int state, int x, int y);

	// Window size
	int getWindowWidth();
	int getWindowHeight();

};