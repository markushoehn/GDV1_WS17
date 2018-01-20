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
        Vec3f getVertexP(Vec3f p, Vec3f n, Vec3f normal);
        Vec3f getVertexN(Vec3f p, Vec3f n, Vec3f normal);
    public:
        ViewFrustum();
        ViewFrustum(float angle, float ratio, float nearDist, float farDist);
        ~ViewFrustum();
        void init(Vec3f p, Vec3f l, Vec3f u);
        bool test(Vec3f p, Vec3f n);
};
