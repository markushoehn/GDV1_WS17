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
#include <time.h>         // clock_t
#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file
#include "main.h"         // this header
#include "terrain.h"      // our fancy terrain
#include <algorithm>
 
// ===============
// === TERRAIN ===
// ===============

void terrain(){
  Terrain x = Terrain(128, 0.12345678f);
  x.generate(0, true);
  x.visualize();
}

// ==============
// === BASICS ===
// ==============

int main(int argc, char** argv) {

  // initialize openGL window
  glutInit(&argc, argv);
  glutInitWindowPosition(300,200);
  glutInitWindowSize(600,400);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("OpenGL Terrain Texture Culling"); 
  // link functions to certain openGL events
  glutDisplayFunc(renderScene);
  glutReshapeFunc(reshape);  
  glutMouseFunc(mousePressed);
  glutMotionFunc(mouseMoved);
  glutKeyboardFunc(keyPressed);
  glutKeyboardUpFunc(keyReleased);
  // further initializations
  initialize();
  setDefaults();  



  // set flag for build dir
  bool build = false;
  string prefix = ".";
  if(build) prefix = "..";


  // generate terrain from random heightmap
  TriangleMesh terr;
  terr.generateHeightmap();
  terr.toggleWithColorArray();
  meshes.push_back(terr);
  // load doppeldecker: mesh[0] (flies in positive z direction)
  TriangleMesh doppeldecker;
  string filename = prefix + "/Models/doppeldecker.off";
  doppeldecker.loadOFF(filename.c_str(), Vec3f(0.0f,1.0f,0.0f), 5.0f);
  // see loading of plane texture below
  doppeldecker.textureID = 0;
  meshes.push_back(doppeldecker);
  
  // second plane
  TriangleMesh doppeldecker2(doppeldecker);
  doppeldecker2.translateToCenter(Vec3f(5.0f,0.0f,0.0f), true);
  doppeldecker2.textureID = -1;
  meshes.push_back(doppeldecker2);
  
  // third plane
  TriangleMesh doppeldecker3(doppeldecker);
  doppeldecker3.translateToCenter(Vec3f(5.0f,0.0f,5.0f), true);
  doppeldecker3.textureID = 1;
  meshes.push_back(doppeldecker3);
  
  float dim = 50.0f;
  float dist = 5.5f;
  // add some more planes
  for (float x = -dim/2.0f; x < dim/2.0f; x = x + dist){
    for (float y = -dim/2.0f; y < dim/2.0f; y = y + dist){
      for (float z = -dim/2.0f; z < dim/2.0f; z = z + dist){
        TriangleMesh doppeldeckerI(doppeldecker);
        doppeldeckerI.translateToCenter(Vec3f(x, y, z), true);
        doppeldeckerI.textureID = 0;
        meshes.push_back(doppeldeckerI);
      }
    }
  }
  
  // cout mesh data
  for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].coutData();

  // load sky box-
  string names [] = {"neg_z.bmp", "pos_x.bmp", "pos_z.bmp", "neg_x.bmp", "pos_y.bmp", "neg_y.bmp"};
  string sbox = "skybox2";
  Image * image;
  for(int i = 0; i < 6; ++i)
  {
    filename = prefix + "/Textures/" + sbox + "/" +names[i];
    image = loadBMP(filename.c_str());
    skyboxTextureIDs[i] = loadTexture(image);
  }

  // Plane texture
  filename = prefix + "/Textures/TEST_GRID.bmp";
  image = loadBMP(filename.c_str());
  textureIDs[0] = loadTexture(image);
  textureIDs[1] = skyboxTextureIDs[0];

  // free memory
  delete image;
  // activate main loop
  coutHelp();
  glutTimerFunc(20, processTimedEvent, clock());
  glutMainLoop();
  return 0;
}

