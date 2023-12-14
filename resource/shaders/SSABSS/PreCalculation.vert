#version 440 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texcoord;

//varying vert
//{
//    vec4 MVvertex;
//    vec4 WorldViewPosition;
//    vec4 WorldViewProjectionPosition;
//}Vert;

void main()
{
//    vec4 modelSpacePos = vec4(inPosition, 1.0);
//
//    vec4 worldPos = TransformWorld(modelSpacePos);

    // SSABSS Penumbra Calculation <-- ShadowAndScene/vert.c
//    Vert.MVvertex = worldPos;
//    Vert.WorldViewPosition = TransformWorldView(modelSpacePos);
//    gl_Position = TransformWorldViewProjection(modelSpacePos);
//    Vert.WorldViewProjectionPosition = gl_Position;
    texcoord = vec2(aTexCoords.x, aTexCoords.y);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
