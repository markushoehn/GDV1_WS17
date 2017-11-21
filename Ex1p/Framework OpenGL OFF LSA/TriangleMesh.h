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
#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include "Vec3.h"

#define M_PI 3.14159265358979f

using namespace std;

class TriangleMesh  {

private:

  // typedefs for data
  typedef Vec3i Triangle;
  typedef Vec3f Normal;
  typedef Vec3f Vertex;
  typedef vector<Triangle> Triangles;
  typedef vector<Normal> Normals;
  typedef vector<Vertex> Vertices;  

  // data of TriangleMesh
  Vertices vertices;
  Normals normals;
  Triangles triangles;

  // private methods
  void calculateNormals();

public:

  // ===============================
  // === CONSTRUCTOR, DESTRUCTOR ===
  // ===============================

  TriangleMesh();
  ~TriangleMesh();

  // clears all data, sets defaults
  void clear();

  // ================
  // === RAW DATA ===
  // ================

  // get raw data references
  vector<Vec3f>& getPoints();
  vector<Vec3i>& getTriangles();
  vector<Vec3f>& getNormals();

  // flip all normals
  void flipNormals();

  // =================
  // === LOAD MESH ===
  // =================

  // read from an LSA file. also calculates normals.
  void loadLSA(const char* filename);

  // read from an OFF file. also calculates normals.
  void loadOFF(const char* filename);

  // ==============
  // === RENDER ===
  // ==============
  
  // draw mesh with set transformation
  void draw();

};


#endif

