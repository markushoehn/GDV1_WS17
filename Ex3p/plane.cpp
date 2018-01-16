#include "plane.h"
#include "Vec3.h"

Plane::Plane(Vec3f v1, Vec3f v2, Vec3f v3){
    Vec3f a1, a2;
    a1 = v1 - v2;
    a2 = v3 - v2;
    normal = a2 ^ a1;
    normal.normalize();
    point = v2;
    d = -(normal * point);
}
Plane::~Plane(){}

float Plane::distance(Vec3f p){
    return (d + normal * p);
}
