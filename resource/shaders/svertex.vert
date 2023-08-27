#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT
{
    vec3 FragPos;
    vec4 ClipPos;
    vec3 Normal;
    vec3 Tangent;
    vec2 Texcoord;
} vs_out;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.ClipPos = projection * view * worldPos;
    gl_Position = vs_out.ClipPos;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.Tangent = aTangent;
    vs_out.Texcoord = aTex;

}