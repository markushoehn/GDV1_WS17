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

#include <GL/glew.h>      // openGL helper
#include <GL/glut.h>      // openGL helper

#include <stdlib.h>       // namespace std
#include <math.h>         // fmod
#include <time.h>         // clock_t
#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file
#include <algorithm>	  // min, max	

#include "imageloader.h"  // simple class for loading bmp files

#include "main.h"

// ==============
// === BASICS ===
// ==============

int main(int argc, char** argv) {

  // init light

  ambientLight[0] = 0.6f; ambientLight[1] = 0.6f; ambientLight[2] = 0.6f; ambientLight[3] = 1.0f;
  diffuseLight[0] = 0.8f; diffuseLight[1] = 0.8f; diffuseLight[2] = 0.8; diffuseLight[3] = 1.0f;
  specularLight[0] = 0.5f; specularLight[1] = 0.5f; specularLight[2] = 0.5f; specularLight[3] = 1.0f;

  //Vec3f ilambdai(0.6f, 0.6f, 0.6f); // itensity of light source
  //float ilambdaa = 0.6f; // ambient intensity
  //Vec3f fd(0.8f, 0.8f, 0.8); // diffuseLight
  //Vec3f fs(0.5f, 0.5f, 0.5f); // specularLight


  // initialize openGL window
  glutInit(&argc, argv);
  glutInitWindowPosition(300,200);
  glutInitWindowSize(300,300);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("OpenGL Raytracer"); 
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
  string buildprefix = ".";

  skyboxTextureIDs.resize(6,0);
  textureIDs.resize(2,0);

  // load sky box-
  string names [] = {"neg_z.bmp", "pos_x.bmp", "pos_z.bmp", "neg_x.bmp", "pos_y.bmp", "neg_y.bmp"};
  string sbox = "skybox", filename = "";
  Image * image;
  for(int i = 0; i < 6; ++i)
  {
    filename = buildprefix + "/Textures/" + sbox + "/" +names[i];

    std::cout << filename << std::endl;

    image = loadBMP(filename.c_str());
    skyboxTextureIDs[i] = loadTexture(image);
  }

  // load meshes
  // string filename;
  TriangleMesh tm1;
  filename = buildprefix + "/Models/ballon.off";
  tm1.loadOFF(filename.c_str(), Vec3f(0.0f,0.0f,0.0f), 20.0f);
  meshes.push_back(tm1);
  tm1.loadOFF(filename.c_str(), Vec3f(1.0f,-0.1f,1.0f), 20.0f);
  meshes.push_back(tm1);
  TriangleMesh tm2;
  filename = buildprefix + "/Models/delphin.off";
  int ycount = 5;
  int xcount = 5;
  for (int i = 0; i < xcount; ++i)
  {
    for (int j = 0; j < ycount; ++j)
    {
      tm2.loadOFF(filename.c_str(), Vec3f(0.6f+1.0f*i,1.0f,0.3f+1.0f*j), 7.0f);
      meshes.push_back(tm2);
    }
  }
  for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].coutData();
  
  filename = buildprefix + "/Textures/TEST_GRID.bmp";
  image = loadBMP(filename.c_str());
  textureIDs[0] = loadTexture(image);
  textureIDs[1] = skyboxTextureIDs[0];

  // add object attributes (material, texture, ...)
  SceneObject so;
  so.matAmbient[0]  = 0.2f; so.matAmbient[1]  = 0.1f; so.matAmbient[2]  = 0.1f; so.matAmbient[3]  = 1.0f;
  so.matDiffuse[0]  = 0.6f; so.matDiffuse[1]  = 0.3f; so.matDiffuse[2]  = 0.3f; so.matDiffuse[3]  = 1.0f;
  so.matSpecular[0] = 0.4f; so.matSpecular[1] = 0.4f; so.matSpecular[2] = 0.4f; so.matSpecular[3] = 1.0f;
  so.matReflect[0]  = 0.2f; so.matReflect[1]  = 0.2f; so.matReflect[2]  = 0.2f; so.matReflect[3]  = 1.0f;
  so.matOpacity[0]  = 0.0f; so.matOpacity[1]  = 0.0f; so.matOpacity[2]  = 0.0f; so.matOpacity[3]  = 1.0f;
  so.matShininess = 0.8f * 128.0f;
  so.textureID = textureIDs[0];
  objects.push_back(so);
  so.matAmbient[0]  = 0.1f; so.matAmbient[1]  = 0.2f; so.matAmbient[2]  = 0.1f; so.matAmbient[3]  = 1.0f;
  so.matDiffuse[0]  = 0.3f; so.matDiffuse[1]  = 0.6f; so.matDiffuse[2]  = 0.3f; so.matDiffuse[3]  = 1.0f;
  so.matSpecular[0] = 0.4f; so.matSpecular[1] = 0.4f; so.matSpecular[2] = 0.4f; so.matSpecular[3] = 1.0f;
  so.matReflect[0]  = 0.5f; so.matReflect[1]  = 0.5f; so.matReflect[2]  = 0.5f; so.matReflect[3]  = 1.0f;
  so.matOpacity[0]  = 0.4f; so.matOpacity[1]  = 0.4f; so.matOpacity[2]  = 0.4f; so.matOpacity[3]  = 0.4f; 
  so.matShininess = 0.8f * 128.0f;
  so.textureID = textureIDs[0];
  objects.push_back(so);

  for (int i = 0; i < xcount*ycount; ++i)
  {
    so.matAmbient[0]  = 0.1f; so.matAmbient[1]  = 0.2f; so.matAmbient[2]  = 0.1f; so.matAmbient[3]  = 1.0f;
    so.matDiffuse[0]  = 0.3f; so.matDiffuse[1]  = 0.6f; so.matDiffuse[2]  = 0.3f; so.matDiffuse[3]  = 1.0f;
    so.matSpecular[0] = 0.4f; so.matSpecular[1] = 0.4f; so.matSpecular[2] = 0.4f; so.matSpecular[3] = 1.0f;
    so.matReflect[0]  = 0.5f; so.matReflect[1]  = 0.5f; so.matReflect[2]  = 0.5f; so.matReflect[3]  = 1.0f;
    so.matShininess = 0.8f * 128.0f;
    so.textureID = textureIDs[0];
    objects.push_back(so);
  }

  // activate main loop
  coutHelp();
  glutTimerFunc(10, processTimedEvent, clock());
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
  // activate depth tests
  glEnable(GL_DEPTH_TEST);
  // enable light with defaults
  glEnable(GL_LIGHT0);
}

