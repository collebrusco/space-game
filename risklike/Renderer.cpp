//
//  renderer.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#include "Renderer.h"
#include "assets/models.h"

/*
 renderer fetches camera its given
 updates position if entity also has transform
 syncs all shaders with camera data
 then renders, which first uploads tex data and model mat if those comps exist
 */

static Graphics gl;



void Renderer::destroy() {}

void Renderer::use_camera(entID cam) {
    camera = cam;
}

Camera* Renderer::fetch_camera(ECS& scene) {
    Camera* cam = scene.tryGetComp<OrthoCamera>(camera);
    if (!cam)
        cam = scene.tryGetComp<PerspectiveCamera>(camera);
    if (!cam)
        throw("renderer does not have a camera!");
    return cam;
}

void Renderer::sync_camera(ECS& scene) {
    Camera* cam = fetch_camera(scene);
    
    Transform* trans = scene.tryGetComp<Transform>(camera);
    if (trans && trans->pos != cam->readPos())
        cam->getPos() = trans->pos;
    
    cam->update();
    gl.forEachShader([&](Shader s)->void {
        s.uMat4("uView", cam->view());
        s.uMat4("uProj", cam->proj());
    });
}

void Renderer::render_system(ECS& scene) {
    sync_camera(scene);
    for (auto e : scene.view<Render>()) {
        auto& render = scene.getComp<Render>(e);
        if (Transform* trans = scene.tryGetComp<Transform>(e))
            trans->sync(State::shaders[render.shader]);
        if (Texture* tex = scene.tryGetComp<Texture>(e))
            tex->sync(State::shaders[render.shader]);
        State::shaders[render.shader].bind();
        gl.DrawMesh(State::meshes[render.vao]);
    }
}
