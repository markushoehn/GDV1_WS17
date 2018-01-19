#include "viewFrustum.h"
#define ANG2RAD 3.14159265358979323846/180.0

ViewFrustum::ViewFrustum(){}
ViewFrustum::ViewFrustum(float farDist, float nearDist, float angle, float ratio) : farDist(farDist), nearDist(nearDist), angle(angle), ratio(ratio){

}
ViewFrustum::~ViewFrustum(){}

void ViewFrustum::init(Vec3f pos, Vec3f dir){
  Vec3f p = pos;
  Vec3f d = dir;
  Vec3f u = Vec3f(0.0,1.0,0.0);
  Vec3f l = p + d;

  Vec3f Z = p - l; // z axis
  Z.normalize();
  Vec3f X = Vec3f(0.0,Z.y,0.0); // x axisof camera
  X.normalize();
  Vec3f Y = Vec3f(0.0,Z.y * X.y,0.0); // up

  float tang = (float)tan(ANG2RAD * angle * 0.5) ;
  float nh = nearDist * tang;
  float nw = nh * ratio;
  float fh = farDist  * tang;
  float fw = fh * ratio;

  // compute the centers of the near and far planes
  Vec3f nc = p - Z * nearDist;
  Vec3f fc = p - Z * farDist;

  // compute the 4 corners of the frustum on the near plane
  Vec3f ntl = nc + Y * nh - X * nw;
  Vec3f ntr = nc + Y * nh + X * nw;
  Vec3f nbl = nc - Y * nh - X * nw;
  Vec3f nbr = nc - Y * nh + X * nw;

  // compute the 4 corners of the frustum on the far plane
  Vec3f ftl = fc + Y * fh - X * fw;
  Vec3f ftr = fc + Y * fh + X * fw;
  Vec3f fbl = fc - Y * fh - X * fw;
  Vec3f fbr = fc - Y * fh + X * fw;

  ViewFrustum::pl[0] = Plane(ntr,ntl,ftl);
  ViewFrustum::pl[1] = Plane(nbl,nbr,fbr);
  ViewFrustum::pl[2] = Plane(ntl,nbl,fbl);
  ViewFrustum::pl[3] = Plane(nbr,ntr,fbr);
  ViewFrustum::pl[4] = Plane(ntl,ntr,nbr);
  ViewFrustum::pl[5] = Plane(ftr,ftl,fbl);
}

bool ViewFrustum::test(Vec3f p, Vec3f n){
  bool INSIDE = true;
  bool OUTSIDE = false;
  bool INTERSECT = true;

  bool result = INSIDE;
  // for each plane
  for(int i=0; i < 6; i++) {

    // is the positive vertex outside?
    if (ViewFrustum::pl[i].distance(p) < 0)
      return OUTSIDE;

    // is the negative vertex outside?
    else if (ViewFrustum::pl[i].distance(n) < 0)
      result =  INTERSECT;
  }
  return result;
}