//
//  main.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/2/23.
//
#include <iostream>

#include "flgl/flgl.h"
#include "ecs/ECS.h"
#include "sw/Stopwatch.h"

#include "assets/models.h"

using namespace ftime;
using namespace glm;
Stopwatch launchSW(SECONDS);

struct ShipType {
    MeshDetails mesh;
    TEXTURE_SLOT tex;
    Shader shad;
};

// ================================================================ COMPS

struct Transform3D {
    vec3 pos, rotation, scale, anchor;
    Transform3D(vec3 p = vec3(0.f), vec3 r = vec3(0.f), vec3 s = vec3(1.f), vec3 a = vec3(0.f))
    { pos = p; rotation = r; scale = s; anchor = a; }
};

struct DifferentialFollower {
    entID target;
    float coeff;
    DifferentialFollower(entID tar, float co){ target = tar; coeff = co;}
    void update(ECS& scene, entID selfID){
        auto& self = scene.getComp<Transform3D>(selfID);
        auto& tar = scene.getComp<Transform3D>(target);
        vec3 delta = tar.pos - self.pos;
        delta.z = 0;
        self.pos += (delta * coeff);
    }
};

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
        asp.shad = gl.loader.UploadShader("vert", "frag");
        asp.tex = gl.loader.UploadTexture("asp", true);
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
        auto& tf = scene.addComp<Transform3D>(e, cam.readPos());
        campos = &tf.pos;
        scene.addComp<DifferentialFollower>(e, player, 0.3);
        cam.setViewWidth(5.f);
        cam.update();
        return e;
    }
    
    entID spec_camera_init(){
        auto e = scene.newEntity();
        auto& tar = scene.getComp<Transform3D>(player);
        auto& cam = scene.addComp<PerspectiveCamera>(e, tar.pos + vec3(0., -1, 1), (tar.pos + vec3(0., 0., 0.5)) - (tar.pos + vec3(0., -1, 1)), vec3(0., 0., 1.), 0.001, 10000., 90);
        auto& tf = scene.addComp<Transform3D>(e, cam.readPos());
        cam.update();
        return e;
    }
    
    entID player_init(){
        auto e = scene.newEntity();
        player = e;
        
        scene.addComp<Transform3D>(e);
        scene.addComp<Shader>(e, asp.shad);
        scene.addComp<MeshDetails>(e, asp.mesh);
        scene.addComp<TEXTURE_SLOT>(e, asp.tex);
        
        return e;
    }
    
    void pcamera_system(float dt) {
        constexpr static const vec3 offset = vec3(0., -1, 1);
        for (auto e : scene.view<PerspectiveCamera>()){
            auto& cam = scene.getComp<PerspectiveCamera>(e);
            auto& win = gl.getWindow();
            auto& tar = scene.getComp<Transform3D>(player);
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
            auto& cam = scene.getComp<OrthoCamera>(e);
            auto tf = scene.tryGetComp<Transform3D>(e);
            if (tf && tf->pos != cam.readPos()){
                cam.getPos() = tf->pos;
            }
            auto& win = gl.getWindow();
            if (win.mouse.scroll.y != 0){
                cam.getViewWidth() += win.mouse.scroll.y;
            }
            cam.update();
            gl.forEachShader([&](Shader s)->void{
                s.uMat4("uProj", cam.proj());
                s.uMat4("uView", cam.view());
            });
        }
    }
    
    void follower_system() {
        for (auto e : scene.view<DifferentialFollower>()){
            scene.getComp<DifferentialFollower>(e).update(scene, e);
        }
    }
    
    void render_system() {
        for (auto e : scene.view<MeshDetails, Shader>()){
            auto& mesh = scene.getComp<MeshDetails>(e);
            auto& shader = scene.getComp<Shader>(e);
            if (Transform3D* trans = scene.tryGetComp<Transform3D>(e)){
                shader.uMat4("uModel", genModelMat3d(trans->pos, trans->rotation, trans->scale, trans->anchor));
            }
            if (TEXTURE_SLOT* tex = scene.tryGetComp<TEXTURE_SLOT>(e)){
                shader.uInt("uTexslot", *tex);
            }
            shader.bind();
            gl.DrawMesh(mesh);
        }
    }
    
    void roll_system(float dt){
        static const float rmax = 15.f;
        static const float coeff = 2.;
        auto& win = gl.getWindow();
        auto& trans = scene.getComp<Transform3D>(player);
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
        auto& win = gl.getWindow();
        auto& trans = scene.getComp<Transform3D>(player);
        static vec3 velo = vec3(0);
        if (win.keyboard[GLFW_KEY_A].down){
            velo -= vec3(.1, 0, 0) * dt;
        } else if (win.keyboard[GLFW_KEY_D].down){
            velo += vec3(.1, 0, 0) * dt;
        }
        
        if (win.keyboard[GLFW_KEY_W].down){
            velo += vec3(0, .1, 0) * dt;
        } else if (win.keyboard[GLFW_KEY_S].down){
            velo -= vec3(0, .1, 0) * dt;
        }
        trans.pos += velo;
//        std::cout << trans.pos.x << ", " << trans.pos.y << "\n";
    }
    
    void star_system() {
        auto& trans = scene.getComp<Transform3D>(player);
        vec2 gp = vec2(trans.pos.x, trans.pos.y);
        
    }
    
    void bg_init() {
        auto e = scene.newEntity();
        scene.addComp<MeshDetails>(e, gl.std.getTileMesh());
        bgshad = gl.loader.UploadShader("fullscreen_vert", "star2d_frag");
        scene.addComp<Shader>(e, bgshad);
    }
    
    void bg_system() {
        bgshad.uFloat("uTime", launchSW.read());
        bgshad.uVec2("uRes", vec2(gl.getWindow().frame.x, gl.getWindow().frame.y));
        bgshad.uFloat("uAspect", gl.getWindow().aspect);
        bgshad.uVec2("uGamePos", vec2(campos->x, campos->y));
        
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
            render_system();
            
            
            window.update();
            dt = delta_timer.stop_reset_start();
        }
    }
    
    void destroy(){
        gl.destroy();
    }
    
};


int main(int argc, const char * argv[]) {
    Risklike risk;
    risk.init();
    risk.loop();
    risk.destroy();
}
