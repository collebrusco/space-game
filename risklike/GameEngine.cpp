//
//  GameEngine.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/6/23.
//
//

#include "GameEngine.h"
#include "c/all.h"
static Graphics gl;

Shader GameEngine::shaders[SHADER_LAST + 1];
MeshDetails GameEngine::meshes[MESH_LAST + 1];
TEXTURE_SLOT GameEngine::textures[TEX_LAST + 1];

GameEngine::GameEngine(const char *title, uint32_t w, uint32_t h) : window(gl.initCreateWindow(title, w, h)),
                                                                    launch_timer(ftime::SECONDS),
                                                                    delta_timer(ftime::SECONDS),
                                                                    dt(1.f/60.f)
{
    launch_timer.start();
    gl.loader.setAssetPath("assets/");
    gl.loader.setShaderPath("shaders/");
}

void GameEngine::exit() {
    userDestroy();
    gl.destroy();
}

bool GameEngine::create() { 
    userCreate();
    return true;
}

void GameEngine::start() {
    while (!window.should_close()) {
        gl.clear();
        userUpdate(dt);
        renderer.render_system(this);
        window.update();
        dt = delta_timer.stop_reset_start();
    }
    exit();
}

void GameEngine::Renderer::use_camera(entID cam) {
    camera = cam;
}

Camera* GameEngine::Renderer::fetch_camera(ECS& scene) {
    Camera* cam = scene.tryGetComp<OrthoCamera>(camera);
    if (!cam)
        cam = scene.tryGetComp<PerspectiveCamera>(camera);
    if (!cam)
        throw("renderer does not have a camera!");
    return cam;
}

void GameEngine::Renderer::sync_camera(ECS& scene) {
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

void GameEngine::Renderer::render_system(GameEngine* eng) {
    sync_camera(eng->scene);
    for (auto e : eng->scene.view<Render>()) {
        auto& render = eng->scene.getComp<Render>(e);
        if (Transform* trans = eng->scene.tryGetComp<Transform>(e))
            trans->sync(shaders[render.shader]);
        if (Texture* tex = eng->scene.tryGetComp<Texture>(e))
            tex->sync(shaders[render.shader]);
        shaders[render.shader].bind();
        gl.DrawMesh(meshes[render.vao]);
    }
}

