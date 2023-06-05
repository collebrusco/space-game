//
//  render.h
//  risklike
//
//  Created by Frank Collebrusco on 6/4/23.
//
//

#ifndef render_h
#define render_h
#include "../lib/flgl/flgl.h"

struct Render {
    MeshDetails vao;
    Shader shader;
    Render(MeshDetails v, Shader s);
};

#endif /* render_h */
