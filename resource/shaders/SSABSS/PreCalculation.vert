#version 440 core

in vec3 inPosition;

varying vert
{
    vec4 MVvertex;
    vec4 WorldViewPosition;
    vec4 WorldViewProjectionPosition;
}Vert;


void main()
{
    vec4 modelSpacePos = vec4(inPosition, 1.0);

    vec4 worldPos = TransformWorld(modelSpacePos);

    // SSABSS Penumbra Calculation <-- ShadowAndScene/vert.c
    Vert.MVvertex = worldPos;
    Vert.WorldViewPosition = TransformWorldView(modelSpacePos);
    gl_Position = TransformWorldViewProjection(modelSpacePos);
    Vert.WorldViewProjectionPosition = gl_Position;
}
