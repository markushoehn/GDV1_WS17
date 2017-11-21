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
// ========================================================================== //

#include <iostream>
#include <fstream>
#include <float.h>
#include <GL/glut.h>
#include "TriangleMesh.h"

void TriangleMesh::calculateNormals() {
  normals.resize(vertices.size());
  // TODO: calculate normals for each vertex

  // normalize normals
  for (Normals::iterator nit=normals.begin(); nit!=normals.end(); ++nit) {
     //the normalize() function returns a boolean which can be used if you want to check for erroneous normals
	 (*nit).normalize();
   }
}

// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh() {
  clear();
}

TriangleMesh::~TriangleMesh() {
  clear();
}

void TriangleMesh::clear() {
  // clear mesh data
  vertices.clear();
  triangles.clear();
  normals.clear();
}

// ================
// === RAW DATA ===
// ================

vector<Vec3f>& TriangleMesh::getPoints() {
  return vertices;
}
vector<Vec3i>& TriangleMesh::getTriangles() {
	return triangles;
}

vector<Vec3f>& TriangleMesh::getNormals() {
  return normals;
}

void TriangleMesh::flipNormals() {
  for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) {
    (*it) *= -1.0;
  }
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadLSA(const char* filename) {  
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadLSA: can not open " << filename << endl;
    return;
  }
  char s[256];
  in >> s;
  // first word: LSA
  if (!(s[0] == 'L' && s[1] == 'S' && s[2] == 'A')) return;
  // get number of vertices nv, faces nf, edges ne and baseline distance
  int nv, nf, ne;
  float baseline;
  in >> nv;
  in >> nf;
  in >> ne;
  in >> baseline;
  if (nv <= 0 || nf <= 0) return;
  // clear any existing mesh
  clear();
  // read vertices  
  vertices.resize(nv);
  // TODO: read alpha, beta, gamma for each vertex and calculate verticex coordinates

  // read triangles
  triangles.resize(nf);
  // TODO: read all triangles from the file

  // calculate normals
	calculateNormals();
}

void TriangleMesh::loadOFF(const char* filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadOFF: can not find " << filename << endl;
    return;
  }
  char s[256];
  in >> s;
  // first word: OFF
  if (!(s[0] == 'O' && s[1] == 'F' && s[2] == 'F')) return;
  // get number of vertices nv, faces nf, edges ne and baseline distance
  int nv, nf, ne;
  in >> nv;
  in >> nf;
  in >> ne;
  if (nv <= 0 || nf <= 0) return;
  // clear any existing mesh
  clear();
  // read vertices  
  vertices.resize(nv);
  // TODO: read all vertices from the file

  // read triangles
  triangles.resize(nf);
  // TODO: read triangles from the file

  // calculate normals
	calculateNormals();
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::draw() {
  if (triangles.size() == 0) return;
  // TODO: draw triangles with immediate mode
}