void initialize() {
  // black screen
  glClearColor(0,0,0,0);
  // initialize glew
  GLenum err = glewInit();
  if (GLEW_OK != err) cout << "GLEW Error (" << err << "): " << glewGetErrorString(err) << endl;
  cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << endl;
  // check if VBOs are supported
  if(glGenBuffers && glBindBuffer && glBufferData && glBufferSubData &&
    glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv)
    cout << "VBOs are supported!" << endl;
  else cout << "VBOs are NOT supported!" << endl;
  // set shading model
  glShadeModel(GL_SMOOTH);
  // set lighting (white light)
  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };  
  GLfloat ambientLight[] =   { 0.1f, 0.1f, 0.1f, 1.0f };
  GLfloat diffuseLight[] =   { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat specularLight[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat shininess = 0.8f * 128.0f;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightf(GL_LIGHT0, GL_SHININESS, shininess);
  glEnable(GL_LIGHT0);
  // enable use of glColor instead of glMaterial
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  // sky box texture IDs (6 IDs, initialized with 0)
  skyboxTextureIDs.resize(6,0);
  textureIDs.resize(2,0);
}

void setDefaults() {
  // scene Information
  cameraPos.clear();
  cameraDir.set(0,0,-1);
  movementSpeed = 0.02f;
  // light information
  lightPos.set(0.0f, 1000.0f, 2000.0f);
  lightMotionSpeed = 0.02f;
  moveLight = false;
  // mouse informations
  mouseX = 0;
  mouseY = 0;
  mouseButton = 0;
  mouseSensitivy = 1.0f;
  // key information, all false
  keyDown.resize(255,false);
  // draw mode (VBO)
  drawMode = 0;
  // last run: 0 objects and 0 triangles
  objectsLastRun = 0;
  trianglesLastRun = 0;
}

void reshape(GLint width, GLint height) {
  float angle = 65.0;
  float farDist = 1000.0;
  float nearDist = 0.5;
  float ratio = (float)width / (float)height;

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(angle, ratio, nearDist, farDist);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // view frustum
  frustum = ViewFrustum(angle, ratio, nearDist, farDist);
}

void processTimedEvent(int x) {
  // Time now. int x is time of last run
  clock_t clock_this_run = clock();
  int msPassed = (clock_this_run - x)*1000/CLOCKS_PER_SEC;
  // perform light movement and trigger redisplay
  if (moveLight == true) {
	  lightPos.rotY(lightMotionSpeed*msPassed);
	  glutPostRedisplay();
  }
  // first person movement
  if (keyDown[(int)'w'] || keyDown[(int)'W']) { 
    cameraPos += movementSpeed*cameraDir*(float)msPassed; 
    glutPostRedisplay();
  }
  if (keyDown[(int)'s'] || keyDown[(int)'S']) { 
    cameraPos -= movementSpeed*cameraDir*(float)msPassed; 
    glutPostRedisplay();
  }
  if (keyDown[(int)'a'] || keyDown[(int)'A']) {
    Vec3f ortho(-cameraDir.z,0.0f,cameraDir.x);
    ortho.normalize();
    cameraPos -= movementSpeed*ortho*(float)msPassed; 
    glutPostRedisplay();
  }
  if (keyDown[(int)'d'] || keyDown[(int)'D']) {
    Vec3f ortho(-cameraDir.z,0.0f,cameraDir.x);
    ortho.normalize();
    cameraPos += movementSpeed*ortho*(float)msPassed; 
    glutPostRedisplay();
  }
  if (keyDown[(int)'q'] || keyDown[(int)'Q']) {
    cameraPos.y += movementSpeed*(float)msPassed; 
    glutPostRedisplay();
  }
  if (keyDown[(int)'e'] || keyDown[(int)'E']) {
    cameraPos.y -= movementSpeed*(float)msPassed; 
    glutPostRedisplay();
  }
  // start event again
  glutTimerFunc(std::max(1,20-msPassed), processTimedEvent, clock_this_run);
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

void drawSkybox() {
  // prepare (no lighting, no depth test, texture filter)
  glEnable(GL_TEXTURE_2D);
  glColor3f(1,1,1);
  // draw skybox
  
  float pWidth = 2;
  float pLength = 2;
  float pHeight = 2;
  float px = cameraPos.x - pWidth/2;
  float py = cameraPos.y - pLength/2;
  float pz = cameraPos.z - pHeight/2;


glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[0]);//neg_z
glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(px,          py,           pz);
  glTexCoord2f(0, 1); glVertex3f(px,          py + pHeight, pz);
  glTexCoord2f(1, 1); glVertex3f(px + pWidth, py + pHeight, pz);
  glTexCoord2f(1, 0); glVertex3f(px + pWidth, py,           pz);
glEnd();
 
glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[2]);//pos_z
glBegin(GL_QUADS);
  glTexCoord2f(1, 0); glVertex3f(px,          py,           pz + pLength);
  glTexCoord2f(1, 1); glVertex3f(px,          py + pHeight, pz + pLength);
  glTexCoord2f(0, 1); glVertex3f(px + pWidth, py + pHeight, pz + pLength);
  glTexCoord2f(0, 0); glVertex3f(px + pWidth, py,           pz + pLength);
glEnd();
 
glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[5]);//neg_y
glBegin(GL_QUADS);
  glTexCoord2f(0, 1); glVertex3f(px + pWidth, py, pz);
  glTexCoord2f(1, 1); glVertex3f(px + pWidth, py, pz + pLength);
  glTexCoord2f(1, 0); glVertex3f(px,          py, pz + pLength);
  glTexCoord2f(0, 0); glVertex3f(px,          py, pz);
