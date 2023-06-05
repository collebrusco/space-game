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

#include "assets/models.h"
#include "renderer.h"
#include "c/follower.h"

using namespace ftime;
using namespace glm;
Stopwatch launchSW(SECONDS);

struct ShipType {
    MeshDetails mesh;
    Texture tex;
    Shader shad;
    ShipType() = default;
};

// ================================================================ COMPS




// ================================================================ GAME

struct Risklike {
    float dt;
    Graphics gl;
    ECS scene;
    Stopwatch delta_timer;
    Shader bgshad;

    ShipType asp;

    entID player;

    Risklike() : delta_timer(SECONDS) {}

    void ships_init(){
        asp.shad = gl.loader.UploadShader("mvp_vert", "tex_frag");
        asp.tex.slot = gl.loader.UploadTexture("atlas", true);
        asp.tex.dims = glm::ivec2(256);
        asp.tex.pos = glm::vec2(0.);
        asp.mesh = gl.loader.UploadMesh(asp_mesh);
    }

    void graphics_init() {
        gl.init();
        gl.loader.setAssetPath("assets/");
        gl.loader.setShaderPath("shaders/");
        gl.createWindow("Risklike Dev", 1280, 720);
        gl.setDepthTestEnable(true);
//        gl.setClearColor(0.2, 0.1, 0.2, 1.f);
    }
    vec3* campos;
    entID ortho_camera_init(){
        auto e = scene.newEntity();
        auto& cam = scene.addComp<OrthoCamera>(e);
        auto& tf = scene.addComp<Transform>(e, cam.readPos());
        campos = &tf.pos;
        scene.addComp<DifferentialFollower>(e, player, 0.3);
        cam.setViewWidth(5.f);
        cam.update();
        renderer_use_camera(e);
        return e;
    }

    entID spec_camera_init(){
        auto e = scene.newEntity();
        auto& tar = scene.getComp<Transform>(player);
        auto& cam = scene.addComp<PerspectiveCamera>(e, tar.pos + vec3(0., -1, 1), (tar.pos + vec3(0., 0., 0.5)) - (tar.pos + vec3(0., -1, 1)), vec3(0., 0., 1.), 0.001, 10000., 90);
        auto& tf = scene.addComp<Transform>(e, cam.readPos());
        cam.update();
        renderer_use_camera(e);
        return e;
    }

    entID player_init(){
        auto e = scene.newEntity();
        player = e;

        scene.addComp<Transform>(e);
        scene.addComp<Render>(e, asp.mesh, asp.shad);
        scene.addComp<Texture>(e, asp.tex);

        return e;
    }

    void pcamera_system(float dt) {
        constexpr static const vec3 offset = vec3(0., -1, 1);
        for (auto e : scene.view<PerspectiveCamera>()){
            auto& cam = scene.getComp<PerspectiveCamera>(e);
            auto& win = gl.getWindow();
            auto& tar = scene.getComp<Transform>(player);
            if (cam.readPos() != tar.pos + offset) {
                cam.getPos() = ((tar.pos + offset)) - cam.readPos() * dt;
                cam.getLook() = (tar.pos + vec3(0., 0., 0.5)) - cam.readPos();
            }

            cam.update();
            gl.forEachShader([&](Shader s)->void{
                s.uMat4("uProj", cam.proj());
                s.uMat4("uView", cam.view());
            });
        }
    }

    void ocamera_system() {
        for (auto e : scene.view<OrthoCamera>()){
            static float prev_rotation = -1;
            auto& cam = scene.getComp<OrthoCamera>(e);
            auto tf = scene.tryGetComp<Transform>(e);
            if (tf && tf->pos != cam.readPos())
                cam.getPos() = tf->pos;
//            if (tf && tf->rotation.z != prev_rotation)
//                cam.getUp() = vec3(angleToVector(prev_rotation + 90.f), 0.f);
            auto& win = gl.getWindow();
            if (win.mouse.scroll.y != 0){
                cam.getViewWidth() += win.mouse.scroll.y;
            }
            cam.update();
            gl.forEachShader([&](Shader s)->void{
                s.uMat4("uProj", cam.proj());
                s.uMat4("uView", cam.view());
            });
            prev_rotation = scene.getComp<Transform>(player).rotation.z;
        }
    }

    void follower_system() {
        for (auto e : scene.view<DifferentialFollower>()){
            scene.getComp<DifferentialFollower>(e).update(scene, e);
        }
    }

    void roll_system(float dt){
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

    void fly_system(float dt) {
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
//        std::cout << trans.pos.x << ", " << trans.pos.y << "\n";
    }

    

    void bg_init() {
        auto e = scene.newEntity();
        bgshad = gl.loader.UploadShader("fullscreen_vert", "star2d_frag");
        scene.addComp<Render>(e, gl.std.getTileMesh(), bgshad);
    }

    void bg_system() {
        bgshad.uFloat("uTime", launchSW.read());
        bgshad.uVec2("uRes", vec2(gl.getWindow().frame.x, gl.getWindow().frame.y));
        bgshad.uFloat("uAspect", gl.getWindow().aspect);
        bgshad.uVec2("uGamePos", vec2(campos->x, campos->y));
        bgshad.uFloat("uRotation", glm::radians(scene.getComp<Transform>(player).rotation.z));

//        uniform float uTime;
//        uniform vec2 uRes;
//        uniform float uAspect;
//
//        uniform vec2 uGamePos;
    }

    void init() {
        launchSW.start();
        dt = 1/60;
        graphics_init();
        ships_init();
        player_init();
        bg_init();
        ortho_camera_init();
//        spec_camera_init();
        
        auto e = scene.newEntity();
        scene.addComp<Transform>(e);
        scene.addComp<Render>(e, gl.std.getTileMesh(), asp.shad);
    }

    void loop(){
        delta_timer.start();
        Window& window = gl.getWindow();
        while (!window.should_close()){
            gl.clear();

            if (window.keyboard[GLFW_KEY_F].pressed){
                static bool toggle = false;
                gl.setWireframe(toggle ^= 0x01);
            }

            roll_system(dt);
            fly_system(dt);
            follower_system();
            ocamera_system();
            bg_system();
            render_system(scene);


            window.update();
            dt = delta_timer.stop_reset_start();
        }
    }

    void destroy(){
        gl.destroy();
    }

};


int main(int argc, const char * argv[]) {
    std::cout << "spec camera is " << sizeof(PerspectiveCamera) << " bytes\northo camera is " << sizeof(OrthoCamera) << " bytes\n";
    
    Risklike risk;
    risk.init();
    risk.loop();
    risk.destroy();
}
