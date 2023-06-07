//
//  State.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#include "State.h"
#include "c/all.h"
#include "assets/models.h"
static Graphics gl;

Shader State::shaders[SHADER_LAST + 1];
MeshDetails State::meshes[MESH_LAST + 1];

State::State() : delta_timer(ftime::SECONDS), launch_timer(ftime::SECONDS), dt(1.f/60.f) { delta_timer.start(); launch_timer.start(); }

void State::load_meshes() {
    meshes[MESH_ASP] = gl.loader.UploadMesh(asp_mesh);
    meshes[MESH_TILE] = gl.std.getTileMesh();
}

void State::load_shaders() {
    shaders[SHADER_SHIP] = gl.loader.UploadShader("mvp_vert", "tex_frag");
    shaders[SHADER_STARS] = gl.loader.UploadShader("fullscreen_vert", "star2d_frag");
}

void State::init() {
    graphics_init();
    open_window();
    ShipTypeObject::initShips();
    player_init();
    camera_init();
}

void State::graphics_init() { 
    gl.init();
    gl.loader.setAssetPath("assets/");
    gl.loader.setShaderPath("shaders/");
}

void State::open_window() {
    gl.createWindow("Risklike Dev", 1280, 720);
    gl.setDepthTestEnable(true);
}

entID State::camera_init() {
    auto e = scene.newEntity();
    auto& cam = scene.addComp<OrthoCamera>(e);
    auto& tf = scene.addComp<Transform>(e, cam.readPos());
    scene.addComp<DifferentialFollower>(e, player, 0.3);
    cam.setViewWidth(5.f);
    cam.update();
//    renderer.use_camera(e);
    return e;
}

void State::player_init() {
    auto e = scene.newEntity();
    player = e;
    auto const& asp = ShipTypeObject::getShip(SHIP_ASP);
    scene.addComp<Transform>(e);
    scene.addComp<Render>(e, asp.getMesh(), asp.getShader());
    scene.addComp<Texture>(e, asp.getTexture());
}




