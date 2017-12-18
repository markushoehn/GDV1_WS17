// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes            //
//   * readOFF                                                                //
//   * draw                                                                   //
//   * transformations                                                        //
// ========================================================================== //

#include <iostream>
#include <iomanip>
#include <fstream>
#include <float.h>
#include <time.h>
#include "ClipPlane.h"
#include "TriangleMesh.h"
#include <algorithm>

// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh() {
  // initialize VBO ids, else clear() might try to delete not existing VBOs 
  VBOv = 0;
  VBOn = 0;
  VBOf = 0;
  VBOc = 0;
  VBOt = 0;
  checkVBOSupport();
  // clear all data
  clear();
}

TriangleMesh::~TriangleMesh() {
  // clear data
  vertices.clear();
  triangles.clear();
  normals.clear();
  colors.clear();
  texCoords.clear();
  boundingBoxMid.clear();
  boundingBoxSize.clear();
  /*The VBOs should have already been deleted!
  Calling glDeleteBuffers in the destructor could cause problems
  since the VBO could still be used in a copy of the class.
  This could occur if a TriangleMesh is stored in a std::vector
  */
}

void TriangleMesh::clear() {
  // clear mesh data
  vertices.clear();
  triangles.clear();
  normals.clear();
  colors.clear();
  texCoords.clear();
  // clear bounding box data
  boundingBoxMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
  boundingBoxMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  boundingBoxMid.clear();
  boundingBoxSize.clear();
  // clear VBO data
  cleanupVBO();
  checkVBOSupport();
  // draw mode data
  drawMode = 0;
  withColorArray = false;
  withBB = false;
  withNormals = false;
  withTexture = false;
}

void TriangleMesh::coutData() {
  cout << endl;
  cout << "=== MESH DATA ===" << endl;
  cout << "nr. triangles: " << triangles.size() << endl;
  cout << "nr. vertices:  " << vertices.size() << endl;
  cout << "nr. normals:   " << normals.size() << endl;
  cout << "nr. colors:    " << colors.size() << endl;
  cout << "nr. texCoords: " << texCoords.size() << endl;
  cout << "BB: (" << boundingBoxMin << ") - (" << boundingBoxMax << ")" << endl;
  cout << "  BBMid: (" << boundingBoxMid << ")" << endl;
  cout << "  BBSize: (" << boundingBoxSize << ")" << endl;
  cout << "VBOs supported = " << VBOsupported << endl;
  cout << "  VBO IDs: f=" << VBOf << ", v=" << VBOv << ", n=" << VBOn << ", c=" << VBOc << ", t=" << VBOt << endl;
  cout << "drawMode = " << drawMode << endl;
  cout << "  withColorArray = " << withColorArray << ", withBB = " << withBB << ", withNormals = " << withNormals << endl;
}

// ================
// === RAW DATA ===
// ================

void TriangleMesh::flipNormals() {
  for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) (*it) *= -1.0;
}

void TriangleMesh::translateToCenter(const Vec3f& newBBmid, bool createVBOs) {
  Vec3f trans = newBBmid - boundingBoxMid;
  for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] += trans;  
  boundingBoxMin += trans;
  boundingBoxMax += trans;
  boundingBoxMid += trans;
  // data changed => delete VBOs and create new ones (not efficient but easy)
  if (createVBOs == true && VBOsupported == true) {
    cleanupVBO();
    createAllVBOs();
  }
}

