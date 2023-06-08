//
//  RiskLike.h
//  risklike
//
//  Created by Frank Collebrusco on 6/6/23.
//
//

#ifndef RiskLike_h
#define RiskLike_h
#include "lib/flgl/flgl.h"
#include "lib/ecs/ECS.h"
#include "lib/sw/Stopwatch.h"
#include "GameEngine.h"
#include "c/all.h"
#include "g/Ships.h"

class RiskLike : public GameEngine {
public:
    RiskLike() : GameEngine("risklike", 1280, 720) {}
private:
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
};

#endif /* RiskLike_h */
