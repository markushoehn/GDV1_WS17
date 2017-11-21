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

#include <stdlib.h>       // namespace std
#include <GL/glut.h>      // openGL helper
#include "Vec3.h"         // basic vector arithmetic class (embedded in std::)
#include "TriangleMesh.h" // simple class for reading and rendering triangle meshes

using namespace std;

// ===================
// === GLOBAL DATA ===
// ===================

// scene Information
Vec3f centerPos;
float angleX, angleY;
// light information
Vec3f lightPos;
float lightMotionSpeed;
bool moveLight;
// mouse information
int mouseX, mouseY, mouseButton;
float mouseSensitivy;
// object
TriangleMesh trimesh;

// ==============
// === BASICS ===
// ==============

int main(int argc, char **argv);

void setDefaults();

void initialize();

void reshape(GLint width, GLint height);

void processTimedEvent(int x);

// =================
// === RENDERING ===
// =================

void drawCS();

void drawLight();

void renderScene(void);

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y);

void mousePressed(int button, int state, int x, int y);

void mouseMoved(int x, int y);

// ===============
// === VARIOUS ===
// ===============

void coutHelp();
