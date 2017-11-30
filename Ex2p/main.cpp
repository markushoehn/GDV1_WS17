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

#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file
#include <math.h>         // fmod
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
  glutCreateWindow("Framework VBO and Shader");
  // link functions to certain openGL events
  glutDisplayFunc(renderScene);
  glutReshapeFunc(reshape);
  glutMouseFunc(mousePressed);
  glutMotionFunc(mouseMoved);
  glutKeyboardFunc(keyPressed);
  // further initializations
  setDefaults();
  initialize();
  // load shaders
  GLuint programID = 0;
  programID = readShaders("Shader/FlatGreyShader.vert", "Shader/FlatGreyShader.frag");
  if (programID != 0) programIDs.push_back(programID);
  cout << programIDs.size() << " shaders loaded. Use keys 3 to " << programIDs.size() + 2 << "." << endl;
  // load mesh
  //char* filename = "Models/delphin.off";
  char* filename = "Models/Sketched-Teddy-org.off";
  Vec3f mid(0.0f,0.0f,0.0f);
  trimesh.loadOFF(filename, mid, 4.0f);
  // activate main loop
  glutTimerFunc(20, processTimedEvent, 0);
  glutMainLoop();
  return 0;
}

void setDefaults() {
  // scene Information
  centerPos.set(0.0f, -4.0f, -8.0f);
  angleX = 0.0f;
  angleY = 0.0f;
  // light information
  lightPos.set(-30.0f, 10.0f, 0.0f);
  lightMotionSpeed = 2.0f;
  moveLight = false;
  // mouse information
  mouseX = 0;
  mouseY = 0;
  mouseButton = 0;
  mouseSensitivy = 1.0f;
  // immediate draw mode
  drawMode = 0;
  // grid size for drawing objects
  gridSize = 3;
  // reset FPS
  fpsCout = false;
  fpsClock = clock();
  fpsCounterFrames = 0;
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
    glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv) {
      cout << "VBOs are supported!" << endl;
  }
  else {
    cout << "VBOs are NOT supported!" << endl;
  }
  // enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // normalize normals because of glScalef used
  glEnable(GL_NORMALIZE);
  // set shading model
  glShadeModel(GL_SMOOTH);
  // set lighting (white light)
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
  // enable use of glColor instead of glMaterial
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
  // Time now
  clock_t clock_this_run = clock();
  // perform light movement and trigger redisplay
  if (moveLight == true) {
    lightPos.rotY(lightMotionSpeed);
    glutPostRedisplay();
  }
  // calculate FPS
  if (fpsCout == true) {
    int msPassedFPS = clock() - fpsClock;
    // cout fps after roughly one second
    if (msPassedFPS > 1000) {
      int gridLength = 2*gridSize + 1;
      int triangles = gridLength*gridLength*trimesh.getNumTriangles();
      float fps = 1000.0f*fpsCounterFrames/msPassedFPS;
      int tps = (int)(fps*triangles);
      cout << fpsCounterFrames << " frames in " << msPassedFPS << " ms: " << fps << " FPS, " << 0.000001*tps << "M triangles/sec." << endl;
      fpsClock = clock();
      fpsCounterFrames = 0;
    }
    // redraw as soon as possible for fps calculation
    glutPostRedisplay();
    glutTimerFunc(1 , processTimedEvent, 0);
    return;
  }
  // start event again
  int msPassed = clock() - clock_this_run;
  glutTimerFunc(std::max(1,20-msPassed) , processTimedEvent, 0);
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
  GLfloat lp[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, lp);
  // draw yellow sphere for light source
  glPushMatrix();
    glTranslatef(lp[0], lp[1], lp[2]);
    glColor3f(1,1,0);
    glutSolidSphere(0.2f,16,16);
  glPopMatrix();
}

