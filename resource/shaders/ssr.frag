#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 projection;
uniform float time;

const float step = 0.1;
const float minRayStep = 0.1;
const int  numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;
const float MAX_THICKNESS = 0.001;
const int MAX_STEP = 10000;
const float STEP_SIZE = 0.4;
const int MAX_SEARCH = 5;

bool rayIsOutofScreen(vec2 ray)
{
    return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}

vec3 BinarySearch(vec3 origin, vec3 direction)
{
    vec3 rayPos = origin;
    float stepSize = STEP_SIZE;
    float sign = -1.0;
    for (int i = 0; i < MAX_SEARCH; i++)
    {
        stepSize *= 0.5;
        rayPos += sign * stepSize * direction;
        vec4 rayInScreen = projection * vec4(rayPos, 1.0);
        rayInScreen /= rayInScreen.w;
        rayInScreen.xy = rayInScreen.xy * 0.5 + 0.5;
        if (rayIsOutofScreen(rayInScreen.xy))
        {
            rayPos += -sign * stepSize * direction;
            return rayPos;
        }
        float rayDepth = texture(DepthBuffer, rayInScreen.xy).x;
        sign = rayDepth - rayInScreen.z > 0.0 ? 1.0 : -1.0;
    }
    return rayPos;
}

vec3 RayMarching(vec3 origin, vec3 direction, int maxStep)
{
    vec3 rayPos = origin;
    for (int i = 1; i < maxStep; i++)
    {
        rayPos += direction * STEP_SIZE;
        vec4 rayInScreen = projection * vec4(rayPos, 1.0);
        rayInScreen /= rayInScreen.w;
        rayInScreen.xy = rayInScreen.xy * 0.5 + 0.5;
        if (rayIsOutofScreen(rayInScreen.xy))
        {
            return vec3(0.0, 0.0, 0.0);
        }
        float rayDepth = texture(DepthBuffer, rayInScreen.xy).x;
        if (rayDepth < rayInScreen.z && rayInScreen.z - rayDepth <= MAX_THICKNESS)
        {
            rayPos = BinarySearch(rayPos, direction);
            return texture(ColorBuffer, rayInScreen.xy).rgb;
        }
    }
    vec4 originInScreen = projection * vec4(origin, 1.0);
    originInScreen /= originInScreen.w;
    originInScreen.xy = originInScreen.xy * 0.5 + 0.5;
    return texture(ColorBuffer, originInScreen.xy).rgb;;
}

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
    float metallic = normal.w;
    vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
    vec4 color = texture(ColorBuffer, Texcoord);
//    if (metallic < 0.5)
//    {
//        FragColor = vec4(color.xyz, 1.0);
//        return;
//    }
    float roughness = color.a;
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    float depth = textureLod(DepthBuffer, Texcoord, 2.0).x;
    vec3 depthColor = vec3(depth, 0.0, 0.0);
    vec3 ndcPos = vec3(screenPos, depth);
    vec4 viewPos = inverseProj * vec4(ndcPos, 1.0);
    viewPos /= viewPos.w;
    vec3 reflectDir = normalize(reflect(viewPos.xyz, viewNormal));
    //vec3 rayColor = RayMarching(viewPos.xyz, reflectDir, MAX_STEP);

    //color.xyz = mix(color.xyz, rayColor, roughness);
    //FragColor = vec4(color.xyz, 1.0);
    FragColor = vec4(depthColor, 1.0);
}