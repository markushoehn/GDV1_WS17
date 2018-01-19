#include "plane.h"
#include "Vec3.h"
#include <math.h>
#include <stdlib.h>

using namespace std;

class ViewFrustum {
    private:
        float farDist;
        float nearDist;
        float angle;
        float ratio;
        Plane pl[6];
    public:
        ViewFrustum();
        ViewFrustum(float farDist, float nearDist, float angle, float ratio);
        ~ViewFrustum();
        void init(Vec3f p, Vec3f l, Vec3f u);
        bool test(Vec3f p, Vec3f n);
};