void TriangleMesh::scaleToLength(const float newLength, bool createVBOs) {
  float length = max(max(boundingBoxSize.x, boundingBoxSize.y), boundingBoxSize.z);
  float scale = newLength / length;
  for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] *= scale;
  boundingBoxMin *= scale;
  boundingBoxMax *= scale;
  boundingBoxMid *= scale;
  boundingBoxSize *= scale;
  // data changed => delete VBOs and create new ones (not efficient but easy)
  if (createVBOs == true && VBOsupported == true) {
    cleanupVBO();
    createAllVBOs();
  }
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadOFF(const char* filename, bool createVBOs) {
  // clear any existing mesh
  clear();
  // load from off
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadOFF: can not find " << filename << endl;
    return;
  }
  const int MAX = 256;
  char s[MAX];
  in >> setw(MAX) >> s;
  // differentiate between OFF (vertices only) and NOFF (vertices and normals)
  bool noff = false;
  if (s[0] == 'O' && s[1] == 'F' && s[2] == 'F')
    ;
  else if (s[0] == 'N' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F')
    noff = true;
  else
    return;
  // get number of vertices nv, faces nf and edges ne
  int nv,nf,ne;
  in >> setw(MAX) >> nv;
  in >> setw(MAX) >> nf;
  in >> setw(MAX) >> ne;
  if (nv <= 0 || nf <= 0) return;
  // read vertices
  vertices.resize(nv);
  for (int i = 0; i < nv; ++i) {
    in >> setw(MAX) >> vertices[i][0];
    in >> setw(MAX) >> vertices[i][1];
    in >> setw(MAX) >> vertices[i][2];
    boundingBoxMin[0] = std::min(vertices[i][0], boundingBoxMin[0]);
    boundingBoxMin[1] = std::min(vertices[i][1], boundingBoxMin[1]);
    boundingBoxMin[2] = std::min(vertices[i][2], boundingBoxMin[2]);
    boundingBoxMax[0] = std::max(vertices[i][0], boundingBoxMax[0]);
    boundingBoxMax[1] = std::max(vertices[i][1], boundingBoxMax[1]);
    boundingBoxMax[2] = std::max(vertices[i][2], boundingBoxMax[2]);
    if (noff) {
      in >> setw(MAX) >> normals[i][0];
      in >> setw(MAX) >> normals[i][1];
      in >> setw(MAX) >> normals[i][2];
    }
  }
  boundingBoxMid = 0.5f*boundingBoxMin + 0.5f*boundingBoxMax;
  boundingBoxSize = boundingBoxMax - boundingBoxMin;
  // read triangles
  triangles.resize(nf);
  for (int i = 0; i < nf; ++i) {
    int three;
    in >> setw(MAX) >> three;
    in >> setw(MAX) >> triangles[i][0];
    in >> setw(MAX) >> triangles[i][1];
    in >> setw(MAX) >> triangles[i][2];
  }
  // close ifstream
  in.close();
  // calculate normals if not given
  if (!noff) calculateNormalsByArea();
  // calculate texture coordinates
  calculateTexCoordsSphereMapping();
  // createVBO
  if (createVBOs == true) {
    createAllVBOs();
  }
}

void TriangleMesh::loadOFF(const char* filename, const Vec3f& BBmid, const float BBlength) {
  loadOFF(filename, false);  
  translateToCenter(BBmid, false);
  scaleToLength(BBlength, true);
}

void TriangleMesh::calculateNormalsByArea() {
  // sum up triangle normals in each vertex
  normals.resize(vertices.size());
  for (Triangles::iterator it = triangles.begin(); it != triangles.end(); ++it) {
    Vec3f vec1, vec2, normal;
    unsigned int id0, id1, id2;
    id0 = (*it)[0];
    id1 = (*it)[1];
    id2 = (*it)[2];
    vec1 = vertices[id1] - vertices[id0];
    vec2 = vertices[id2] - vertices[id0];
    normal = vec1 ^ vec2;
    normals[id0] += normal;
    normals[id1] += normal;
    normals[id2] += normal;
  }
  // normalize normals
  for (Normals::iterator nit=normals.begin(); nit!=normals.end(); ++nit) (*nit).normalize();
}

void TriangleMesh::calculateTexCoordsSphereMapping() {
  texCoords.clear();
  // texCoords by central projection on unit sphere
  // optional ...
}

