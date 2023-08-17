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
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D AOMap;
uniform sampler2D OtherMap;

void main()
{
    vec3 baseColor = texture(NormalMap, fs_in.Texcoord).xyz;
    FragColor = vec4(baseColor, 1.0);
}