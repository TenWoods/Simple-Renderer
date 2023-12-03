#version 440 core

out float Visibility;

in vec2 Texcoord;

uniform sampler2D DepthMap;
uniform sampler2D VisibilityMap;
uniform ivec2 previousDim;
uniform int previousLevel;

void main()
{

}