void TriangleMesh::calculateBB() {
  // clear bounding box data
  boundingBoxMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
  boundingBoxMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  boundingBoxMid.clear();
  boundingBoxSize.clear();
  // iterate over vertices
  for (unsigned int i = 0; i < vertices.size(); i++) {
    boundingBoxMin[0] = std::min(vertices[i][0], boundingBoxMin[0]);
    boundingBoxMin[1] = std::min(vertices[i][1], boundingBoxMin[1]);
    boundingBoxMin[2] = std::min(vertices[i][2], boundingBoxMin[2]);
    boundingBoxMax[0] = std::max(vertices[i][0], boundingBoxMax[0]);
    boundingBoxMax[1] = std::max(vertices[i][1], boundingBoxMax[1]);
    boundingBoxMax[2] = std::max(vertices[i][2], boundingBoxMax[2]);
  }
  boundingBoxMid = 0.5f*boundingBoxMin + 0.5f*boundingBoxMax;
  boundingBoxSize = boundingBoxMax - boundingBoxMin;
}

void TriangleMesh::checkVBOSupport() {
  // check if VBOs are supported and cleanup VBOs
  if(glGenBuffers && glBindBuffer && glBufferData && glBufferSubData && glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv) {
    VBOsupported = true;
    //cout << "VBOs are supported!" << endl;
  }
  else {
    VBOsupported = false;
    //cout << "VBOs are NOT supported!" << endl;
  }
}

GLuint TriangleMesh::createVBO(const void* data, int dataSize, GLenum target, GLenum usage) {
  // 0 is reserved, glGenBuffers() will return non-zero id if success
  GLuint id = 0;  
  // create a vbo
  glGenBuffers(1, &id);
  // activate vbo id to use
  glBindBuffer(target, id);
  // upload data to video card
  glBufferData(target, dataSize, data, usage); 
  // check data size in VBO is same as input array, if not return 0 and delete VBO
  int bufferSize = 0;
  glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
  if(dataSize != bufferSize) {
    glDeleteBuffers(1, &id);
    id = 0;
    cout << "createVBO() ERROR: Data size (" << dataSize << ") is mismatch with input array (" << bufferSize << ")." << endl;
  }
  // unbind after copying data
  glBindBuffer(target, 0);   
  return id;
}