glEnd();

glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[4]);//pos_y
glBegin(GL_QUADS);
  glTexCoord2f(0, 1); glVertex3f(px,          py + pHeight, pz);
  glTexCoord2f(1, 1); glVertex3f(px,          py + pHeight, pz + pLength);
  glTexCoord2f(1, 0); glVertex3f(px + pWidth, py + pHeight, pz + pLength);
  glTexCoord2f(0, 0); glVertex3f(px + pWidth, py + pHeight, pz);
glEnd();
 
glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[1]);//pos_x
glBegin(GL_QUADS);
  glTexCoord2f(1, 0); glVertex3f(px, py,           pz);
  glTexCoord2f(0, 0); glVertex3f(px, py,           pz + pLength);
  glTexCoord2f(0, 1); glVertex3f(px, py + pHeight, pz + pLength);
  glTexCoord2f(1, 1); glVertex3f(px, py + pHeight, pz);
glEnd();
 
glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[3]);//neg_x
glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(px + pWidth, py,           pz);
  glTexCoord2f(1, 0); glVertex3f(px + pWidth, py,           pz + pLength);
  glTexCoord2f(1, 1); glVertex3f(px + pWidth, py + pHeight, pz + pLength);
  glTexCoord2f(0, 1); glVertex3f(px + pWidth, py + pHeight, pz);
 glEnd();

  // restore matrix and attributes
  glPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}

void drawLight() {
  GLfloat lp[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, lp);
  // draw yellow sphere for light source
  glPushMatrix();    
    glTranslatef(lp[0], lp[1], lp[2]);
    glColor3f(1,1,0);    
    glutSolidSphere(2.0f,16,16);    
  glPopMatrix();  
}


void drawTerrain() {
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    meshes[0].draw();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // reset active color to white
    glDisable(GL_COLOR_MATERIAL);
}

