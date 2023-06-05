//
//  renderer.h
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#ifndef renderer_h
#define renderer_h

#include "lib/flgl/flgl.h"
#include "lib/ecs/ECS.h"
#include "c/transform.h"
#include "c/render.h"
#include "c/texture.h"

/*
 Place render components on each entity to be rendered.
 Select a camera for the renderer to use
 call render system to draw
 */



void renderer_use_camera(entID e);

void render_system(ECS& scene);

#endif /* renderer_h */
