// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// ========================================================================= //

#ifndef CLIPPLANE_H
#define CLIPPLANE_H

#include "Vec3.h"

using namespace std;

class ClipPlane {

  Vec3f planeNormal;
  float planeDistance;

public:

  // constructor which normalizes the plane = ax + by + cz + d = 0
  ClipPlane(const float a, const float b, const float c, const float d) {
    planeNormal.set(a,b,c);
    float l = planeNormal.length();
    planeNormal /= l;
    planeDistance = d / l;
  }

  float evaluatePoint(const Vec3f& p) {
    return planeNormal*p + planeDistance;
  }

};

#endif