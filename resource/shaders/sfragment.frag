#version 440 core

out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec2 Texcoord;
} fs_in;

uniform vec3 lightPos;

uniform sampler2D BaseColorMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicRoughnessMap;
uniform sampler2D AOMap;
uniform sampler2D OtherMap;

void main()
{
    vec3 baseColor = texture(MetallicRoughnessMap, fs_in.Texcoord).xyz;
    FragColor = vec4(baseColor, 1.0);
}