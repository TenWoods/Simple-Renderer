#version 440 core

attribute vec2 inTexCoord;
attribute vec3 inPosition;
attribute vec3 inNormal;

uniform float m_Texw;
uniform float m_Texh;

varying vert {
    vec2 texcoord;
}Vert;

void main() {
    vec4 modelSpacePos = vec4(inPosition, 1.0);

    // SSABSS Blur1 Implementation <-- Blur3/vert.c
    Vert.texcoord = vec2(inTexCoord.x, inTexCoord.y);

    gl_Position = TransformWorldViewProjection(modelSpacePos);
}
