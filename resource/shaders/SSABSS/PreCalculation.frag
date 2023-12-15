#version 440 core

#define PI 3.14159265

layout(location = 0, index = 0) out vec4 out_1;
layout(location = 1, index = 0) out vec4 out_2;
layout(location = 2, index = 0) out vec4 out_3;

in vec2 texcoord;

uniform mat4 m_LightViewProjectionMatrix0;
// For SSABSS
uniform mat4 m_LightViewMatrix0;

uniform vec3 m_LightPos;
uniform vec3 m_LightDir;

uniform float m_LightSize;

uniform mat4 inverseView;
uniform mat4 view;
uniform mat4 projection;

// #endif

const mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0);

vec2 Possion32[32] = vec2[](
vec2(-0.975402, -0.0711386), vec2(-0.920347, -0.41142), vec2(-0.883908, 0.217872),
vec2(-0.884518, 0.568041), vec2(-0.811945, 0.90521), vec2(-0.792474, -0.779962),
vec2(-0.614856, 0.386578), vec2(-0.580859, -0.208777), vec2(-0.53795, 0.716666),
vec2(-0.515427, 0.0899991), vec2(-0.454634, -0.707938), vec2(-0.420942, 0.991272),
vec2(-0.261147, 0.588488), vec2(-0.211219, 0.114841), vec2(-0.146336, -0.259194),
vec2(-0.139439, -0.888668), vec2(0.0116886, 0.326395), vec2(0.0380566, 0.625477),
vec2(0.0625935, -0.50853), vec2(0.125584, 0.0469069), vec2(0.169469, -0.997253),
vec2(0.320597, 0.291055), vec2(0.359172, -0.633717), vec2(0.435713, -0.250832),
vec2(0.507797, -0.916562), vec2(0.545763, 0.730216), vec2(0.56859, 0.11655),
vec2(0.743156, -0.505173), vec2(0.736442, -0.189734), vec2(0.843562, 0.357036),
vec2(0.865413, 0.763726), vec2(0.872005, -0.927));


uniform sampler2D m_SSABSSshadowMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;

//varying prim
//{
//    vec3 Normal;
//    vec3 NormView;
//    vec4 Position; //screen space position
//    vec4 MVvertex;
//    vec4 WorldViewPosition; //world space position
//}Prim;

void main()
{
    vec3 normal = texture(NormalMap, texcoord).xyz;
    vec4 worldPosition = texture(PositionMap, texcoord);
    vec3 viewNormal = vec3(vec4(normal, 0.0) * inverseView);

    vec4 L = vec4(-m_LightPos, 1);

    vec4 posView = view * worldPosition;
    vec4 screenPos = projection * posView;
    screenPos /= screenPos.w;
    vec4 posLight = m_LightViewMatrix0 * worldPosition;

    float depLight = -posLight.z;
    posLight = m_LightViewProjectionMatrix0 * worldPosition;
    vec3 projCoord = (posLight.xyz / posLight.w) * 0.5 + vec3(0.5);

    float zAtShadowMap = texture(m_SSABSSshadowMap, projCoord.xy).x;

    float zFromLight = projCoord.z;

    vec3 Lvec = normalize(L.xyz / L.w - posView.xyz / posView.w);
    // vec3 Lvec = m_LightDir;

    // Hardshadow
    float shadow = zAtShadowMap < zFromLight - 0.0015 ? 0:1.0;

    float blocker = 0;
    int count = 0;

    // light size = 13
    float range = m_LightSize * 10 * projCoord.z;
    // Possion Sampling 32
    for (int i = 0; i < 32; i++)
    {
        vec2 zAtShadowMap_tmp = texture(m_SSABSSshadowMap, projCoord.xy + 0.001 * range * Possion32[i]).xy;
        // zAtShadowMap_tmp = texture(m_SSABSSshadowMap, projCoord.xy).xy; // ?????????
        if (zAtShadowMap_tmp.x < zFromLight - 0.0015)
        {
            blocker += zAtShadowMap_tmp.y;
            count++;
        }
    }

    blocker /= count;
    if (count == 0) {
        count = 1;
        blocker = texture(m_SSABSSshadowMap, projCoord.xy).y;
    }

    float zeye = abs(posView.z) * .2;

    float w_p = m_LightSize * (depLight - blocker) / (blocker * zeye);
    w_p = abs(w_p);
    float theta;
    float px = posView.x;
    float py = posView.y;
    float pz = posView.z;
    float nx = viewNormal.x;
    float ny = viewNormal.y;
    float nz = viewNormal.z;

    float F = (nx * px + ny * py) * (nx * px + ny * py) + nz * nz * (px * px + py * py - w_p * w_p);
    float E = -2 * (nx * nz * px * py + nx * ny * px * pz - ny * nz * px * px + (nz * nz + ny * ny) * py * pz + ny * nz * w_p * w_p);
    float D = -2 * (ny * nz * px * py + nx * ny * py * pz - nx * nz * py * py + (nz * nz + nx * nx) * px * pz + nx * nz * w_p * w_p);
    float C = (2 * nx * ny * pz * pz - 2 * ny * nz * px * pz - 2 * nx * nz * py * pz - 2 * (nx * nx + ny * ny) * px * py - 2 * nx * ny * w_p * w_p);
    float B = ((nx * nx + ny * ny) * px * px + (ny * ny + nz * nz) * pz * pz + 2 * nx * nz * px * pz - ny * ny * w_p * w_p);
    float A = ((nx * nx + ny * ny) * py * py + (nx * nx + nz * nz) * pz * pz + 2 * ny * nz * py * pz - nx * nx * w_p * w_p);

    if (abs(B - A) < 0.00001)
    {
        theta = 0.0;
    }
    else
    {
        theta = (1 / 2.0 * atan(C / (B - A)));
    }
    float Ad = A * cos(theta) * cos(theta) + B * sin(theta) * sin(theta) - C * sin(theta) * cos(theta);
    float Bd = A * sin(theta) * sin(theta) + B * cos(theta) * cos(theta) + C * sin(theta) * cos(theta);

    float nsx = (E * C - 2 * D * B) / (C * C - 4 * A * B);
    float nsy = (D * C - 2 * E * A) / (C * C - 4 * A * B);
    float K = A * nsx * nsx + B * nsy * nsy + C * nsx * nsy - D * nsx - E * nsy + F;

    float r1 = sqrt(-K / Ad);
    float r2 = sqrt(-K / Bd);

    r1 = max(r1 * 10, 0.2);
    r2 = max(r2 * 10, 0.2);

    vec2 axis1 = vec2(cos(-theta), sin(-theta)) * 0.5 + 0.5;
    vec2 axis2 = vec2(-sin(-theta), cos(-theta)) * 0.5 + 0.5;

    // out_1 = vec4(Prim.Position.z, 0, 0, 1);
    out_1 = vec4(shadow, r1 * 0.5, r2 / r1, abs(screenPos.z / 100));
    out_2 = vec4(axis1, axis2);
    out_3 = vec4(1, 1, 1, max(0, dot(normal, Lvec)));
}
