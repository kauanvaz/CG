//*********************************************************************
//  Computacao Grafica usando OpenGL
//  Codigo Exemplo
//  Modificações: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************

/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md2Player.cpp -- Copyright (c) 2006-2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Main file of the MD2 Loader.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Md2Model.h"

#define KEY_SPACE 32
#define KEY_ESC 27
#define ANIM_RUN   11
#define ANIM_STAND 13
#define ACTION_KEY_SPACE  8 //PULAR


using namespace std;

Md2Model * player;
Md2Object * playerObj;
vector<string> animList;

// Timer
struct glut_timer_t{
    double current_time;
    double last_time;

} timer;

// Camera
struct Vector3d
{
  float x, y, z;

} rot, eye;


bool bTextured = true;
bool bLightGL = true;
bool bAnimated = true;
bool bJump = false;
bool bRun = false;

int count_frame = 0;
int renderMode = 0;
int frameRate = 5;
unsigned int animNumber = ACTION_KEY_SPACE;
int fps = 0;

void info_modotexto();

// -------------------------------------------------------------------------
// init
//
// Application initialization.  Setup keyboard input, mouse input,
// timer, camera and OpenGL.
// -------------------------------------------------------------------------

static void init (){
  // Initialize timer
  timer.current_time = 0;
  timer.last_time = 0;

  // Initialize camera input
  rot.x = 0.0f;   eye.x = 0.0f;
  rot.y = 0.0f;   eye.y = 0.0f;
  rot.z = 0.0f;   eye.z = 8.0f;

  //
  // Initialize OpenGL
  //

  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_CULL_FACE);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glCullFace (GL_BACK);
}


// -------------------------------------------------------------------------
// reshape
//
// OpenGL window resizing.  Update the viewport and the projection matrix.
// -------------------------------------------------------------------------

