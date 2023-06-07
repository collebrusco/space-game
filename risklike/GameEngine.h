//
//  GameEngine.h
//  risklike
//
//  Created by Frank Collebrusco on 6/6/23.
//
//

#ifndef GameEngine_h
#define GameEngine_h
#include "lib/ecs/ECS.h"
#include "lib/flgl/flgl.h"
#include "lib/sw/Stopwatch.h"
#include "Renderer.h"
#include "renderer_enums.h"

class GameEngine {
public:
    bool create();
    void start();
protected:
    GameEngine(const char* title, uint32_t w = 1280, uint32_t h = 720);
    static Shader shaders[SHADER_LAST + 1];
    static MeshDetails meshes[MESH_LAST + 1];
    static TEXTURE_SLOT textures[TEX_LAST + 1];
    Window& window;
    ECS scene;
    float dt;
    ftime::Stopwatch launch_timer;
    class Renderer {
    private:
        entID camera;
        void sync_camera(ECS& scene);
    public:
    //    Renderer();
    //    ~Renderer();
        void use_camera(entID e);
        Camera* fetch_camera(ECS& scene);
        void render_system(GameEngine* eng);
        void destroy();
    };
    Renderer renderer;
private:
    ftime::Stopwatch delta_timer;
    virtual void userCreate() = 0;
    virtual void userUpdate(float dt) = 0;
    virtual void userDestroy() = 0;
    void exit();
};

#endif /* GameEngine_h */
