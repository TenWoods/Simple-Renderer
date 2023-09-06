#version 440 core

out vec4 DepthMipmap;

in vec2 Texcoord;

uniform sampler2D Depthmap;
uniform ivec2 previousDim;
uniform int previousLevel;

void main()
{
    vec2 previousTexcoord = 2.0 * Texcoord;
    vec4 sampledDepth;
    sampledDepth.x = textureLod(Depthmap, previousTexcoord, previousLevel).r;
    sampledDepth.y = textureLod(Depthmap, previousTexcoord + vec2(1.0, 0.0), previousLevel).r;
    sampledDepth.z = textureLod(Depthmap, previousTexcoord + vec2(1.0, 1.0), previousLevel).r;
    sampledDepth.w = textureLod(Depthmap, previousTexcoord + vec2(0.0, 1.0), previousLevel).r;
    float minDepth = min(min(sampledDepth.x, sampledDepth.y), min(sampledDepth.z, sampledDepth.w));
    bool isOddColumn = ((previousDim.x & 1) != 0);
    bool isOddRow = ((previousDim.y & 1) != 0);
    if (isOddColumn)
    {

    }
}