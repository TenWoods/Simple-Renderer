#version 440 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

varying vert
{
    vec2 texcoord;
}Vert;

void main()
{
    vec4 modelSpacePos = vec4(inPosition, 1.0);

    // SSABSS Blur1 Implementation <-- Blur3/vert.c
    Vert.texcoord = vec2(aTexCoords.x, aTexCoords.y);

    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}