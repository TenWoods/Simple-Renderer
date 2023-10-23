#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2D ShadowMap;
uniform vec3 cameraPos;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

const float step = 0.1;
const float minRayStep = 0.1;
const int  numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;
const float MAX_THICKNESS = 0.001;
const int MAX_STEP = 1000;
const float STEP_SIZE = 0.4;
const int MAX_SEARCH = 5;
const int width = 1920;
const int height = 1080;

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

float DistanceSquared(vec2 A, vec2 B)
{
    A -= B;
    return dot(A, A);
}

//vec3 RayMarching2D(vec3 origin, vec3 dierction, int maxStep)
//{
//    vec3 result = vec3(0.0);
//    //view space
//    vec3 V0 = (view * vec4(origin, 1.0)).xyz;
//    vec3 V1 = (view * vec4(origin + maxStep * dierction, 1.0)).xyz;
//    //clip space
//    vec4 H0 = projection * vec4(V0, 1.0);
//    vec4 H1 = projection * vec4(V1, 1.0);
//    float k0 = 1.0 / H0.w;
//    float k1 = 1.0 / H1.w;
//    vec3 Q0 = V0 * k0;
//    vec3 Q1 = V1 * k1;
//    //NDC space
//    vec2 P0 = H0.xy * k0;
//    vec2 P1 = H1.xy * k1;
//    //clip into [-1, 1]
//    if (P1.x > 1.0)
//        P1.x = 1.0;
//    if (P1.x < -1.0)
//        P1.x = -1.0;
//    if (P1.y > 1.0)
//        P1.y = 1.0;
//    if (P1.y < -1.0)
//        P1.y = -1.0;
//    //screen space
//    P0 = (P0 + 1) / 2 * vec2(width, height);
//    P1 = (P1 + 1) / 2 * vec2(width, height);
//    P1 += vec2(DistanceSquared(P0, P1) < 0.0001 ? 0.01 : 0.0);
//    vec2 delta = P1 - P0;
//
//    bool permute = false;
//
//    if (abs(delta.x) < abs(delta.y))
//    {
//        permute = true;
//        delta = delta.yx;
//        P0 = P0.yx;
//        P1 = P1.yx;
//    }
//
//    float stepDir = sign(delta.x);
//    float invDx = stepDir / delta.x;
//
//    vec2 dP = vec2(stepDir, delta.y * invDx);
//    vec3 dQ = (Q1 - Q0) * invDx;
//    float dk = (k1 - k0) * invDx;
//
//    P0 += dP;
//    Q0 += dQ;
//    k0 += dk;
//
//    int step = 0;
//    float k = k0;
//    float endX = P1.x * stepDir;
//    vec3 Q = Q0;
//    float prevMaxEstimate = V0.z;
//
//    vec2 uv;
//
//    for (vec2 P = P0; step < maxStep; step++, P += dP, Q.z += dQ.z, k += dk)
//    {
//        uv = permute ? P.yx : P;
//        float rayZmin = prevMaxEstimate;
//        float rayZmax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
//        prevMaxEstimate = rayZmax;
//        if (rayZmin > rayZmax)
//        {
//            float temp = rayZmin;
//            rayZmin = rayZmax;
//            rayZmax = temp;
//        }
//        if (uv.x > width || uv.y > height || uv.x < 0 || uv.y < 0)
//            break;
//        float bufferDepth = texelFetch(DepthBuffer, ivec2(uv)).r;
//        if (rayZmin <= bufferDepth && rayZmax >= bufferDepth)
//        {
//            result = texelFetch(ColorBuffer, ivec2(uv), 0).xyz;
//            //result = vec3(uv / vec2(width, height), 1.0);
//            break;
//        }
//    }
//    return result;
//}

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
            //return vec3(rayInScreen.xy, 1.0);
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
//    vec2 screenPos = Texcoord * 2.0 - 1.0;
//    float depth = textureLod(DepthBuffer, Texcoord, 5.0).x;
//    vec3 depthColor = vec3(depth, 0.0, 0.0);
//    vec3 ndcPos = vec3(screenPos, depth);
//    vec4 viewPos = inverseProj * vec4(ndcPos, 1.0);
//    viewPos /= viewPos.w;
    //vec4 worldPos = texture(WorldPosBuffer, Texcoord);
    //vec4 viewPos = view * worldPos;
    //vec3 viewDir = normalize(cameraPos - worldPos.xyz);
    //vec3 reflectDir = normalize(reflect(viewPos.xyz, viewNormal));
    //vec3 reflectDir = normalize(reflect(-viewDir.xyz, normal.xyz));
    //vec3 V1 = (view * vec4(worldPos.xyz + MAX_STEP * reflectDir, 1.0)).xyz;
    //vec3 V1 = viewPos.xyz + MAX_STEP * reflectDir;
    //vec3 rayColor = RayMarching(viewPos.xyz, reflectDir, MAX_STEP);
    //vec3 rayColor = RayMarching2D(worldPos.xyz, reflectDir, MAX_STEP);
    //color.xyz = mix(color.xyz, rayColor, 1 - roughness);
    //color.xyz += rayColor * 0.5;
    float lightDepth = texture(ShadowMap, Texcoord).r;
    //FragColor = vec4(color.xyz, 1.0);
    FragColor = vec4(normal.xyz, 1.0);
    //FragColor = vec4(lightDepth, lightDepth, lightDepth, 1.0);
}