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
//   * draw, drawOutlined                                                     //
//   * transformations                                                        //
// ========================================================================== //

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Vec3.h"
#include <algorithm>

#define M_PI 3.14159265358979f

using namespace std;

class TriangleMesh  {

private:

  // typedefs for data
  typedef Vec3ui Triangle;
  typedef Vec3f Vertex;
  typedef Vec3f Normal;  
  typedef Vec3f Color;
  struct TexCoord { float u, v; };

  typedef vector<Triangle> Triangles;
  typedef vector<Vertex> Vertices;
  typedef vector<Normal> Normals;
  typedef vector<Color> Colors;
  typedef vector<TexCoord> TexCoords;

  // data of TriangleMesh
  Vertices vertices;    // vertex positions
  Normals normals;      // normals per vertex
  Triangles triangles;  // indices of vertices that form a triangle
  Colors colors;        // r,g,b in [0,1]
  TexCoords texCoords;  // u,v in [0,1]

  // VBO ids for vertices, normals, faces, colors, texCoords
  bool VBOsupported;
  unsigned int VBOv, VBOn, VBOf, VBOc, VBOt;

  // draw mode data
  unsigned int drawMode;
  bool withColorArray;
  bool withBB;
  bool withNormals;
  bool withTexture;

  // bounding box data
  Vec3f boundingBoxMin;
  Vec3f boundingBoxMax;
  Vec3f boundingBoxMid;
  Vec3f boundingBoxSize;

  // ===============================
  // === CONSTRUCTOR, DESTRUCTOR ===
  // ===============================

public:
    
  int textureID = -1;

  TriangleMesh();
  ~TriangleMesh();

  // clears all data, sets defaults
  void clear();

  // prints some information
  void coutData();

  // ================
  // === RAW DATA ===
  // ================

  // get raw data references
  vector<Vec3f>& getVertices() { return vertices; }
  vector<Vec3ui>& getTriangles() { return triangles; }
  vector<Vec3f>& getNormals() { return normals; }
  vector<Vec3f>& getColors() { return colors; }
  vector<TexCoord>& getTexCoords() { return texCoords; }

  // get size of all elements
  unsigned int getNumVertices() { return vertices.size(); }
  unsigned int getNumNormals() { return normals.size(); }
  unsigned int getNumTriangles() { return triangles.size(); }
  unsigned int getNumColors() { return colors.size(); }
  unsigned int getNumTexCoords() { return texCoords.size(); }

  // get boundingBox data
  Vec3f getBoundingBoxMin() { return boundingBoxMin; }
  Vec3f getBoundingBoxMax() { return boundingBoxMax; }
  Vec3f getBoundingBoxMid() { return boundingBoxMid; }
  Vec3f getBoundingBoxSize() { return boundingBoxSize; }

  // flip all normals
  void flipNormals();

  // translates vertices so that the bounding box center is at newBBmid
  void translateToCenter(const Vec3f& newBBmid, bool createVBOs = true);

  // scales vertices so that the largest bounding box size has length newLength
  void scaleToLength(const float newLength, bool createVBOs = true);

  // =================
  // === LOAD MESH ===
  // =================

  // read from an OFF file. also calculates normals if not given in the file.
  void loadOFF(const char* filename, bool createVBOs = true);

  // read from an OFF file. also calculates normals if not given in the file.
  // translates and scales vertices with bounding box center at BBmid and largest side BBlength
  void loadOFF(const char* filename, const Vec3f& BBmid, const float BBlength);

  void generateHeightmap(float bbox_height = 100.0f, float bbox_plane = 1000.0f);

private:

  // calculate normals, weighted by area
  void calculateNormalsByArea();

  // calculate texture coordinates by central projection
  void calculateTexCoordsSphereMapping();

  // calculates axis aligned bounding box data
  void calculateBB();

  // check VBO support
  void checkVBOSupport();

  // create VBOs for vertices, faces, normals, colors, textureCoords
  void createAllVBOs();

  // create VBO
  GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage);  

  // clean up VBO data (delete from gpu memory)
  void cleanupVBO();  

  // ==============
  // === RENDER ===
  // ==============

public:

  // sets draw mode. 0=immediate, 1=array, 2=vbo
  void setDrawMode(unsigned int _drawMode = 2);

  // enable or disable colors
  void toggleWithColorArray();

  // enable or disable colors
  void toggleWithTexture();

  // enable or disable drawing of the bounding box
  void toggleWithBB();

  // enable or disable drawing normals
  void toggleWithNormals();

  // draw mesh with current drawing mode settings. returns the number of triangles drawn.
  unsigned int draw();
  
private:

  // draw mesh with immediate mode
  void drawImmediate();

  // draw mesh with vertex array
  void drawArray(); 

  // draw VBO
  void drawVBO();

  // draw the bounding box (wired, immediate mode) (withBB)
  void drawBB();

  // draw object normals (lines, immediate mode) (withNormals)
  void drawNormals();

  // ===========
  // === VFC ===
  // ===========

  // check if bounding box is visible in view frustum
  bool boundingBoxIsVisible();

};


#endif

