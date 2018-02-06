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

#ifndef MAIN_H
#define MAIN_H

#include "Vec3.h"         // basic vector arithmetic class (embedded in std::)
#include "TriangleMesh.h" // simple class for reading and rendering triangle meshes
#include "Ray.h"          // class for ray intersection with triangles

using namespace std;

// ===================
// === GLOBAL DATA ===
// ===================

// skybox texture IDs
vector<GLuint> skyboxTextureIDs;


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
// objects
vector<TriangleMesh> meshes;
vector<GLuint> textureIDs;
struct SceneObject {
  float matAmbient[4];
  float matDiffuse[4];
  float matSpecular[4];
  float matReflect[4];
  float matOpacity[4];
  float matShininess;
  float matRefract;
  GLuint textureID;
};
vector<SceneObject> objects;
// draw mode
unsigned int drawMode;
// raytraced texture
GLuint raytracedTextureID;
unsigned int intersectionTests;

// light components for opengl and raytracing
float ambientLight[4];
float diffuseLight[4];
float specularLight[4];

// ==============
// === BASICS ===
// ==============

int main(int argc, char **argv);

void initialize();

void setDefaults();

void reshape(const GLint width, const GLint height);

void processTimedEvent(const int x);

// =================
// === RENDERING ===
// =================

void drawCS();

void drawLight();

void renderScene(void);

Vec3f calculateIntensity(Ray<float> ray, unsigned int& hits, int depth, bool verbose);

void raytrace();

int intersectRayObjectsEarliest(const Ray<float> &ray, float &t, float &u, float &v, unsigned int &hitTri);

bool rayAABBIntersect(const Ray<float> &r, const Vec3f& vmin, const Vec3f& vmax, float t0, float t1);

// =================
// === CALLBACKS ===
// =================

void keyPressed(const unsigned char key, const int x, const int y);

void keyReleased(const unsigned char key, const int x, const int y);

void mousePressed(const int button, const int state, const int x, const int y);

void mouseMoved(const int x, const int y);

// ===============
// === VARIOUS ===
// ===============

void coutHelp();

#endif
