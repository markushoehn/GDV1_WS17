// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes           //
//   * readOFF                                                               //
//   * draw                                                                  //
//   * transformations                                                       //
// ==========================================================================//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <float.h>
#include "TriangleMesh.h"

// =========================
// === PRIVATE FUNCTIONS ===
// =========================

void TriangleMesh::calculateNormals() {
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
		// cross product
		normal = vec1 ^ vec2; 
		// sum up in vertices (=> weighted by area)
		normals[id0] += normal;
		normals[id1] += normal;
		normals[id2] += normal;
	}
	for (Normals::iterator nit=normals.begin(); nit!=normals.end(); ++nit){
		//the normalize() function returns a boolean which can be used if you want to check for erroneous normals
		(*nit).normalize();
	}
}

void TriangleMesh::createAllVBOs() {
	// check if VBOs are supported
	if(glGenBuffers && glBindBuffer && glBufferData && glBufferSubData &&
		glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv)
		VBOsupported = true;
	else {
		VBOsupported = false;
		cout << "VBOs are not supported!" << endl;
	}
	
	// exercise b)

	// TODO: if this is loaded the array drawing doesnt work anymore?
	if(VBOsupported) {

		int vertexSize = 3*vertices.size()*sizeof(float);
		int normalSize = 3*normals.size()*sizeof(float);
		int triangleSize = 3*triangles.size()*sizeof(float);

		glGenBuffers(1, &VBOv);
		glBindBuffer(GL_ARRAY_BUFFER, VBOv);
		glBufferData(GL_ARRAY_BUFFER, vertexSize, &vertices[0], GL_STATIC_DRAW);
		
		glGenBuffers(1, &VBOn);
		glBindBuffer(GL_ARRAY_BUFFER, VBOn);
		glBufferData(GL_ARRAY_BUFFER, normalSize, &normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &VBOf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleSize, &triangles[0], GL_STATIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void TriangleMesh::cleanupVBO() {
	// delete VBO
	if (VBOsupported) {
		if (VBOv != 0) glDeleteBuffers(1, &VBOv);
		if (VBOn != 0) glDeleteBuffers(1, &VBOn);
		if (VBOf != 0) glDeleteBuffers(1, &VBOf);
	}
	VBOv = 0;
	VBOn = 0;
	VBOf = 0;
}

// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh() {
	// initialize VBO IDs with 0
	VBOv = 0;
	VBOn = 0;
	VBOf = 0;
	VBOsupported = false;
	// clear all data
	clear();
}

TriangleMesh::~TriangleMesh() {
	// clear all data
	clear();
}

void TriangleMesh::clear() {
	// clear mesh data
	vertices.clear();
	triangles.clear();
	normals.clear();
	// clear bounding box data
	boundingBoxMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
	boundingBoxMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	boundingBoxMid.clear();
	boundingBoxSize.clear();
	// clear VBO data
	cleanupVBO();
}

// ================
// === RAW DATA ===
// ================

void TriangleMesh::flipNormals() {
	for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) (*it) *= -1.0;
}

void TriangleMesh::translateToCenter(const Vec3f& newBBmid) {
	Vec3f trans = newBBmid - boundingBoxMid;
	for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] += trans;  
	boundingBoxMin += trans;
	boundingBoxMax += trans;
	boundingBoxMid += trans;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

void TriangleMesh::scaleToLength(const float newLength) {
	float length = max(max(boundingBoxSize.x, boundingBoxSize.y), boundingBoxSize.z);
	float scale = newLength / length;
	for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] *= scale;
	boundingBoxMin *= scale;
	boundingBoxMax *= scale;
	boundingBoxMid *= scale;
	boundingBoxSize *= scale;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadOFF(const char* filename) {
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
	// clear any existing mesh
	clear();
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
	if (!noff) calculateNormals();
	// createVBO
	createAllVBOs();
}

void TriangleMesh::loadOFF(const char* filename, const Vec3f& BBmid, const float BBlength) {
	loadOFF(filename);  
	translateToCenter(BBmid);
	scaleToLength(BBlength);
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::drawImmediate() {
	if (triangles.size() == 0) return;
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < triangles.size(); i++) {
		for (unsigned int v = 0; v < 3; v++) {
			glNormal3f( normals[triangles[i][v]].x,  normals[triangles[i][v]].y,  normals[triangles[i][v]].z);
			glVertex3f(vertices[triangles[i][v]].x, vertices[triangles[i][v]].y, vertices[triangles[i][v]].z);
		}
	}
	glEnd();
}

void TriangleMesh::drawArray() {
	if (triangles.size() == 0) return;

	// exercise 2a)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	// glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
	// size: The number of vertex coordinates, 2 for 2D points, 3 for 3D points.
	// type: GL_FLOAT, GL_SHORT, GL_INT or GL_DOUBLE.
	// stride: The number of bytes to offset to the next vertex (used for interleaved array).
	// pointer: The pointer to the vertex array.
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	
	// glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
	// type: GL_FLOAT, GL_SHORT, GL_INT or GL_DOUBLE.
	// stride: The number of bytes to offset to the next normal (used for interleaved array).
	// pointer: The pointer to the vertex array.
	glNormalPointer(GL_FLOAT, 0, &normals[0]);

 	// glDrawElements() requires 4 parameters.
	// The first one is the type of primitive,
	// the second is the number of indices of index array,
	// the third is data type of index array
	// the last parameter is the address of index array

	glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, &triangles[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void TriangleMesh::drawVBO() {
	if (VBOsupported == false) return;
	if (VBOv == 0 || VBOn == 0 || VBOf == 0) return;
	// TODO: draw in VBO mode
	// exercise 2b)
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);
    glIndexPointer(GL_UNSIGNED_INT, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);
	glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, 0); // last parameter is offset

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



/*

// exercise 2c)

averaged FPS over ~10 sec
1024100 triangles. (delphin.off, normal shader)
Intel(R) Core(TM) i3 CPU       M 370  @ 2.40GHz


Immediate
0.007254109846153848

Array
0.02880962641666666

VBO
0.06143070797101453

VBO is 2 times faster then Array Mode.
VBO is 8 times faster then Immediate Mode.
Array Mode is nearly 4 times faster then Immediate Mode.



bear modell
Immediate
0.002692334

Array
0.006124764

VBO
0.007063694

*/

