// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universitšt Darmstadt                                          //
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
	// TODO: create VBOs
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
	// TODO: draw in array mode
}

void TriangleMesh::drawVBO() {
	if (VBOsupported == false) return;
	if (VBOv == 0 || VBOn == 0 || VBOf == 0) return;
	// TODO: draw in VBO mode
}