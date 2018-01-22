// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// ========================================================================== //

#include "Vec3.h"

template<class T>
class Ray {

  typedef Vec3<T> Vec3_;

 public:
	 	 
  Vec3_ d,o;

  Ray(const T origin[3], const T p[3])
    {
      o[0] = origin[0];
      o[1] = origin[1];
      o[2] = origin[2];

      d[0] = p[0] - o[0];
      d[1] = p[1] - o[1];
      d[2] = p[2] - o[2];

      d = d.normalized();
    }

  int  triangleIntersect(const T p0[3],const T p1[3],const T p2[3],T& u, T& v, T &rayt) const
   {
     Vec3_ e1(p1[0]-p0[0],
	         p1[1]-p0[1],
	         p1[2]-p0[2]);

     Vec3_ e2(p2[0]-p0[0],
	         p2[1]-p0[1],
	         p2[2]-p0[2]);

     Vec3_ t(o[0]-p0[0],
	        o[1]-p0[1],
	        o[2]-p0[2]);

     Vec3_ p = d^e2;
     Vec3_ q = t^e1;

     T d1 = p*e1;
     if (fabs(d1) < 10e-7)
       return 0;

     T f = 1.0f/d1;

     u = f*(p*t);

     if (u < 0 || u > 1.0)
       return 0;

     v = f*(q*d);

     if (v < 0.0 || v > 1.0 || (u+v) > 1.0)
       return 0;

     rayt = f*(q*e2);

     return 1;
   }


};
