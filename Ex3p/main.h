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
#include <GL/glew.h>      // openGL helper
#include <GL/glut.h>      // openGL helper
#include "Vec3.h"         // basic vector arithmetic class (embedded in std::)
#include "TriangleMesh.h" // simple class for reading and rendering triangle meshes
#include "imageloader.h"  // loads bmp files

using namespace std;

// ===================
// === GLOBAL DATA ===
// ===================

// camera Information
Vec3f cameraPos;
Vec3f cameraDir;
float angleX, angleY, movementSpeed;
// light information
Vec3f lightPos;
float lightMotionSpeed;
bool moveLight;
// mouse information
int mouseX, mouseY, mouseButton;
float mouseSensitivy;
// key pressed information
vector<bool> keyDown;
// object
unsigned int objectsLastRun, trianglesLastRun;
vector<TriangleMesh> meshes;
// draw mode
unsigned int drawMode;
// skybox texture IDs
vector<GLuint> skyboxTextureIDs;

vector<GLuint> textureIDs;

// ==============
// === BASICS ===
// ==============

int main(int argc, char **argv);

void initialize();

void setDefaults();

void reshape(GLint width, GLint height);

void processTimedEvent(int x);

// =================
// === RENDERING ===
// =================

void drawCS();

void drawLight();

void drawSkybox();

void renderScene(void);

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y);

void keyReleased(unsigned char key, int x, int y);

void mousePressed(int button, int state, int x, int y);

void mouseMoved(int x, int y);

// ===============
// === VARIOUS ===
// ===============

void coutHelp();
