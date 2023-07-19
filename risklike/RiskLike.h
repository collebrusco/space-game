//
//  RiskLike.h
//  risklike
//
//  Created by Frank Collebrusco on 6/6/23.
//
//

#ifndef RiskLike_h
#define RiskLike_h
#include "lib/flgl/inc/flgl.h"
#include "lib/flgl/inc/flgl_math.h"
#include "lib/ecs/ECS.h"
#include "lib/sw/Stopwatch.h"
#include "GameEngine.h"
#include "c/all.h"
#include "g/Ships.h"
#include "SkySystem.h"
#include "ActionSystem.h"

class RiskLike : public GameEngine {
public:
    RiskLike() : GameEngine("risklike", 1280, 720) {}
//                 ss(this) {}
private:
//    SkySystem ss;
    void userCreate()           override;
    void userUpdate(float dt)   override;
    void userDestroy()          override;
    
    entID player;
    void camera_init();
    void player_init();
    void roll_system(float dt);
    void fly_system(float dt);
    void bg_init();
    void bg_system();
    
//    ActionSystem actionSystem;
};

#endif /* RiskLike_h */
