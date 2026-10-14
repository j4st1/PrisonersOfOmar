#pragma once

inline constexpr const char* MAIN_VERT = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uLightSpace;
uniform mat3 uNormalMat;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 vLightSpacePos;

void main()
{
    vec4 world = uModel * vec4(aPos, 1.0);
    vWorldPos      = world.xyz;
    vNormal        = uNormalMat * aNormal;
    vUV            = aUV;
    vLightSpacePos = uLightSpace * world;
    gl_Position    = uProj * uView * world;
}
)GLSL";

inline constexpr const char* MAIN_FRAG = R"GLSL(
#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 vLightSpacePos;

out vec4 FragColor;

uniform sampler2D uDiffuse;
uniform sampler2D uShadowMap;
uniform vec3  uLightDir;      // direction the light travels
uniform vec3  uLightColor;
uniform vec3  uViewPos;
uniform vec3  uTint;
uniform float uAmbient;

float shadowFactor(vec3 N, vec3 L)
{
    vec3 proj = vLightSpacePos.xyz / vLightSpacePos.w;
    proj = proj * 0.5 + 0.5;

    if (proj.z > 1.0 || proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0)
        return 0.0;

    float bias = max(0.0035 * (1.0 - dot(N, L)), 0.0006);

    vec2 texel = 1.0 / vec2(textureSize(uShadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float d = texture(uShadowMap, proj.xy + vec2(x, y) * texel).r;
            shadow += (proj.z - bias > d) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);

    vec3 base = texture(uDiffuse, vUV).rgb * uTint;

    float diff = max(dot(N, L), 0.0);
    float sh   = shadowFactor(N, L);

    vec3 ambient = uAmbient * base;
    vec3 direct  = (1.0 - sh) * diff * uLightColor * base;

    // slight rim/back light so shadows are not pitch black
    float back = max(dot(N, -L), 0.0) * 0.15;

    vec3 color = ambient + direct + back * base;

    FragColor = vec4(color, 1.0);
}
)GLSL";

inline constexpr const char* DEPTH_VERT = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uLightSpace;

void main()
{
    gl_Position = uLightSpace * uModel * vec4(aPos, 1.0);
}
)GLSL";

inline constexpr const char* DEPTH_FRAG = R"GLSL(
#version 330 core
void main() { }
)GLSL";
