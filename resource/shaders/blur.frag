#version 440 core
out vec4 FragColor;

in vec2 Texdcoord;

uniform sampler2D ColorBuffer;
uniform bool isVertical;
uniform int previousLevel;

const float WEIGHT_0 = 0.001;
const float WEIGHT_1 = 0.028;
const float WEIGHT_2 = 0.233;
const float WEIGHT_3 = 0.474;

void main()
{
    ivec2 thisLevelTexelCoord = ivec2(gl_FragCoord);
    ivec2 previousTexcoord = 2 * thisLevelTexelCoord;
    vec3 color;
    if (isVertical)
    {
        color = vec3(0.0);
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, 3), previousLevel).xyz * WEIGHT_0;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, 2), previousLevel).xyz * WEIGHT_1;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, 1), previousLevel).xyz * WEIGHT_2;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, 0), previousLevel).xyz * WEIGHT_3;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, -1), previousLevel).xyz * WEIGHT_2;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, -2), previousLevel).xyz * WEIGHT_1;
        color += texelFetch(ColorBuffer, previousTexcoord + ivec2(0, -3), previousLevel).xyz * WEIGHT_0;
    }
    else
    {
        color = texelFetch(ColorBuffer, thisLevelTexelCoord, previousLevel+1).xyz;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(3, 0), previousLevel+1).xyz * WEIGHT_0;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(2, 0), previousLevel+1).xyz * WEIGHT_1;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(1, 0), previousLevel+1).xyz * WEIGHT_2;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(0, 0), previousLevel+1).xyz * WEIGHT_3;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(-1, 0), previousLevel+1).xyz * WEIGHT_2;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(-2, 0), previousLevel+1).xyz * WEIGHT_1;
//        color += texelFetch(ColorBuffer, thisLevelTexelCoord + ivec2(-3, 0), previousLevel+1).xyz * WEIGHT_0;
    }
    FragColor = vec4(color, 1.0);
}