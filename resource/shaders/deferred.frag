#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 projection;
uniform mat4 view;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Result
{
    bool isHit;
    vec2 uv;
    vec3 position;
    int iterationCount;
};

float noise(vec2 seed)
{
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}


void main()
{
    vec4 color = texture(ColorBuffer, Texcoord);
    float roughness = color.a;
    vec2 screenPos = 2.0 * Texcoord - 1.0;
    float depth = texture(DepthBuffer, Texcoord).x;
    depth = depth * 2.0 - 1.0;
    vec3 ndcPos = vec3(screenPos, depth);
    vec4 position = inverseProj * vec4(ndcPos, 1.0);
    position /= position.w;
    //FragColor = vec4(texture(ColorBuffer, Texcoord).xyz, 1.0);

    FragColor = vec4(position.xyz, 1.0);
}