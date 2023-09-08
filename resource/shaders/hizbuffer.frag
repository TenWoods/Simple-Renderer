#version 440 core

out vec4 DepthMipmap;

in vec2 Texcoord;

uniform sampler2D Depthmap;
uniform ivec2 previousDim;
uniform int previousLevel;

float pack_depth()
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
        vec2 columnDepth;
        columnDepth.x = textureLod(Depthmap, previousTexcoord + vec2(2.0, 0.0), previousLevel).r;
        columnDepth.y = textureLod(Depthmap, previousTexcoord + vec2(2.0, 1.0), previousLevel).r;
        if (isOddRow)
        {
            float cornerDepth = textureLod(Depthmap, previousTexcoord + vec2(2.0, 2.0), previousLevel).r;
            minDepth = min(minDepth, cornerDepth);
        }
        minDepth = min(min(minDepth, columnDepth.x), columnDepth.y);
    }
    if (isOddRow)
    {
        vec2 rowDepth;
        rowDepth.x = textureLod(Depthmap, previousTexcoord + vec2(0.0, 2.0), previousLevel).r;
        rowDepth.y = textureLod(Depthmap, previousTexcoord + vec2(1.0, 2.0), previousLevel).r;
        minDepth = min(min(rowDepth.x, minDepth), rowDepth.y);
    }
    return minDepth;
}

void main()
{

}