void setDefaults() {
  // scene Information
  cameraPos.set(0,0,10);
  cameraDir.set(0,0,-1);
  movementSpeed = 0.02f;
  // light information
  lightPos.set(20.0f, 2.0f, 0.0f);
  lightMotionSpeed = 0.05f;
  moveLight = false;
  // mouse information
  mouseButton = 0;
  mouseSensitivy = 1.0f;
  // key information, all false
  keyDown.resize(256,false);
  // draw mode (VBO)
  drawMode = 0;
}

void reshape(const GLint width, const GLint height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (float)width / (float)height, 1, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void processTimedEvent(const int x) {
  // Time now, int x is time of last run
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

void drawSkybox() {
  // prepare (no lighting, no depth test, texture filter)
  glEnable(GL_TEXTURE_2D);
  glColor3f(1,1,1);
  // draw skybox
  
  float pWidth = 4;
  float pLength = 4;
  float pHeight = 4;
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
  GLfloat al[] = { ambientLight[0], ambientLight[1], ambientLight[2], 1.0f };
  GLfloat dl[] = { diffuseLight[0], diffuseLight[1], diffuseLight[2], 1.0f };
  GLfloat sl[] = { specularLight[0], specularLight[1], specularLight[2], 1.0f };
  glLightfv(GL_LIGHT0, GL_AMBIENT, al);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dl);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sl);
  glLightfv(GL_LIGHT0, GL_POSITION, lp);


  // draw yellow sphere for light source
  glPushMatrix();    
    glTranslatef(lp[0], lp[1], lp[2]);
    glColor3f(1,1,0);    
    glutSolidSphere(0.2f,16,16);
  glPopMatrix();  
}

