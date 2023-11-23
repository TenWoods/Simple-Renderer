#version 440 core

layout (location = 0) out vec4 ColorBuffer;
layout (location = 1) out vec4 NormalBuffer;
layout (location = 2) out float DepthBuffer;
layout (location = 3) out vec4 PositionBuffer;

in VS_OUT
{
    vec3 FragPos;
    vec4 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec2 Texcoord;
} fs_in;

uniform sampler2D BaseColorMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicRoughnessMap;

const float PI = 3.14159265359;
const float near = 0.1;
const float far  = 1000.0;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(NormalMap, fs_in.Texcoord).xyz * 2.0 - 1.0;
    //    vec3 Q1 = dFdx(fs_in.FragPos);
    //    vec3 Q2 = dFdy(fs_in.FragPos);
    //    vec2 st1 = dFdx(fs_in.Texcoord);
    //    vec2 st2 = dFdy(fs_in.Texcoord);

    vec3 N = normalize(fs_in.Normal);
    vec3 T = normalize(fs_in.Tangent);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 normal = normalize(TBN * tangentNormal);
    return normal;
}

void main()
{
    vec3 baseColor = texture(BaseColorMap, fs_in.Texcoord).xyz;
    vec3 mrTex = texture(MetallicRoughnessMap, fs_in.Texcoord).xyz;
    float metallic = mrTex.x;
    float roughness = mrTex.y;
    vec3 normal = getNormalFromMap();
    //vec3 normal = fs_in.Normal;
//    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
//    vec3 F0 = vec3(0.04);
//    F0 = mix(F0, baseColor, metallic);
//    vec3 Lo = vec3(0.0);
//    Lo += calculate_point(baseColor, F0, viewDir, normal, metallic, roughness);
//
//    vec3 ambient = vec3(0.1) * baseColor;
    //vec3 color = ambient + Lo;
    //color = pow(color, vec3(1.0/2.2));
    //float depth = LinearizeDepth(gl_FragCoord.z) / 100.0;
    float depth = gl_FragCoord.z;

    ColorBuffer = vec4(baseColor, metallic);
    NormalBuffer = vec4(normal, roughness);
    DepthBuffer = depth;
    PositionBuffer = vec4(fs_in.FragPos, 1.0);
}