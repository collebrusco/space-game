//
//  follower.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#include "follower.h"
using namespace glm;

DifferentialFollower::DifferentialFollower(entID tar, float co){ target = tar; coeff = co;}

void DifferentialFollower::update(ECS& scene, entID selfID) {
    auto& self = scene.getComp<Transform>(selfID);
    auto& tar = scene.getComp<Transform>(target);
    vec3 delta = tar.pos - self.pos;
    delta.z = 0;
    self.pos += (delta * coeff);
}
