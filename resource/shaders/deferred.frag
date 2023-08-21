#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;

void main()
{
    FragColor = vec4(texture(ColorBuffer, Texcoord).xyz, 1.0);
}