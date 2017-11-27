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
  for (int i = 0; i < normals.size(); ++i)
  {
    normals[i] = Vec3f();
  }

  // ==============
  // Task d) and e): calculate normals for each vertex
  for (int i = 0; i < triangles.size(); ++i)
  {
    // calculate normal of triangle
    Vec3i ti = triangles[i];
    Vec3f e1 = vertices[ti.x] - vertices[ti.y];
    Vec3f e2 = vertices[ti.y] - vertices[ti.z];
    Vec3f e3 = vertices[ti.z] - vertices[ti.x];
    Vec3f cross = e1 ^ (e3 * -1);
    cross.normalize();
    // calculate weighting factors
    e1.normalize();
    e2.normalize();
    float f1 = acos(e1 * (e3 * -1));
    float f2 = acos((e1 * -1) * e2);
    float f3 = acos((e2 * -1) * e3);
    // add weighted normal to all points
    normals[ti.x] += cross * f1;
    normals[ti.y] += cross * f2;
    normals[ti.z] += cross * f3;
  }
  // ==============

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

// TASK b)
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

  // ==============
  // Task b): read alpha, beta, gamma for each vertex and calculate verticex coordinates
  for(int i = 0; i < nv; ++i)
  {
    float alpha, beta, gamma, hb, x, y, z;
    in >> alpha;
    in >> beta;
    in >> gamma;
    alpha = alpha * 2 * M_PI / 360.0f;
    beta = beta * 2 * M_PI / 360.0f;
    gamma = gamma * 2 * M_PI / 360.0f;
    hb = cos(alpha) * (baseline * sin(0.5f * M_PI - beta)) / sin(alpha + beta);
    x = tan(beta) * hb;
    y = sin(gamma) * hb;
    z = cos(gamma) * hb * -1;
    Vec3f tmp(x, y, z);
    vertices[i] = tmp;
  }
  // ==============
  triangles.resize(nf);

  // ==============
  // Task b): read all triangles from the file
  for(int i = 0; i < nf; ++i)
  {
    int n, x, y, z;
    in >> n;
    in >> x;
    in >> y;
    in >> z;
    Vec3i tmp(x, y, z);
    triangles[i] = tmp;
  }
  // ==============

  // calculate normals
	calculateNormals();
}

// TASK b)
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

  // ==============
  // Task a): read all vertices from the file
  for(int i = 0; i < nv; ++i)
  {
    float x, y, z;
    in >> x;
    in >> y;
    in >> z;
    Vec3f tmp(x, y, z);
    vertices[i] = tmp;
  }
  // ==============
  // read triangles
  triangles.resize(nf);

  // ==============
  // Task a): read triangles from the file
  for(int i = 0; i < nf; ++i)
  {
    int n, x, y, z;
    in >> n;
    in >> x;
    in >> y;
    in >> z;
    Vec3i tmp(x, y, z);
    triangles[i] = tmp;
  }
  // ==============

  // calculate normals
  calculateNormals();
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::draw() {
  if (triangles.size() == 0) return;

  // ==============
  // Task c): draw triangles with immediate mode
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < triangles.size(); ++i)
  {
    glVertex3d(vertices[triangles[i].x].x, vertices[triangles[i].x].y, vertices[triangles[i].x].z);
    glVertex3d(vertices[triangles[i].y].x, vertices[triangles[i].y].y, vertices[triangles[i].y].z);
    glVertex3d(vertices[triangles[i].z].x, vertices[triangles[i].z].y, vertices[triangles[i].z].z);
  }
  glEnd();
  // ==============
}
