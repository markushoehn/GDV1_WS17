#include "Vec3.h"
#include <math.h>
#include <stdlib.h>

using namespace std;

class Plane {
    public:
        Vec3f normal;
        Vec3f point;
        float d;

        Plane();
        Plane(Vec3f v1, Vec3f v2, Vec3f v3);
        ~Plane();
        float distance(Vec3f p);
};