void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // render ray trace result on a quad in front of the camera

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  drawSkybox();
  glEnable(GL_DEPTH_TEST);

  if ( raytracedTextureID ) {
    // save matrix and load identities => no transformation, no projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // render textured quad in the middle of the view frustum (depth 0) filling complete view window
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, raytracedTextureID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex3d(-1.0,-1.0,0);
    glTexCoord2d(1.0,0.0); glVertex3d( 1.0,-1.0,0);
    glTexCoord2d(1.0,1.0); glVertex3d( 1.0, 1.0,0);
    glTexCoord2d(0.0,1.0); glVertex3d(-1.0, 1.0,0);
    glEnd();
    // restore matrix
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_2D);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
  // else, standard openGL rendering
  else {
    // clear and set camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vec3f cameraLookAt = cameraPos + cameraDir;
    gluLookAt(cameraPos.x,    cameraPos.y,    cameraPos.z,    // Position
              cameraLookAt.x, cameraLookAt.y, cameraLookAt.z, // Lookat
              0.0,            1.0,            0.0);           // Up-direction  
    // draw coordinate system without lighting
    glDisable(GL_LIGHTING);
    drawCS();
    // draw sphere for light still without lighting
    drawLight();
    // draw object
    glEnable(GL_LIGHTING);
    for (unsigned int i = 0; i < meshes.size(); i++) {
      // apply materials and texture
      glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].matAmbient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].matDiffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].matSpecular);
      glMaterialf(GL_FRONT, GL_SHININESS, objects[i].matShininess);
      glBindTexture(GL_TEXTURE_2D, objects[i].textureID);
      // draw
      meshes[i].draw();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // swap Buffers
  glutSwapBuffers();
}

