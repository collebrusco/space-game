//
//  RiskLike.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/6/23.
//
//

#include "RiskLike.h"
#include "assets/models.h"
#include <iostream>
using namespace glm;
static Graphics gl;

void RiskLike::camera_init() {
    auto e = scene.newEntity();
    auto& cam = scene.addComp<OrthoCamera>(e);
    auto& tf = scene.addComp<Transform>(e, cam.readPos());
    scene.addComp<DifferentialFollower>(e, player, 0.3);
    cam.setViewWidth(5.f);
    cam.update();
    renderer.use_camera(e);
}

void RiskLike::player_init() {
    auto e = scene.newEntity();
    player = e;
    auto const& asp = ShipTypeObject::getShip(SHIP_ASP);
    scene.addComp<Transform>(e);
    scene.addComp<Render>(e, asp.getMesh(), asp.getShader());
    scene.addComp<Texture>(e, asp.getTexture());
//    scene.addComp<Actor*>(e, new PlayerActor(e));
}
void RiskLike::userCreate() {
    meshes[MESH_ASP] = gl.loader.UploadMesh(asp_mesh);
    meshes[MESH_TILE] = gl.std.getTileMesh();
    shaders[SHADER_SHIP] = gl.loader.UploadShader("mvp_vert", "tex_frag");
    shaders[SHADER_STARS] = gl.loader.UploadShader("fullscreen_vert", "star2d_frag");
    ShipTypeObject::initShips();
    gl.setDepthTestEnable(true);
    player_init();
    camera_init();
    bg_init();
}
void RiskLike::userUpdate(float dt) {
    if (window.keyboard[GLFW_KEY_F].pressed){
        static bool toggle = false;
        gl.setWireframe(toggle ^= 0x01);
    }
    if (window.mouse.scroll.y != 0.f) {
        ((OrthoCamera*)renderer.fetch_camera(scene))->getViewWidth() += window.mouse.scroll.y;
    }
    
    std::cout << 1 / dt << "fps\n";
    
//    actionSystem.execute(dt);
    roll_system(dt);
    fly_system(dt);
    DifferentialFollower::follower_system(scene);

    bg_system();
}

void RiskLike::userDestroy() {
    
}

void RiskLike::roll_system(float dt){
    static const float rmax = 15.f;
    static const float coeff = 2.;
    auto& win = gl.getWindow();
    auto& trans = scene.getComp<Transform>(player);
    if (win.keyboard[GLFW_KEY_A].down){
        float delta = coeff * (rmax - abs(trans.rotation.y));
        trans.rotation.y -= delta * dt;
    } else if (win.keyboard[GLFW_KEY_D].down){
        float delta = coeff * (rmax - abs(trans.rotation.y));
        trans.rotation.y += delta * dt;
    } else if (trans.rotation.y != 0){
        trans.rotation.y += ((5. * dt) * (-trans.rotation.y));
    }

    if (win.keyboard[GLFW_KEY_W].down){
        float delta = coeff * (rmax - abs(trans.rotation.x));
        trans.rotation.x += delta * dt;
    } else if (win.keyboard[GLFW_KEY_S].down){
        float delta = coeff * (rmax - abs(trans.rotation.x));
        trans.rotation.x -= delta * dt;
    } else if (trans.rotation.x != 0){
        trans.rotation.x += ((5. * dt) * (-trans.rotation.x));
    }
}

void RiskLike::fly_system(float dt) {
    static const float ACCEL_MAX = 0.1f;
    auto& win = gl.getWindow();
    auto& trans = scene.getComp<Transform>(player);
    static vec3 velo = vec3(0);
    static float av = 0;
    if (win.keyboard[GLFW_KEY_A].down) {
        av += 4 * dt;
    } else if (win.keyboard[GLFW_KEY_D].down){
        av -= 4 * dt;
    }

    if (win.keyboard[GLFW_KEY_W].down) {
        velo = velo + glm::vec3((angleToVector(trans.rotation.z + 90.f) * ACCEL_MAX) * dt, 0.f);
    } else if (win.keyboard[GLFW_KEY_S].down){
        velo = velo - glm::vec3((angleToVector(trans.rotation.z + 90.f) * ACCEL_MAX) * dt, 0.f);
    }
    
    if (win.keyboard[GLFW_KEY_SPACE].down) {
        velo = velo - ((velo / length(velo)) * ACCEL_MAX * dt);
    }
    
    trans.pos += velo;
    trans.rotation.z += av;
}

void RiskLike::bg_init() {
    auto e = scene.newEntity();
    scene.addComp<Render>(e, MESH_TILE, SHADER_STARS);
}

void RiskLike::bg_system() {
    static vec2 pGamePos;
    Camera* cam = renderer.fetch_camera(scene);
    shaders[SHADER_STARS].uFloat("uTime", launch_timer.read());
    shaders[SHADER_STARS].uVec2("uRes", vec2(gl.getWindow().frame.x, gl.getWindow().frame.y));
    shaders[SHADER_STARS].uFloat("uAspect", gl.getWindow().aspect);
    shaders[SHADER_STARS].uVec2("uGamePos", vec2(cam->readPos().x, cam->readPos().y) / 5.f);
    shaders[SHADER_STARS].uVec2("uPGamePos", pGamePos / 5.f);
    shaders[SHADER_STARS].uFloat("uZoom", ((OrthoCamera*)cam)->readViewWidth() / 10.f);
    pGamePos = vec2(cam->readPos().x, cam->readPos().y);
}