void TriangleMesh::createAllVBOs() {
  // create VBOs
  if (VBOsupported) {
    VBOf = createVBO(  &triangles[0].x,     triangles.size()*sizeof(Triangle), GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    VBOv = createVBO(   &vertices[0].x,      vertices.size()*sizeof(Vertex),   GL_ARRAY_BUFFER,         GL_STATIC_DRAW);
    VBOn = createVBO(    &normals[0].x,       normals.size()*sizeof(Normal),   GL_ARRAY_BUFFER,         GL_STATIC_DRAW);  
    if (colors.size() == vertices.size())
      VBOc = createVBO(   &colors[0].x,        colors.size()*sizeof(Color),    GL_ARRAY_BUFFER,         GL_STATIC_DRAW);
    if (texCoords.size() == vertices.size())
      VBOt = createVBO(&texCoords[0].u,     texCoords.size()*sizeof(TexCoord), GL_ARRAY_BUFFER,         GL_STATIC_DRAW);
  }
}

void TriangleMesh::cleanupVBO() {
  // delete VBO
  if (VBOsupported) {
    if (VBOv != 0) glDeleteBuffers(1, &VBOv);
    if (VBOn != 0) glDeleteBuffers(1, &VBOn);
    if (VBOf != 0) glDeleteBuffers(1, &VBOf);
    if (VBOc != 0) glDeleteBuffers(1, &VBOc);
    if (VBOt != 0) glDeleteBuffers(1, &VBOt);
  }
  VBOv = 0;
  VBOn = 0;
  VBOf = 0;
  VBOc = 0;
  VBOt = 0;
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::setDrawMode(unsigned int _drawMode) {
  drawMode = _drawMode;
  cout << "setDrawMode: " << drawMode;
  switch (drawMode) {
  case 1:  cout << " = array" << endl; break;
  case 2:  cout << " = VBO" << endl; break;
  default: cout << " = immediate" << endl; break;
  }
}

void TriangleMesh::toggleWithColorArray() {
  withColorArray = !withColorArray;
  cout << "toggleWithColorArray: " << withColorArray << endl;
}

void TriangleMesh::toggleWithTexture() {
  withTexture = !withTexture;
  cout << "toggleWithTexture: " << withTexture << endl;
}

void TriangleMesh::toggleWithBB() {
  withBB = !withBB;
  cout << "toggleWithBB: " << withBB << endl;
}

void TriangleMesh::toggleWithNormals() {
  withNormals = !withNormals;
  cout << "toggleWithNormals: " << withNormals << endl;
}

unsigned int TriangleMesh::draw() {
  if (boundingBoxIsVisible() == false) return 0; 
  glDisable(GL_LIGHTING);
  if (withBB) drawBB();
  if (withNormals) drawNormals();
  glEnable(GL_LIGHTING);
  switch(drawMode) {
    case 1:
      drawArray();
      break;
    case 2:
      if (VBOsupported == true) drawVBO();
      else drawArray();
      break;
    default:
      drawImmediate();
  }
  return triangles.size();
}

void TriangleMesh::drawImmediate() {
  if (triangles.size() == 0) return;
  if (withTexture && texCoords.size() == vertices.size()) glEnable(GL_TEXTURE_2D);
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < triangles.size(); i++) {
    for (unsigned int v = 0; v < 3; v++) {
      glNormal3f(normals[triangles[i][v]].x, normals[triangles[i][v]].y, normals[triangles[i][v]].z);
      if (withColorArray && colors.size() == vertices.size()) 
        glColor3f(colors[triangles[i][v]].x, colors[triangles[i][v]].y, colors[triangles[i][v]].z);
      if (withTexture && texCoords.size() == vertices.size())
        glTexCoord2f(texCoords[triangles[i][v]].u, texCoords[triangles[i][v]].v);
      glVertex3f(vertices[triangles[i][v]].x, vertices[triangles[i][v]].y, vertices[triangles[i][v]].z);
    }
  }
  glEnd();
  if (withTexture && texCoords.size() == vertices.size()) glDisable(GL_TEXTURE_2D);
}

void TriangleMesh::drawArray() {
  if (triangles.size() == 0) return;
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT,0, &vertices[0]);
  glNormalPointer(GL_FLOAT,0, &normals[0]);
  if (withColorArray && colors.size() == vertices.size()) {
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, &colors[0]);
  }
  if (withTexture && texCoords.size() == vertices.size()) {
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);
  }
  glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  if (withTexture && texCoords.size() == vertices.size()) glDisable(GL_TEXTURE_2D); 
}

void TriangleMesh::drawVBO() {
  if (VBOf == 0 || VBOv == 0 || VBOn == 0) return;
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, VBOn);
  glNormalPointer(GL_FLOAT, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, VBOv);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  if (withColorArray && colors.size() == vertices.size() && VBOc != 0) {
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, VBOc);
    glColorPointer(3, GL_FLOAT, 0, 0);
  }
  if (withTexture && texCoords.size() == vertices.size() && VBOt != 0) {
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, VBOt);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);
  glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  if (withColorArray && colors.size() == vertices.size() && VBOc != 0) glDisableClientState(GL_COLOR_ARRAY);
  if (withTexture && texCoords.size() == vertices.size()){
	   glDisable(GL_TEXTURE_2D);
	   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}

void TriangleMesh::drawBB() {
  glPushMatrix();
  glTranslatef(boundingBoxMid.x, boundingBoxMid.y, boundingBoxMid.z);
  glScalef(boundingBoxSize.x, boundingBoxSize.y, boundingBoxSize.z);
  glutWireCube(1.0);
  glPopMatrix();
}

void TriangleMesh::drawNormals() {
  glBegin(GL_LINES);
    for (unsigned int i = 0; i < normals.size() && i < vertices.size(); i++) {
      Vec3f normalEnd = vertices[i] + normals[i];
      glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
      glVertex3f(normalEnd.x, normalEnd.y, normalEnd.z);
    }
  glEnd();
}

// ===========
// === VFC ===
// ===========

bool TriangleMesh::boundingBoxIsVisible() {
  // openGL stores matrices in column major order:
  // float pm = _11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44
  //     index:  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
  // TODO ...
 
  // default: box is visible => return true
  return true;
}