void calculateIntensity(Ray<float> ray, Vec3f& I, unsigned int& hits, int depth, bool verbose) {
      float t = 1000.0f;              // ray parameter hit point, initialized with max view length
      float u,v;                      // barycentric coordinates (w = 1-u-v)
      // intersection test
      int hitMesh;
      unsigned int hitTri;
      if (verbose) std::cout << std::endl << "test - depth " << depth << std::endl;
      if ((hitMesh = intersectRayObjectsEarliest(ray,t,u,v,hitTri)) != -1) {
        if(verbose) std::cout << "hit" << std::endl;
        // get hit position
        vector<Vec3f>& vertices = meshes[hitMesh].getVertices();
        vector<Vec3ui>& triangles = meshes[hitMesh].getTriangles();
        Vec3f p0 = vertices[triangles[hitTri][0]];
        Vec3f p1 = vertices[triangles[hitTri][1]];
        Vec3f p2 = vertices[triangles[hitTri][2]];
        // reconstruction from barycentric coordinates:
        Vec3f P = (1-u-v) * p0 + u * p1 + v * p2;
        // get light vector
        Vec3f L = lightPos - P;
        L.normalize();
        // get normal vector
        vector<Vec3f>& normals = meshes[hitMesh].getNormals();
        Vec3f n0 = normals[triangles[hitTri][0]];
        Vec3f n1 = normals[triangles[hitTri][1]];
        Vec3f n2 = normals[triangles[hitTri][2]];
        // interpolation via barycentric coordinates:
        Vec3f N = (1-u-v) * n0 + u * n1 + v * n2;
        N /= N.length();
        // get observation vector
        Vec3f V = ray.o - P;
        V.normalize();
        // get halfway vector
        Vec3f H = V + L;
        H.normalize();

        Vec3f ilambdaa(ambientLight[0], ambientLight[1], ambientLight[2]);
        Vec3f fd(diffuseLight[0], diffuseLight[1], diffuseLight[2]);
        Vec3f fs(specularLight[0], specularLight[1], specularLight[2]);

        Vec3f ilambdai(1.0f, 1.0f, 1.0f); // itensity of light source

        Vec3f ka(objects[hitMesh].matAmbient[0], objects[hitMesh].matAmbient[1], objects[hitMesh].matAmbient[2]);
        Vec3f kd(objects[hitMesh].matDiffuse[0], objects[hitMesh].matDiffuse[1], objects[hitMesh].matDiffuse[2]);
        Vec3f ks(objects[hitMesh].matSpecular[0], objects[hitMesh].matSpecular[1], objects[hitMesh].matSpecular[2]);
        Vec3f kr(objects[hitMesh].matReflect[0], objects[hitMesh].matReflect[1], objects[hitMesh].matReflect[2]);
        Vec3f kt(objects[hitMesh].matOpacity[0], objects[hitMesh].matOpacity[1], objects[hitMesh].matOpacity[2]); 
        float ke = objects[hitMesh].matShininess;

        // create ray against light source
        float epsilon = 0.0001f;
        Vec3f newPos = P + (epsilon * L);
        float endP[3]; endP[0]=(float)lightPos[0];    endP[1]=(float)lightPos[1];    endP[2]=(float)lightPos[2];
        float eyeP[3]; eyeP[0]=(float)newPos[0];      eyeP[1]=(float)newPos[1];      eyeP[2]=(float)newPos[2];
        Ray<float> lightRay(&eyeP[0], &endP[0]);

        int s = 0;
        // check if lightray hits light
        if ((hitMesh = intersectRayObjectsEarliest(lightRay,t,u,v,hitTri)) == -1) {
          s = 1;
        }
        if(verbose) std::cout << "light ray: s=" << s << std::endl;

        // create recursive ray, in ideally reflected direction
        Vec3f R = 2 * (V * N) * N - V;
        newPos = P + (epsilon * R);
        Vec3f recPos = newPos + R * 1000;
        float endR[3]; endR[0]=(float)recPos[0];  endR[1]=(float)recPos[1];  endR[2]=(float)recPos[2];
        float eyeR[3]; eyeR[0]=(float)newPos[0];  eyeR[1]=(float)newPos[1];  eyeR[2]=(float)newPos[2];
        Ray<float> recursiveRay(&eyeR[0], &endR[0]);

        // check if recursiveRay hits other objects
        Vec3f recI(0.0f, 0.0f, 0.0f);
        if ((depth > 0)) {
            if(verbose) std::cout << "---- start recursion";
            calculateIntensity(recursiveRay, recI, hits, depth-1, verbose);
            if(verbose) std::cout << "---- end recursion" << std::endl;
        }
        else if(verbose) std::cout << std::endl << "no further recursion - min depth reached" << std::endl;
        if(verbose) std::cout << "recursive intensity: " << recI << std::endl;

        // create transparent ray, through the object
        newPos = P + (V * epsilon * (-1));
        Vec3f transPos = newPos + V * 1000 * (-1);
        float endT[3]; endT[0]=(float)transPos[0];  endT[1]=(float)transPos[1];  endT[2]=(float)transPos[2];
        float eyeT[3]; eyeT[0]=(float)newPos[0];    eyeT[1]=(float)newPos[1];    eyeT[2]=(float)newPos[2];

        // check if transparent ray hits other objects
        Vec3f transI(0.0f, 0.0f, 0.0f);
        Ray<float> transparentRay(&eyeT[0], &endT[0]);
        if (depth > 0) {
            if(verbose) std::cout << "---- start transparency recursion";
            calculateIntensity(transparentRay, transI, hits, depth-1, verbose);
            if(verbose) std::cout << "---- end transparency recursion" << std::endl;
        } else if(verbose) std::cout << "no further transparency recursion - min depth reached" << std::endl;
        if(verbose) std::cout << "transparent intensity: " << transI << std::endl;

        // calculate each pixel value
        if(verbose && !depth) {
            std::cout << "parameters recursive calls:" << std::endl;
            std::cout << "hitMesh: " << hitMesh << std::endl;
            std::cout << "hitTri: " << hitTri << std::endl;
            std::cout << "fs: " << fs << std::endl;
            std::cout << "ilambdai: " << ilambdai << std::endl;
            std::cout << "ks: " << ks << std::endl;
            std::cout << "ke: " << ke << std::endl;
            std::cout << "Normal vector: " << N << std::endl;
            std::cout << "Light vector: " << L << std::endl;
            std::cout << "N * L: " << N * L << std::endl;
            std::cout << "Observation vector: " << V << std::endl;
            std::cout << "Halfway vector: " << H << std::endl;
            std::cout << "H * N: " << H * N << std::endl;
            std::cout << "pow(H * N, ke): " << pow(H * N, ke) << std::endl;
            std::cout << "ambient: " << ilambdaa[0] * ka[0] << std::endl;
            std::cout << "diffuse: " << s * ilambdai[0] * kd[0] * fd[0] * (L * N) << std::endl;
            std::cout << "specular: " << s * ilambdai[0] * ks[0] * fs[0] * pow(H * N, ke) << std::endl;
            std::cout << "reflected: " << kr[0] * recI[0] << std::endl;
        }
        float NH = max(N * H, 0.0f);
        float NL = max(N * L, 0.0f);
        float redPixel = (ilambdaa[0] * ka[0]) + (s * ilambdai[0] * (kd[0] * fd[0] * NL + ks[0] * fs[0] * pow(NH, ke))) + (kr[0] * recI[0]) + (kt[0] * transI[0]);
        float greenPixel = (ilambdaa[1] * ka[1]) + (s * ilambdai[1] * (kd[1] * fd[1] * NL + ks[1] * fs[1] * pow(NH, ke))) + (kr[1] * recI[1]) + (kt[1] * transI[1]);
        float bluePixel = (ilambdaa[2] * ka[2]) + (s * ilambdai[2] * (kd[2] * fd[2] * NL + ks[2] * fs[2] * pow(NH, ke))) + (kr[2] * recI[2]) + (kt[2] * transI[2]);
        I[0] = redPixel;
        I[1] = greenPixel;
        I[2] = bluePixel;
        if(verbose) std::cout << "overall intensity: " << I << std::endl;
        hits++;
      }
      else if (verbose)
          std::cout << "no hit" << std::endl;
}

