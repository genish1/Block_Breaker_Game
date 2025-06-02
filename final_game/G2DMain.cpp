#include "G2Dfull.h"

#include <string>
#include <map>
#include <vector>
#include <chrono>
 

using namespace std;

/////////////////////////////////////////////////////////////
//
//	    Init   Window
//
/////////////////////////////////////////////////////////////



int Wwidth, Wheight;
int _G2D::getWindowWidth()    { return Wwidth;  }
int _G2D::getWindowHeight()   { return Wheight; }


void InitColors();

void G2D::initWindow(V2 WindowSize, V2 WindowStartPos, std::string name)
{
	int argcc = 3;
	
	char  arg0[] = "programName";
	char  arg1[] = "arg";
	char  arg2[] = "another arg";
	char* argc[] = { arg0, arg1, arg2, NULL };

	glutInit(&argcc, argc);

	Wwidth  = WindowSize.x;
	Wheight = WindowSize.y;

	glutInitWindowPosition(WindowStartPos.x, WindowStartPos.y);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(Wwidth, Wheight);
	glutCreateWindow(name.c_str());


	InitColors();
}

// détecte un changement de taille de la fenêtre et remet la taille initiale
void reshape(int w, int h)   
{
	glutReshapeWindow(Wwidth, Wheight);
}

/////////////////////////////////////////////////////////////
//
//	   Gestion de l'animation et des FPS
//
/////////////////////////////////////////////////////////////




void(*_LogicCallBack)(GameData & G);
void(*_RenderCallBack)(const GameData & G);
GameData * myGameData;
int _nbCalltoLogicPerSec = 10;

typedef std::chrono::high_resolution_clock Time;
auto T0             = Time::now();


int  frameNumber = 0;
bool firstCall   = true;

 

void anim(int value)
{
	if (firstCall)
	{
		firstCall = false;
		T0 = Time::now();
	}
	double Tms = G2D::elapsedTimeFromStartSeconds();

	double dt = 1.0 / _nbCalltoLogicPerSec;
	 
	if ( ! G2D::isOnPause() )
	{ 
		while ( (dt * frameNumber) < Tms)
		{ 
			_LogicCallBack(*myGameData);
			frameNumber++;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1, anim, value);	
	
}

auto   T0Pause = Time::now();
double timeSpentInPause = 0;  

double deltaTimeMS(std::chrono::steady_clock::time_point now, 
						  std::chrono::steady_clock::time_point start)
{
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<double> duration;

	duration fs = now - start;
	ms d = std::chrono::duration_cast<ms>(fs);
	double Tms = d.count() * 0.001;

	return Tms;
}
 
double G2D::elapsedTimeFromStartSeconds()
{
	double Tms = deltaTimeMS(Time::now(),T0);
	return Tms - timeSpentInPause;
}

void startPause()
{
	T0Pause = Time::now();
}

void endPause()
{
	double Tms = deltaTimeMS(Time::now(), T0Pause);

	timeSpentInPause += Tms;
}
 


/////////////////////////////////////////////////////////////
//
//	    Run Opengl Rendering
//
/////////////////////////////////////////////////////////////



void OPENGLRender()
{
	_RenderCallBack(*myGameData);
}




void G2D::Run(void logic(GameData & G), void Render(const GameData & G), GameData & G,int nbCalltoLogicPerSec,bool HideCursor)
{
	if (HideCursor)
	  glutSetCursor(GLUT_CURSOR_NONE);

	_nbCalltoLogicPerSec = nbCalltoLogicPerSec;

	glLoadIdentity();
	glOrtho(0, Wwidth, 0, Wheight, -1, 10);    // donne l'echelle x et y
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	// glEnable(GL_POLYGON_SMOOTH); à éviter rendu erroné

	// Init 	
	_G2D::initKeys();


	// callbacks
	glutReshapeFunc(reshape);
	glutKeyboardFunc(_G2D::keyboard);
	glutKeyboardUpFunc(_G2D::keyUp);
	glutSpecialFunc(_G2D::keySpecial);
	glutSpecialUpFunc(_G2D::keySpecialUp);
	glutMotionFunc(_G2D::mouseMove);
	glutPassiveMotionFunc(_G2D::mouseMove);
	glutMouseFunc(_G2D::mouse);


	// logic
	_LogicCallBack   = logic;
	_RenderCallBack  = Render;
	myGameData       = & G;
	glutDisplayFunc(OPENGLRender);      // fonction appelée lors d'un repaint
	glutTimerFunc(300, anim, 0);         // callback de timer avec id = 0, toutes les 50m
	glutMainLoop();
}


