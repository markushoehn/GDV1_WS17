// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple and extendable openGL program offering                    //
//   * basic navigation and basic pipeline rendering                         //
// ========================================================================= //

#include <math.h>         // fmod
#include <stdio.h>        // cout
#include <iostream>       // cout
#include "main.h"         // this header
#include <algorithm>

// ==============
// === BASICS ===
// ==============

int main(int argc, char** argv) {
	// initialize openGL window
	glutInit(&argc, argv);
	glutInitWindowPosition(300,200);
	glutInitWindowSize(600,400);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("TU Darmstadt, GDV1, OpenGL P1");
	// link functions to certain openGL events
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutMouseFunc(mousePressed);
	glutMotionFunc(mouseMoved);
	glutKeyboardFunc(keyPressed);
	// further initializations
	setDefaults();
	initialize();

	// load mesh // TODO: enter correct filename and Load OFF or LSA.
    char* filename = "Modelle/delphin.off"; 
    trimesh.loadOFF(filename); 

	// activate main loop
	glutTimerFunc(20, processTimedEvent, 0);
	glutMainLoop();
	return 0;
}

void setDefaults() {
	// scene Information
	centerPos.set(1.0f, -2.0f, -5.0f);
	angleX = 0.0f;
	angleY = 0.0f;
	// light information
	lightPos.set(-10.0f, 0.0f, 0.0f);
	lightMotionSpeed = 2.0f;
	moveLight = false;
	// mouse information
	mouseX = 0;
	mouseY = 0;
	mouseButton = 0;
	mouseSensitivy = 1.0f;
	// object
}

void initialize() {
	// black screen
	glClearColor(0,0,0,0);
	// enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// set shading model
	glShadeModel(GL_SMOOTH);
	// set lighting and material
	GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };	
	GLfloat ambientLight[] =   { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] =   { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat shininess = 128.0f;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightf(GL_LIGHT0, GL_SHININESS, shininess);
	glEnable(GL_LIGHT0);
	// enable use of glColor instead of glMaterial for ambient and diffuse property
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    // white shiny specular highlights
    GLfloat specularLightMaterial[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininessMaterial = 128.0f;
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininessMaterial);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, specularLightMaterial);

	// keybindings => cout
	coutHelp();
}

void reshape(GLint width, GLint height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (float)width / (float)height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void processTimedEvent(int x) {
	// start event again in 20ms
	glutTimerFunc(20 , processTimedEvent, x+20);
	// perform light movement and trigger redisplay
	if (moveLight == true) {
		lightPos.rotY(lightMotionSpeed);
		glutPostRedisplay();
	}
}

// =================
// === RENDERING ===
// =================

void drawCS() {
	glBegin(GL_LINES);
	// red X
	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(5,0,0);
	// green Y
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,5,0);
	// blue Z
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,5);
	glEnd();
}

void drawLight() {
  // set light position in within current coordinate system
	GLfloat lp[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lp);
	// draw yellow sphere for light source
	glPushMatrix();
	glTranslatef(lp[0], lp[1], lp[2]);
	glColor3f(1,1,0);
	glutSolidSphere(0.1f,64,64);
	glPopMatrix();
}

void renderScene() {
	// clear and set camera
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// translate to centerPos
	glTranslatef(centerPos.x, centerPos.y, centerPos.z);
	// draw coordinate system without lighting
	glDisable(GL_LIGHTING);
	// rotate scene
	glRotatef(angleX,0.0f,1.0f,0.0f);
	glRotatef(angleY,1.0f,0.0f,0.0f);
	drawCS();
	// draw sphere for light still without lighting
	drawLight();
	// draw objects
	glEnable(GL_LIGHTING);
	glColor3f(0.2,0.5,0.8);
	trimesh.draw();
  // swap buffers
	glutSwapBuffers();
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y) {
	switch (key) {
	// esc => exit
	case 27:
		exit(0);
		break;
		// help file
	case 'h' :
	case 'H' :
		coutHelp();
		break;
		// reset view
	case 'r' :
	case 'R' :
		centerPos.set(0.0f, -2.0f, -5.0f);
		angleX = 0.0f;
		angleY = 0.0f;
		glutPostRedisplay();
		break;
		// Light movement
	case 'l' :
	case 'L' :
		moveLight = !moveLight;
		break;
	}
}

void mousePressed(int button, int state, int x, int y) {
	mouseButton = button;
	mouseX = x;
	mouseY = y;
}

void mouseMoved(int x, int y) {
	// rotate 
	if (mouseButton == GLUT_LEFT_BUTTON) {
		angleX = fmod(angleX + (x-mouseX)*mouseSensitivy,360.0f);
		angleY += (y-mouseY)*mouseSensitivy;
		glutPostRedisplay();
	}
	// zoom (here translation in z)
	if (mouseButton == GLUT_RIGHT_BUTTON) {
		centerPos.z -= 0.2f*(y-mouseY)*mouseSensitivy;
		glutPostRedisplay();
	}
	// translation in xy
	if (mouseButton == GLUT_MIDDLE_BUTTON) {
		centerPos.x += 0.2f*(x-mouseX)*mouseSensitivy;
		centerPos.y -= 0.2f*(y-mouseY)*mouseSensitivy;
		glutPostRedisplay();
	}
	// update mouse for next relative movement
	mouseX = x;
	mouseY = y;
}

// ===============
// === VARIOUS ===
// ===============

void coutHelp()
{
	cout << endl;
	cout << "====== KEY BINDINGS ======" << endl;
	cout << "ESC: exit" << endl;
	cout << "H: show this (H)elp file" << endl;
	cout << "R: (R)eset view" << endl;
	cout << "L: toggle (L)ight movement" << endl;
	cout << "==========================" << endl;
	cout << endl;
}