void renderScene() {
  unsigned int trianglesDrawn = 0;
  unsigned int objectsDrawn = 0;
  // clear and set camera
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  Vec3f cameraLookAt = cameraPos + cameraDir;
  gluLookAt(cameraPos.x,    cameraPos.y,    cameraPos.z,    // Position
            cameraLookAt.x, cameraLookAt.y, cameraLookAt.z, // Lookat
            0.0,            1.0,            0.0);           // Up-direction
  // sky box without lighting and without depth test
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  drawSkybox();
  glEnable(GL_DEPTH_TEST);
  // draw coordinate system without lighting  
  drawCS();
  // draw sphere for light still without lighting
  drawLight();
  // draw objects. count triangles and objects drawn.
  unsigned int triangles = 0;  
  //draw() returns the drawn number of triangles
  drawTerrain();

  // doppeldecker
//   glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
//   frustum.init(cameraPos, cameraLookAt, Vec3f(0.0,1.0,0.0));
//   if(frustum.test(meshes[1].getBoundingBoxMin(), meshes[1].getBoundingBoxMax())){
//     triangles = meshes[1].draw();
//   }
//   glBindTexture(GL_TEXTURE_2D, 0);
  
  // draw all meshes (with texture) 
  // meshes[0] is the terrain
  frustum.init(cameraPos, cameraLookAt, Vec3f(0.0,1.0,0.0));
  for(int i = 1; i < meshes.size(); i++){
    if (meshes[i].textureID != -1){
      glBindTexture(GL_TEXTURE_2D, textureIDs[meshes[i].textureID]);
    }
    
    if(frustum.test(meshes[i].getBoundingBoxMin(), meshes[i].getBoundingBoxMax())){
      triangles = meshes[i].draw();
    }
    glBindTexture(GL_TEXTURE_2D, 0); 
    
    if (triangles > 0) {
      trianglesDrawn += triangles;
      objectsDrawn++;
    }
  }
  
  // swap Buffers
  glutSwapBuffers();
  // cout number of objects and triangles if different from last run
  if (objectsDrawn != objectsLastRun || trianglesDrawn != trianglesLastRun) {
    objectsLastRun = objectsDrawn;
    trianglesLastRun = trianglesDrawn;
    cout << "renderScene: " << objectsDrawn << " objects and " << trianglesDrawn << " triangles." << endl;
  }
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y) {
  keyDown[key] = true;
	switch (key) {
    case 27:
      exit(0);
      break;
    case '1':
      glShadeModel(GL_FLAT);
      glutPostRedisplay();
      break;
    case '2':
      glShadeModel(GL_SMOOTH);
      glutPostRedisplay();
      break; 
    case 'M' :
    case 'm' :
      drawMode = (drawMode + 1) % 3;
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].setDrawMode(drawMode);
      glutPostRedisplay();
      break;
    case 'h' :
    case 'H' :
      coutHelp();
      break;
    // reset view
    case 'r' :
    case 'R' :
      setDefaults();
      glutPostRedisplay();
      break;
    // light movement
    case 'l' :
    case 'L' :
	    moveLight = !moveLight;
      break;
    // color array
    case 'c' :
    case 'C' :
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithColorArray();
      glutPostRedisplay();
      break;
    // texture
    case 't' :
    case 'T' :
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithTexture();
      glutPostRedisplay();
      break;
    // bounding box
    case 'b' :
    case 'B' :
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithBB();
      glutPostRedisplay();
      break;
    // normal
    case 'n' :
    case 'N' :
     for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithNormals();
      glutPostRedisplay();
      break;
    // movement speed up
    case '+' :
      movementSpeed *= 2.0f;
      break;
    // movement speed down
    case '-' :
      movementSpeed /= 2.0f;
      break;
	}
}

void keyReleased(unsigned char key, int x, int y) {
  keyDown[key] = false;
}

void mousePressed(int button, int state, int x, int y) {
  mouseButton = button;
  mouseX = x; 
  mouseY = y;
}

void mouseMoved(int x, int y) {
  // update angle with relative movement
  angleX = fmod(angleX + (x-mouseX)*mouseSensitivy,360.0f);
  angleY -= (y-mouseY)*mouseSensitivy;
  angleY = max(-70.0f,min(angleY,70.0f));
  // calculate camera direction (vector length 1)
  cameraDir.x =  sin(angleX*M_RadToDeg) * cos(angleY*M_RadToDeg);
  cameraDir.z = -cos(angleX*M_RadToDeg) * cos(angleY*M_RadToDeg);
  cameraDir.y = max(0.0f,min(sqrtf(1.0f - cameraDir.x*cameraDir.x -  cameraDir.z* cameraDir.z),1.0f));
  if (angleY < 0) cameraDir.y = -cameraDir.y;
  // update mouse for next relative movement
  mouseX = x;
  mouseY = y;
  glutPostRedisplay();
}

// ===============
// === VARIOUS ===
// ===============

void coutHelp() {
  cout << endl;
  cout << "====== KEY BINDINGS ======" << endl;
  cout << "ESC: exit" << endl;
  cout << "H: show this (H)elp file" << endl;
  cout << "R: (R)eset to default values" << endl;
  cout << "L: toggle (L)ight movement" << endl;
  cout << endl;
  cout << "A,W,S,D: first person movement" << endl;
  cout << "+,-: movement speed up and down" << endl;
  cout << endl;
  cout << "1: GL Shader FLAT" << endl;
  cout << "2: GL Shader SMOOTH" << endl;
  cout << endl;
  cout << "M: Switch Draw (M)ode. 0: Immediate, 1: Array, 2: VBO" << endl;
  cout << "C: toggle use of (C)olor array" << endl;
  cout << "T: toggle use of (T)extures" << endl;
  cout << "B: toggle (B)ounding box" << endl;
  cout << "N: toggle drawing (N)ormals" << endl;
  cout << "==========================" << endl;
  cout << endl;
}