void raytrace() {
  // initialization
  GLdouble MV[16];
  GLdouble PR[16];
  GLint VP[4];
  glGetDoublev(GL_MODELVIEW_MATRIX, MV);
  glGetDoublev(GL_PROJECTION_MATRIX, PR);
  glGetIntegerv(GL_VIEWPORT, VP);
  vector<Vec3f> pictureRGB(VP[2]*VP[3]);
  intersectionTests = 0;
  unsigned int hits = 0;
  clock_t clockStart = clock();
  std::cout << "   10   20   30   40   50   60   70   80   90  100" << endl;
  std::cout << "====|====|====|====|====|====|====|====|====|====|" << endl;
  // iterate over all pixel
  unsigned int pixelCounter = 0;
  #pragma omp parallel for schedule(dynamic)
  for (int y = VP[1]; y < VP[1] + VP[3]; y++) {
    for (int x = VP[0]; x < VP[0] + VP[2]; x++) {
      // get pixel index for addressing pictureRGB array
      int pixel = (y-VP[1])*(VP[2]-VP[0]) + x - VP[0];
      GLdouble end[3];
      GLdouble eye[3];
      // convert pixel coordinate to two points on near and far plane in world coordinates
      gluUnProject(x,y,-1, MV, PR, VP, &eye[0], &eye[1], &eye[2]);
      gluUnProject(x,y, 1, MV, PR, VP, &end[0], &end[1], &end[2]);
      // create primary ray
      float endF[3]; endF[0]=(float)end[0];  endF[1]=(float)end[1];  endF[2]=(float)end[2]; 
      float eyeF[3]; eyeF[0]=(float)eye[0];  eyeF[1]=(float)eye[1];  eyeF[2]=(float)eye[2]; 
      Ray<float> ray(&eyeF[0], &endF[0]);
      pictureRGB[pixel] = Vec3f(0.0f, 0.0f, 0.0f);
      calculateIntensity(ray, pictureRGB[pixel], hits, 3, false);
      // cout "." every 1/50 of all pixels
      #pragma omp flush (pixelCounter)
      pixelCounter++;
      #pragma omp flush (pixelCounter)
      if (pixelCounter % (VP[2]*VP[3]/50) == 0) cout << ".";      
    }
  }
  clock_t clockEnd = clock();
  int msPassed = (clockEnd - clockStart)*1000/CLOCKS_PER_SEC;
  cout << endl << "finished. tests: " << intersectionTests << ", hits: " << hits << ", ms: " << msPassed << endl;
  // generate openGL texture
  float mul = 255.0f; // multiply rgb values within [0,1] by 255 
  cout << "normalizing picture with multiplicator " << mul << endl << endl;
  vector<GLuint> picture(VP[2]*VP[3]);
  for (int y = VP[1]; y < VP[1] + VP[3]; y++) {
    for (int x = VP[0]; x < VP[0] + VP[2]; x++) {
      // get pixel index for addressing pictureRGB array
      int pixel = (y-VP[1])*(VP[2]-VP[0]) + x - VP[0];
      // cap R G B within 0 and 255
      int R = max(0, min((int)(mul*pictureRGB[pixel][0]), 255));
      int G = max(0, min((int)(mul*pictureRGB[pixel][1]), 255));
      int B = max(0, min((int)(mul*pictureRGB[pixel][2]), 255));
      // enter R G B values into GLuint array
      picture[pixel] = R | (G<<8) | (B<<16);
    }
  }
  // generate texture
  glGenTextures(1, &raytracedTextureID);
  glBindTexture(GL_TEXTURE_2D, raytracedTextureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VP[2], VP[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, &picture[0]);
}

int intersectRayObjectsEarliest(const Ray<float> &ray, float &t, float &u, float &v, unsigned int &hitTri) {
  // iterate over all meshes
  unsigned int i_small = -1;
  float t_small = -1.0f;
  float u_small = -1.0f;
  float v_small = -1.0f;
  for (unsigned int i = 0; i < meshes.size(); i++) {
    // optional: check ray versus bounding box first (t must have been initialized!)
    if (rayAABBIntersect(ray, meshes[i].boundingBoxMin, meshes[i].boundingBoxMax, 0.0f, t) == false) continue;
    // get triangle information
    vector<Vec3f>& vertices = meshes[i].getVertices();
    vector<Vec3ui>& triangles = meshes[i].getTriangles();

    // brute force: find smallest t
    for (unsigned int j = 0; j < triangles.size(); j++) {
      Vec3f& p0 = vertices[triangles[j][0]];
      Vec3f& p1 = vertices[triangles[j][1]];
      Vec3f& p2 = vertices[triangles[j][2]];
      int hit = ray.triangleIntersect(&p0.x, &p1.x, &p2.x, u_small, v_small, t_small);
      intersectionTests++;
      if (hit == 1 && t_small > 0.0f && t > t_small) {
        t = t_small;
        i_small = i;
        hitTri = j;
        u = u_small;
        v = v_small;
      }
    }
  }
  return i_small;
}

// Smits’ method: Brian Smits. Efficient bounding box intersection. Ray tracing news, 15(1), 2002.
bool rayAABBIntersect(const Ray<float> &r, const Vec3f& vmin, const Vec3f& vmax, float t0, float t1) {
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  if (r.d.x >= 0) {
    tmin = (vmin.x - r.o.x) / r.d.x;
    tmax = (vmax.x - r.o.x) / r.d.x;
  }
  else {
    tmin = (vmax.x - r.o.x) / r.d.x;
    tmax = (vmin.x - r.o.x) / r.d.x;
  }
  if (r.d.y >= 0) {
    tymin = (vmin.y - r.o.y) / r.d.y;
    tymax = (vmax.y - r.o.y) / r.d.y;
  }
  else {
    tymin = (vmax.y - r.o.y) / r.d.y;
    tymax = (vmin.y - r.o.y) / r.d.y;
  }
  if ( (tmin > tymax) || (tymin > tmax) )
    return false;

  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  if (r.d.z >= 0) {
    tzmin = (vmin.z - r.o.z) / r.d.z;
    tzmax = (vmax.z - r.o.z) / r.d.z;
  }
  else {
    tzmin = (vmax.z - r.o.z) / r.d.z;
    tzmax = (vmin.z - r.o.z) / r.d.z;
  }

  if ( (tmin > tzmax) || (tzmin > tmax) )
    return false;

  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;

  return ( (tmin < t1) && (tmax > t0) );
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(const unsigned char key, const int x, const int y) {
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
    case 'p' :
    case 'P' :
      lightPos = cameraPos;
      glutPostRedisplay();
      break;
    // raytrace
    case 'x' :
    case 'X' :
      raytrace();
      glutPostRedisplay();
      break;
    case 'y' :
    case 'Y' :
      glDeleteTextures(1,&raytracedTextureID);
      raytracedTextureID = 0;
      glutPostRedisplay();
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

void keyReleased(const unsigned char key, const int x, const int y) {
  keyDown[key] = false;
}

void mousePressed(const int button, const int state, const int x, const int y) {
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
  cout << "H: show this (h)elp file" << endl;
  cout << "R: (r)eset to default values" << endl;
  cout << "L: toggle (l)ight movement" << endl;
  cout << "P: move the light to current (p)osition" << endl;
  cout << endl;
  cout << "A,W,S,D,Q,E: first person movement" << endl;
  cout << "+,-: movement speed up and down" << endl;
  cout << endl;
  cout << "1: GL Shader FLAT" << endl;
  cout << "2: GL Shader SMOOTH" << endl;
  cout << endl;
  cout << "M: switch draw (m)ode. 0: immediate, 1: array, 2: VBO" << endl;
  cout << "C: toggle use of (c)olor array" << endl;
  cout << "T: toggle use of (t)exture coordinates" << endl;
  cout << "B: toggle drawing (b)ounding box" << endl;
  cout << "N: toggle drawing (n)ormals" << endl;
  cout << endl;
  cout << "X: raytrace scene" << endl;
  cout << "Y: clear and switch back to openGL rendering" << endl;
  cout << "==========================" << endl;
  cout << endl;
}
