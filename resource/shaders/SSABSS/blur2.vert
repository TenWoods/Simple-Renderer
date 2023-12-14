#version 440 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texcoord;

void main()
{

    // SSABSS Blur1 Implementation <-- Blur3/vert.c
    // Vert.texcoord = vec2(inTexCoord.x * m_Texw, inTexCoord.y * m_Texh);
    texcoord = vec2(aTexCoords.x, aTexCoords.y);

    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
