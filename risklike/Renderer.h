//
//  Renderer.h
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#ifndef Renderer_h
#define Renderer_h

#include "lib/flgl/flgl.h"
#include "lib/ecs/ECS.h"
#include "c/transform.h"
#include "c/render.h"
#include "c/texture.h"
#include "renderer_enums.h"
//#include "State.h"

/*
 Place render components on each entity to be rendered.
 Select a camera for the renderer to use
 call render system to draw
 */

//class Renderer {
//private:
//    entID camera;
//    void sync_camera(ECS& scene);
//public:
////    Renderer();
////    ~Renderer();
//    void use_camera(entID e);
//    Camera* fetch_camera(ECS& scene);
//    void render_system(GameEngine* eng);
//    void destroy();
//};

#endif /* Renderer_h */
