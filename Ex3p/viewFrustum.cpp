#include "viewFrustum.h"
#define ANG2RAD 3.14159265358979323846/180.0

ViewFrustum::ViewFrustum(){}
ViewFrustum::ViewFrustum(float angle, float ratio, float nearDist, float farDist) : farDist(farDist), nearDist(nearDist), angle(angle), ratio(ratio){

}
ViewFrustum::~ViewFrustum(){}

void ViewFrustum::init(Vec3f p, Vec3f l, Vec3f u){

  Vec3f Z = p - l; // z axis
  Z.normalize();
  Vec3f X = u ^ Z; // x axisof camera
  X.normalize();
  Vec3f Y = Z ^ X; // up

  float tang = (float)tan(ANG2RAD * angle * 0.5);
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

  ViewFrustum::pl[0] = Plane(ntr, ntl, ftl);
  ViewFrustum::pl[1] = Plane(nbl, nbr, fbr);
  ViewFrustum::pl[2] = Plane(ntl, nbl, fbl);
  ViewFrustum::pl[3] = Plane(nbr, ntr, fbr);
  ViewFrustum::pl[4] = Plane(ntl, ntr, nbr);
  ViewFrustum::pl[5] = Plane(ftr, ftl, fbl);
}

bool ViewFrustum::test(Vec3f p, Vec3f n){
  bool INSIDE = true;
  bool OUTSIDE = false;
  bool INTERSECT = true;

  bool result = INSIDE;
  // for each plane
  for(int i=0; i < 6; i++) {

    // is the positive vertex outside?
    if (pl[i].distance(getVertexP(p, n, pl[i].normal)) < 0)
      return OUTSIDE;

    // is the negative vertex outside?
    else if (pl[i].distance(getVertexN(p, n, pl[i].normal)) < 0)
      result =  INTERSECT;
  }
  return result;
}


Vec3f ViewFrustum::getVertexP(Vec3f p, Vec3f n, Vec3f normal){
    // max
    Vec3f res = n;
    if (normal.x <= 0)
        res.x = p.x;
    if (normal.y <= 0)
        res.y = p.y;
    if (normal.z <= 0)
        res.z = p.z;
    return res;
}

Vec3f ViewFrustum::getVertexN(Vec3f p, Vec3f n, Vec3f normal){
    // min
    Vec3f res = p;
    if (normal.x >= 0)
        res.x = n.x;
    if (normal.y >= 0)
        res.y = n.y;
    if (normal.z >= 0)
        res.z = n.z;
    return res;
}