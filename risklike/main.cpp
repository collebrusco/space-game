//
//  main.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/2/23.
//
#include <iostream>

#include "lib/flgl/flgl.h"
#include "lib/ecs/ECS.h"
#include "lib/sw/Stopwatch.h"
#include "State.h"

#include "assets/models.h"
#include "Renderer.h"
#include "c/follower.h"

using namespace ftime;
using namespace glm;
//Stopwatch launchSW(SECONDS);

static Graphics gl;
static State state;
static Renderer renderer;


//void graphics_init() {
//    gl.init();
//    gl.loader.setAssetPath("assets/");
//    gl.loader.setShaderPath("shaders/");
//    gl.createWindow("Risklike Dev", 1280, 720);
//    gl.setDepthTestEnable(true);
////        gl.setClearColor(0.2, 0.1, 0.2, 1.f);
//}
//vec3* campos;
//entID ortho_camera_init(){
//    auto e = state.scene.newEntity();
//    auto& cam = state.scene.addComp<OrthoCamera>(e);
//    auto& tf = state.scene.addComp<Transform>(e, cam.readPos());
////    campos = &tf.pos;
//    state.scene.addComp<DifferentialFollower>(e, state.player, 0.3);
//    cam.setViewWidth(5.f);
//    cam.update();
//    renderer_use_camera(e);
//    return e;
//}

//entID spec_camera_init(){
//    auto e = state.scene.newEntity();
//    auto& tar = state.scene.getComp<Transform>(state.player);
//    auto& cam = state.scene.addComp<PerspectiveCamera>(e, tar.pos + vec3(0., -1, 1), (tar.pos + vec3(0., 0., 0.5)) - (tar.pos + vec3(0., -1, 1)), vec3(0., 0., 1.), 0.001, 10000., 90);
//    auto& tf = state.scene.addComp<Transform>(e, cam.readPos());
//    campos = &tf.pos;
//    cam.update();
//    renderer_use_camera(e);
//    return e;
//}

//void ocamera_system() {
//    for (auto e : state.scene.view<OrthoCamera>()){
//        static float prev_rotation = -1;
//        auto& cam = state.scene.getComp<OrthoCamera>(e);
//        auto tf = state.scene.tryGetComp<Transform>(e);
//        if (tf && tf->pos != cam.readPos())
//            cam.getPos() = tf->pos;
////            if (tf && tf->rotation.z != prev_rotation)
////                cam.getUp() = vec3(angleToVector(prev_rotation + 90.f), 0.f);
//        auto& win = gl.getWindow();
//        if (win.mouse.scroll.y != 0){
//            cam.getViewWidth() += win.mouse.scroll.y;
//        }
//        cam.update();
//        gl.forEachShader([&](Shader s)->void{
//            s.uMat4("uProj", cam.proj());
//            s.uMat4("uView", cam.view());
//        });
//        prev_rotation = state.scene.getComp<Transform>(state.player).rotation.z;
//    }
//}

void roll_system(float dt){
    static const float rmax = 15.f;
    static const float coeff = 2.;
    auto& win = gl.getWindow();
    auto& trans = state.scene.getComp<Transform>(state.player);
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

void fly_system(float dt) {
    static const float ACCEL_MAX = 0.1f;
    auto& win = gl.getWindow();
    auto& trans = state.scene.getComp<Transform>(state.player);
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
//        std::cout << trans.pos.x << ", " << trans.pos.y << "\n";
}

void bg_init() {
    auto e = state.scene.newEntity();
    state.scene.addComp<Render>(e, MESH_TILE, SHADER_STARS);
}

void bg_system() {
    static vec2 pGamePos;
    Camera* cam = renderer.fetch_camera(state.scene);
    state.shaders[SHADER_STARS].uFloat("uTime", state.launch_timer.read());
    state.shaders[SHADER_STARS].uVec2("uRes", vec2(gl.getWindow().frame.x, gl.getWindow().frame.y));
    state.shaders[SHADER_STARS].uFloat("uAspect", gl.getWindow().aspect);
    state.shaders[SHADER_STARS].uVec2("uGamePos", vec2(cam->readPos().x, cam->readPos().y) / 5.f);
    state.shaders[SHADER_STARS].uVec2("uPGamePos", pGamePos / 5.f);
    state.shaders[SHADER_STARS].uFloat("uRotation", glm::radians(state.scene.getComp<Transform>(state.player).rotation.z));
    pGamePos = vec2(cam->readPos().x, cam->readPos().y);

//        uniform float uTime;
//        uniform vec2 uRes;
//        uniform float uAspect;
//
//        uniform vec2 uGamePos;
}

void init() {
    state.init();
    state.load_meshes();
    state.load_shaders();
    renderer.use_camera(state.camera_init());
    bg_init();
//    ortho_camera_init();
//    spec_camera_init();
    
//    auto e = state.scene.newEntity();
//    state.scene.addComp<Transform>(e);
//    state.scene.addComp<Render>(e, gl.std.getTileMesh(), asp.shad);
}

void loop(){
    state.delta_timer.start();
    Window& window = gl.getWindow();
    while (!window.should_close()){
        gl.clear();

        if (window.keyboard[GLFW_KEY_F].pressed){
            static bool toggle = false;
            gl.setWireframe(toggle ^= 0x01);
        }

        roll_system(state.dt);
        fly_system(state.dt);
        DifferentialFollower::follower_system(state.scene);

        bg_system();
        renderer.render_system(state.scene);


        window.update();
        state.dt = state.delta_timer.stop_reset_start();
    }
}

void destroy(){
    gl.destroy();
}


int main(int argc, const char * argv[]) {
    std::cout << "spec camera is " << sizeof(PerspectiveCamera) << " bytes\northo camera is " << sizeof(OrthoCamera) << " bytes\n";
    
    init();
    loop();
    destroy();
}