static void reshape (int w, int h){
    if (h == 0)
        h = 1;

    glViewport (0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (45.0, w / static_cast<GLfloat>(h), 0.1, 1000.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glutPostRedisplay ();
}


// -------------------------------------------------------------------------
// updateTimer
//
// Update the timer.
// -------------------------------------------------------------------------

static void updateTimer (struct glut_timer_t *t){
  t->last_time = t->current_time;
  t->current_time = glutGet (GLUT_ELAPSED_TIME) * 0.001f;
}

// -------------------------------------------------------------------------
// gameLogic
//
// Perform application logic.
// -------------------------------------------------------------------------

static void gameLogic (){
    // Calculate frame per seconds
    static double current_time = 0;
    static double last_time = 0;
    static int n = 0;

    n++;
    current_time = timer.current_time;

    if( (current_time - last_time) >= 1.0 ){
        fps = n;
        n = 0;
        last_time = current_time;
    }

    // Animate player
    if (bAnimated){
        double dt = timer.current_time - timer.last_time;
        playerObj->animate (frameRate * dt);
		if (bJump){
			Md2Model::AnimMap anim = player->anims();
		    count_frame = playerObj->currentFrame();			
            if(count_frame == anim[animList[animNumber]].end-1){
                bJump = false;
            }
        }
    }
}


// -------------------------------------------------------------------------
// draw3D
//
// Render the 3D part of the scene.
// -------------------------------------------------------------------------

static void draw3D (){
    // Clear window
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    // Perform camera transformations
    glTranslated (-eye.x, -eye.y, -eye.z);
    glRotated (rot.x, 1.0f, 0.0f, 0.0f);
    glRotated (rot.y, 0.0f, 1.0f, 0.0f);
    glRotated (rot.z, 0.0f, 0.0f, 1.0f);

    glEnable (GL_DEPTH_TEST);

    if (bLightGL)
        glEnable (GL_LIGHTING);

    if (bTextured)
        glEnable (GL_TEXTURE_2D);

    if(bJump){
        playerObj->setAnim(animList[animNumber]);
    }else if (bRun){
        playerObj->setAnim(animList[ANIM_RUN]);
    }else{
        playerObj->setAnim(animList[ANIM_STAND]);
    }

    // Draw objects
    playerObj->drawObjectItp(bAnimated, (Md2Object::Md2RenderMode) renderMode);


    glDisable (GL_LIGHTING);
    glDisable (GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
}




// -------------------------------------------------------------------------
// display
//
// Render the main scene to the screen.
// -------------------------------------------------------------------------

static void
display (){
  gameLogic ();

  draw3D ();

  glutSwapBuffers ();
}


// -------------------------------------------------------------------------
// keyPress
//
// Key press glut callback function.  Called when user press a key.
// -------------------------------------------------------------------------

void info_modotexto(){
    std::cout << "MENU DE AJUDA" << endl << endl;
    std::cout << "   P:    Play/Pause" << endl;
    std::cout << "   L:    Light" << endl;
    std::cout << "   S:    Solid" << endl;
    std::cout << "   T:    Textured" << endl;
    std::cout << "   W:    Wired" << endl;
    std::cout << "  -/+:   Frame Rate" << endl;
    std::cout << " SETAS:  Movimenta Modelo 3D" << endl;
	std::cout << " ESPACO: Pular" << endl;
    std::cout << endl << "INDICE DE ANIMACOES" << endl;

    Md2Model::AnimMap::const_iterator it;

    for (int i = 0; i < 16; i+=2){
        std::cout << i << " : " << animList[i] << "    " << (i+1) << " : " << animList[i+1]<< endl;
    }


}

static void keyPress (unsigned char key, int x, int y)
{
    switch(key){
        case 'p':case 'P': bAnimated = !bAnimated; break;
        case 'l':case 'L': bLightGL = !bLightGL; break;
        case 's':case 'S': glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); break;
        case 't':case 'T': bTextured = !bTextured; break;
        case 'w':case 'W': glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); break;
        case '+': frameRate++; break;
        case '-': frameRate--; break;
        case '[': animNumber--; break;
        case ']': animNumber++; break;        
        case KEY_ESC: exit(0); break;
        case KEY_SPACE: bJump = true; break;
    }
    if (frameRate < 0)
    frameRate = 0;
	if(animNumber<0) animNumber = animList.size()-1;
	if(animNumber>animList.size()-1)animNumber = 0;
    glutPostRedisplay ();
}


/*
 * Controle das teclas especiais (F1 a F12, etc...)
 */
void keyboard_special(int key, int x, int y){
    switch(key){
        case GLUT_KEY_RIGHT:
            if(rot.y > 90.0){
                rot.y -= 10.0;
            }else if(rot.y < 90.0){
                rot.y += 10.0;
            }else{
                eye.x -= 0.05*frameRate;
            }
        break;
        case GLUT_KEY_LEFT:
            if(rot.y > 270.0){
                rot.y -= 10.0;
            }else if(rot.y < 270.0){
                rot.y += 10.0;
            }else{
                eye.x += 0.05*frameRate;
            }
        break;
        case GLUT_KEY_UP:
            if(rot.y > 180.0){
                rot.y -= 10.0;
            }else if(rot.y < 180.0){
                rot.y += 10.0;
            }else{
                eye.z += 0.05*frameRate;
            }
        break;
        case GLUT_KEY_DOWN:
            if( rot.y!=0 && rot.y <= 180.0){
                rot.y -= 10.0;
            }else if ( rot.y!=0 && rot.y > 180.0){
                rot.y += 10.0;
                if(rot.y >= 360.0) rot.y = 0.0;
            }else{
                eye.z -= 0.05*frameRate;
            }
        break;

    }

    bRun = true;
    glutPostRedisplay();
}

void keyUp(int key, int x, int y){
    bRun = false;
}

// -------------------------------------------------------------------------
// idleVisible
//
// Idle glut callback function.  Continuously called. Perform background
// processing tasks.
// -------------------------------------------------------------------------

static void idleVisible (){
  // Update the timer
  updateTimer (&timer);

  if (bAnimated)
    glutPostRedisplay ();
}

// -------------------------------------------------------------------------
// windowStatus
//
// Window status glut callback function.  Called when the status of
// the window changes.
// -------------------------------------------------------------------------

static void windowStatus (int state){
    // Disable rendering and/or animation when the
    // window is not visible
    if ((state != GLUT_HIDDEN) && (state != GLUT_FULLY_COVERED)){
        glutIdleFunc (idleVisible);
    }else{
        glutIdleFunc (NULL);
    }
}



// -------------------------------------------------------------------------
// main
//
// Application main entry point.
// -------------------------------------------------------------------------

int main (int argc, char *argv[]){

    // Initialize GLUT
    glutInit (&argc, argv);

    // create an OpenGL window
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize (800, 600);
    glutCreateWindow ("Carregando Modelo 3D - Formato: MD2");

    player = new Md2Model("./data/tris.md2");
    player->loadTexture("./data/ctf_b.pcx");
    player->setTexture("./data/ctf_b.pcx");

    Md2Model::AnimMap::const_iterator it;
    for (it = player->anims().begin (); it != player->anims().end(); ++it){
        animList.push_back (it->first);
    }

    playerObj = new Md2Object (player);
    playerObj->setScale(0.1);
    playerObj->setAnim(animList[ANIM_STAND]);

    info_modotexto();
    init();

    // Setup glut callback functions
    glutKeyboardFunc (keyPress);
    glutSpecialFunc(keyboard_special);
    glutSpecialUpFunc(keyUp);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutWindowStatusFunc (windowStatus);
    glutIdleFunc (idleVisible);

    // Enter the main loop
    glutMainLoop ();

    return 0;
}
