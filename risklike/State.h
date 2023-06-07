//
//  State.h
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#ifndef State_h
#define State_h

#include "lib/ecs/ECS.h"
#include "lib/sw/Stopwatch.h"
#include "lib/flgl/flgl.h"
#include "g/ShipTypes.h"
#include "renderer_enums.h"

/*
 so the idea is to have the shared game data in one place, such as
 the scene, what entityID is the player, dt,...
 the state also has all these initialization functions for now
 these are meant to get the graphics open, and emplace the starting entities
 in the future, potentially virtualize these methods so multiple states can be defined this way
 and a state machine could easily be used for levels, menus, other states
 */

struct State {
    float dt;
    ECS scene;
    ftime::Stopwatch delta_timer;
    ftime::Stopwatch launch_timer;
    entID player;
    
    static Shader shaders[SHADER_LAST + 1];
    static MeshDetails meshes[MESH_LAST + 1];
    
    State();
    void init();
    static void load_meshes();
    static void load_shaders();
    void graphics_init();
    void open_window();
    entID camera_init();
    void player_init();
};

#endif /* State_h */