void renderScene() {
  // fps counter
  fpsCounterFrames++;
  // clear and set camera
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  // translate to centerPos
  glTranslatef(centerPos.x, centerPos.y, centerPos.z);
  // rotate scene
  glRotatef(angleX,0.0f,1.0f,0.0f);
  glRotatef(angleY,1.0f,0.0f,0.0f);
  // draw coordinate system without lighting
  glDisable(GL_LIGHTING);
  drawCS();
    // draw sphere for light still without lighting
  drawLight();
  // scale scene to fixed box size
  float scale = 5.0f / max(1,gridSize);
  glScalef(scale, scale, scale);
  // draw objects
  glEnable(GL_LIGHTING);
  for (int i = -gridSize; i <= gridSize; ++i) {
	  for (int j = -gridSize; j <= gridSize; ++j) {
		  if (i != 0 || j != 0) {
			  float r = (float)i/(2.0f*gridSize) + 0.5f;
			  float g = (float)j/(2.0f*gridSize) + 0.5f;
			  float b = 1.0f - 0.5f*r - 0.5f*g;
        glColor3f(r,g,b);
		  }
      else glColor3f(1,1,1);
      glPushMatrix();
      glTranslatef(4.0f*i, 0.0f, 4.0f*j);
      switch (drawMode) {
      case 1:
        trimesh.drawArray();
        break;
      case 2:
        trimesh.drawVBO();
        break;
      default:
        trimesh.drawImmediate();
        break;
      }
      glPopMatrix();
	  }
  }
  // swap Buffers
  glutSwapBuffers();
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y) {
	switch (key) {
    // EXC => exit
    case 27:
      exit(0);
      break;
    // OpenGL flat shading
    case '1':
      glUseProgram(0);
      glShadeModel(GL_FLAT);
      glutPostRedisplay();
      break;
    // OpenGL smooth shading
    case '2':
      glUseProgram(0);
      glShadeModel(GL_SMOOTH);
      glutPostRedisplay();
      break;
    // shader 3-9
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
        int n = (int)key - 48;
        if (programIDs.size() > n-3)
          glUseProgram(programIDs[n-3]);
      }
      glutPostRedisplay();
      break;
    // change draw mode
    case 'M' :
    case 'm' :
      drawMode = (drawMode + 1) % 3;
      cout << "drawMode: " << drawMode;
      switch (drawMode) {
        case 1:  cout << " = array" << endl; break;
        case 2:  cout << " = VBO" << endl; break;
        default: cout << " = immediate" << endl; break;
      }
      glutPostRedisplay();
      break;
    // help text
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
    // Light movement
    case 'l' :
    case 'L' :
	    moveLight = !moveLight;
      break;
    // increase gridSize
    case '+' :
      {
        gridSize++;
        int gridLength = 2*gridSize + 1;
        cout << "Drawing " << gridLength*gridLength*trimesh.getNumTriangles() << " triangles." << endl;
        glutPostRedisplay();
        break;
      }
    // decrease gridSize
    case '-' :
      {
        gridSize--;
        if (gridSize < 0) gridSize = 0;
        int gridLength = 2*gridSize + 1;
        cout << "Drawing " << gridLength*gridLength*trimesh.getNumTriangles() << " triangles." << endl;
        glutPostRedisplay();
        break;
      }
    // toggle FPS calculation
    case 'f':
    case 'F':
      fpsCout = !fpsCout;
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

void coutHelp() {
  cout << endl;
  cout << "====== KEY BINDINGS ======" << endl;
  cout << "ESC: exit" << endl;
  cout << "H:   show this (H)elp file" << endl;
  cout << "R:   (R)eset view" << endl;
  cout << "L:   toggle (L)ight movement" << endl;
  cout << "F:   toggle FPS output" << endl;
  cout << "+/-: in-/decrease the gridSize for drawn objects" << endl;
  cout << "" << endl;
  cout << "1:   GL Shader FLAT" << endl;
  cout << "2:   GL Shader SMOOTH" << endl;
  cout << "3+:  Custom Shader" << endl;
  cout << "" << endl;
  cout << "M:   Switch Draw (M)ode. 0: Immediate, 1: Array, 2: VBO" << endl;
  cout << "==========================" << endl;
  cout << endl;
}