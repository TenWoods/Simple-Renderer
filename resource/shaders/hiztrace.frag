#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2D ShadowMap;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 view;
uniform mat4 projection;

const int MAXLENGTH = 10000;
const int width = 1024;
const int height = 1024;
const int MAX_STEP = 10000;
const float MAX_THICKNESS = 0.0001;
const int MAX_LEVEL = 4;
//
//vec3 Hiztrace(vec3 origin, vec3 direction, float maxDistance)
//{
//    vec2 crossStep = vec2(direction.x >= 0 ? 1 : -1, direction.y >=0 ? 1 : -1);
//    vec2 crossOffset = crossStep / vec2(width, height) / 128;
//    crossStep = clamp(crossStep, 0.0, 1.0);
//
//    vec3 rayPos = origin;
//    float minZ = rayPos.z;
//    float maxZ = rayPos.z + direction.z * maxDistance;
//    float deltaZ = maxZ - minZ;
//
//}

vec3 BinarySearch(vec3 origin, vec3 direction)
{
    float sign = -1.0;
    vec3 rayPos = origin;
    for (int i = 0; i < 6; i++)
    {
        direction *= 0.5;
        rayPos += sign * direction;
        float rayDepth = texture(DepthBuffer, rayPos.xy).x;
        sign = rayDepth - rayPos.z > 0.0 ? 1.0 : -1.0;
    }
    return rayPos;
}

vec3 LinearTrace(vec3 origin, vec3 direction, float maxDistance)
{
    vec3 end = origin + direction * maxDistance;
    vec3 dp = end - origin;
    ivec2 originScreen = ivec2(origin.xy * ivec2(width, height));
    ivec2 endScreen = ivec2(end.xy * ivec2(width, height));
    ivec2 dpScreen = endScreen - originScreen;
    int max_dist = max(abs(dpScreen.x), abs(dpScreen.y));
    dp /= max_dist;

    vec3 rayPos = origin + dp;
    vec3 dir = dp;
    vec3 result = vec3(0.0);
    for (int i = 0; i < max_dist && i < MAX_STEP; i++)
    {
        rayPos += i * dir;
        float rayDepth = texture(DepthBuffer, rayPos.xy).r;
        if (rayDepth <= rayPos.z && rayPos.z - rayDepth <= MAX_THICKNESS)
        {
            vec3 rayResult = BinarySearch(rayPos, dir);
            result = texture(ColorBuffer, rayResult.xy).xyz;
            break;
        }
    }
    return result;
}

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
    //vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
    vec3 viewNormal = normal.xyz;
    vec4 color = texture(ColorBuffer, Texcoord);

    //calculate position
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    float depth = textureLod(DepthBuffer, Texcoord, 0.0).x;
    vec4 beginClipPos = vec4(screenPos, depth, 1.0);
    vec4 beginViewPos = inverseProj * beginClipPos;
    beginViewPos /= beginViewPos.w;
    vec4 reflectDir = vec4(normalize(reflect(beginViewPos.xyz, viewNormal)), 0.0);
    vec4 endViewPos = beginViewPos + reflectDir * MAXLENGTH;
    endViewPos /= (endViewPos.z < 0 ? endViewPos.z : 1.0);  //?
    vec4 endClipPos = projection * endViewPos;
    endClipPos /= endClipPos.w;
    vec3 reflectDirClip = normalize(endClipPos.xyz - beginClipPos.xyz);

    //transform to texture space
    beginClipPos.xy *= vec2(0.5, 0.5);
    beginClipPos.xy += vec2(0.5, 0.5);
    reflectDirClip.xy *= vec2(0.5, 0.5);

    //max iteratuib distance
    float maxDistance = reflectDirClip.x >= 0 ? (1 - beginClipPos.x)/reflectDirClip.x : -beginClipPos.x/reflectDirClip.x;
    maxDistance = min(maxDistance, reflectDirClip.y >= 0 ? (1 - beginClipPos.y)/reflectDirClip.y : -beginClipPos.y/reflectDirClip.y);
    maxDistance = min(maxDistance, reflectDirClip.z >= 0 ? (1 - beginClipPos.z)/reflectDirClip.z : -beginClipPos.z/reflectDirClip.z);

    vec3 rayColor = LinearTrace(beginClipPos.xyz, reflectDirClip, maxDistance);

    color.xyz += rayColor.xyz;

    FragColor = color;
}