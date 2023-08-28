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
uniform float time;

const float step = 0.1;
const float minRayStep = 0.1;
const int maxStep = 100;
const int  numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;
const float MAX_THICKNESS = 0.0017;
const float STEP_SIZE = 0.4;

bool rayIsOutofScreen(vec2 ray)
{
    return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}

vec3 TraceRay(vec3 rayPos, vec3 dir, int iterationCount)
{
    float sampleDepth;
    vec3 hitColor = vec3(0);
    bool hit = false;

    for(int i = 0; i < iterationCount; i++)
    {
        rayPos += dir;
        if(rayIsOutofScreen(rayPos.xy)){
            break;
        }

        sampleDepth = texture(DepthBuffer, rayPos.xy).r;
        float depthDif = rayPos.z - sampleDepth;
        if(depthDif >= 0 && depthDif < 0.00001){ //we have a hit
            hit = true;
            hitColor = texture(ColorBuffer, rayPos.xy).rgb;
            break;
        }
    }
    return hitColor;
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
        if (rayDepth < rayInScreen.z)
        {
            //TODO: binary search
            return texture(ColorBuffer, rayInScreen.xy).rgb;
        }
    }
    return vec3(0.0, 0.0, 0.0);
}

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
    float metallic = normal.w;
    vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
//    if (metallic < 0.01)
//        discard;
    vec4 color = texture(ColorBuffer, Texcoord);
    float roughness = color.a;
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    float depth = texture(DepthBuffer, Texcoord).x;
    vec3 ndcPos = vec3(screenPos, depth);
    vec4 viewPos = inverseProj * vec4(ndcPos, 1.0);
    viewPos /= viewPos.w;
    vec3 reflectDir = normalize(reflect(viewPos.xyz, viewNormal));
    const int MAX_STEP = 1000;
    vec3 rayColor = RayMarching(viewPos.xyz, reflectDir, MAX_STEP);
    color.xyz += rayColor*0.5;
    FragColor = vec4(color.xyz, 1.0);
}