#version 440 core

layout(location = 0) out vec4 out_1;

varying vert
{
    vec4 LightViewPosition;
    vec4 LightViewProjectionPosition;
}Vert;

void main()
{
    float z = (Vert.LightViewProjectionPosition.z / Vert.LightViewProjectionPosition.w) * 0.5 + 0.5;
    // float z = (Vert.LightViewProjectionPosition.z / Vert.LightViewProjectionPosition.w);
    out_1 = vec4(z, abs(Vert.LightViewPosition.z), 0, 1);
}
