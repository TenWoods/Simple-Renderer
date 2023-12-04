#version 440 core
layout (location = 0) out float DepthMipmap;
layout (location = 1) out float VisibilityMipMap;

in vec2 Texcoord;

uniform sampler2D Depthmap;
uniform sampler2D VisibilityMap;
uniform ivec2 previousDim;
uniform int previousLevel;

const float nearPlane = 0.1f;
const float farPlane = 1000.0f;

float Linearize(float z)
{
    return (2.0 * nearPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    ivec2 thisLevelTexelCoord = ivec2(gl_FragCoord);
    ivec2 previousTexcoord = 2 * thisLevelTexelCoord;
    vec4 sampledDepth;
    sampledDepth.x = texelFetch(Depthmap, previousTexcoord, previousLevel).r;
    sampledDepth.y = texelFetch(Depthmap, previousTexcoord + ivec2(1, 0), previousLevel).r;
    sampledDepth.z = texelFetch(Depthmap, previousTexcoord + ivec2(1, 1), previousLevel).r;
    sampledDepth.w = texelFetch(Depthmap, previousTexcoord + ivec2(0, 1), previousLevel).r;
    float minDepth = min(min(sampledDepth.x, sampledDepth.y), min(sampledDepth.z, sampledDepth.w));
    float maxZ = max(max(sampledDepth.x, sampledDepth.y), max(sampledDepth.z, sampledDepth.w));
    bool isOddColumn = ((previousDim.x & 1) != 0);
    bool isOddRow = ((previousDim.y & 1) != 0);
    if (isOddColumn)
    {
        vec2 columnDepth;
        columnDepth.x = texelFetch(Depthmap, previousTexcoord + ivec2(2, 0), previousLevel).r;
        columnDepth.y = texelFetch(Depthmap, previousTexcoord + ivec2(2, 1), previousLevel).r;
        if (isOddRow)
        {
            float cornerDepth = texelFetch(Depthmap, previousTexcoord + ivec2(2, 2), previousLevel).r;
            minDepth = min(minDepth, cornerDepth);
        }
        minDepth = min(min(minDepth, columnDepth.x), columnDepth.y);
    }
    if (isOddRow)
    {
        vec2 rowDepth;
        rowDepth.x = texelFetch(Depthmap, previousTexcoord + ivec2(0, 2), previousLevel).r;
        rowDepth.y = texelFetch(Depthmap, previousTexcoord + ivec2(1, 2), previousLevel).r;
        minDepth = min(min(rowDepth.x, minDepth), rowDepth.y);
    }
    vec4 visibility;
    if (previousLevel == 0)
    {
        visibility = vec4(1.0);
    }
    else
    {
        visibility.x = texelFetch(VisibilityMap, previousTexcoord, previousLevel).r;
        visibility.y = texelFetch(VisibilityMap, previousTexcoord + ivec2(1, 0), previousLevel).r;
        visibility.z = texelFetch(VisibilityMap, previousTexcoord + ivec2(0, 1), previousLevel).r;
        visibility.w = texelFetch(VisibilityMap, previousTexcoord + ivec2(1, 1), previousLevel).r;
    }
    sampledDepth.x = Linearize(sampledDepth.x);
    sampledDepth.y = Linearize(sampledDepth.y);
    sampledDepth.z = Linearize(sampledDepth.z);
    sampledDepth.w = Linearize(sampledDepth.w);
    maxZ = Linearize(maxZ);
    vec4 integration = (sampledDepth / maxZ) * visibility;
    float coarseVisibility = dot(vec4(0.25), integration);
    VisibilityMipMap = coarseVisibility;
    DepthMipmap = minDepth;
}