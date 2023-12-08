#version 440 core

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat3 m_NormView;
// For compare with Zheng's SSABSS only.
// uniform mat4 m_ViewInv;

in vert
{
    vec4 MVvertex;
    vec4 WorldViewPosition;
    vec4 WorldViewProjectionPosition;
}Vert[];

out prim
{
    vec3 Normal;
    vec3 NormView;
    vec4 Position;
    vec4 MVvertex;
    vec4 WorldViewPosition;
}Prim;

void main()
{
    vec3 A = Vert[2].MVvertex.xyz - Vert[0].MVvertex.xyz;
    vec3 B = Vert[1].MVvertex.xyz - Vert[0].MVvertex.xyz;
    // For compare with Zheng's SSABSS only.
    // vec3 A = (m_ViewInv * (Vert[2].MVvertex.xyzw - Vert[0].MVvertex.xyzw)).xyz;
    // vec3 B = (m_ViewInv * (Vert[1].MVvertex.xyzw - Vert[0].MVvertex.xyzw)).xyz;
    Prim.Normal = normalize(cross(A, B));

    for (int i = 0; i < gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;

        Prim.Position = gl_in[i].gl_Position;
        Prim.MVvertex = Vert[i].MVvertex;

        Prim.NormView =  m_NormView * normalize(cross(A, B));

        Prim.WorldViewPosition = Vert[i].WorldViewPosition;
        EmitVertex();
    }

    EndPrimitive();
}

