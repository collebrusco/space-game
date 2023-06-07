//
//  renderer_enums.h
//  risklike
//
//  Created by Frank Collebrusco on 6/5/23.
//

#ifndef renderer_enums_h
#define renderer_enums_h

#define SHADER_LAST SHADER_STARS
enum ShaderName {
    SHADER_SHIP = 0,
    SHADER_STARS
};

#define MESH_LAST MESH_TILE
enum MeshName {
    MESH_ASP = 0,
    MESH_TILE = 1
};

#define TEX_LAST TEX_ATLAS
enum TextureName {
    TEX_ATLAS = 0
};

#endif /* renderer_enums_h */
