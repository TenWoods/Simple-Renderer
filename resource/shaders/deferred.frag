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
uniform float SCR_WIDTH;
uniform float SCR_HEIGHT;

const float step = 0.1;
const float minRayStep = 0.1;
const int maxStep = 100;
const int  numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

//#define Scale vec3(.8, .8, .8)
//#define K 19.19

//struct Ray
//{
//    vec3 origin;
//    vec3 direction;
//};
//
//struct Result
//{
//    bool isHit;
//    vec2 uv;
//    vec3 position;
//    int iterationCount;
//};

//float noise(vec2 seed)
//{
//    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
//}
//
//vec3 hash(vec3 a)
//{
//    a = fract(a * Scale);
//    a += dot(a, a.yxz + K);
//    return fract((a.xxy + a.yxx)*a.zyx);
//}
//
//vec3 fresnelSchlick(float cosTheta, vec3 F0)
//{
//    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
//}
//
//vec3 binarySearch(inout vec3 dir, inout vec3 viewPos, inout float dDepth)
//{
//    float depth;
//    vec4 projPos;
//    for (int i = 0; i < numBinarySearchSteps; i++)
//    {
//        projPos = projection * vec4(viewPos, 1.0);
//        projPos.xy /= projPos.w;
//        projPos.xy = projPos.xy * 0.5 + 0.5;
//
//        depth = texture(DepthBuffer, projPos.xy).x;
//
//        dDepth = viewPos.z - depth;
//        dir *= 0.5;
//        if (dDepth > 0.0)
//            viewPos += dir;
//        else
//            viewPos -= dir;
//    }
//    projPos = projection * vec4(viewPos, 1.0);
//    projPos.xy /= projPos.w;
//    projPos.xy = projPos.xy * 0.5 + 0.5;
//    return vec3(projPos.xy, depth);
//}
//
//vec4 ray_marching(vec3 dir, inout vec3 viewPos, out float dDepth)
//{
//    dir *= step;
//    float depth;
//    int steps;
//    vec4 projPos;
//    for (int i = 0; i < maxStep; i++)
//    {
//        viewPos += dir;
//        projPos = projection * vec4(viewPos, 1.0);
//        projPos.xy /= projPos.w;
//        projPos.xy = projPos.xy * 0.5 + 0.5;
//        depth = texture(DepthBuffer, projPos.xy).x;
//        if (depth > 1000.0)
//            continue;
//        depth = viewPos.z - depth;
//        if((dir.z - dDepth) < 1.2)
//        {
//            if(dDepth <= 0.0)
//            {
//                vec4 Result;
//                Result = vec4(binarySearch(dir, viewPos, dDepth), 1.0);
//                return Result;
//            }
//        }
//    }
//    return vec4(projPos.xy, depth, 0.0);
//}

bool rayIsOutofScreen(vec2 ray){
    return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}


vec3 TraceRay(vec3 rayPos, vec3 dir, int iterationCount){
    float sampleDepth;
    vec3 hitColor = vec3(0);
    bool hit = false;

    for(int i = 0; i < iterationCount; i++){
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

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
    float metallic = normal.w;
    vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
//    if (metallic < 0.01)
//        discard;
    vec4 color = texture(ColorBuffer, Texcoord);
    float roughness = color.a;
    vec2 screenPos = vec2(gl_FragCoord.x / SCR_WIDTH,  gl_FragCoord.y / SCR_HEIGHT);
    float depth = texture(DepthBuffer, Texcoord).x;
    depth = depth * 2.0 - 1.0;
    vec3 ndcPos = vec3(screenPos, depth);
    vec4 viewPos = inverseProj * vec4(ndcPos, 1.0);
    viewPos /= viewPos.w;
    vec3 reflectDir = normalize(reflect(viewPos.xyz, viewNormal));
    if(reflectDir.z > 0){
        FragColor = vec4(color.xyz, 1.0);
        return;
    }
    float maxRayDistance = 100.0f;
    vec3 rayEndPositionView = viewPos.xyz + reflectDir * maxRayDistance;
    vec4 rayEndPositionTexture = projection * vec4(rayEndPositionView, 1);
    rayEndPositionTexture /= rayEndPositionTexture.w;
    rayEndPositionTexture.xyz = (rayEndPositionTexture.xyz + vec3(1)) / 2.0f;
    vec3 rayDirectionTexture = rayEndPositionTexture.xyz - ndcPos;
    ivec2 screenSpaceStartPosition = ivec2(ndcPos.x * SCR_WIDTH, ndcPos.y * SCR_HEIGHT);
    ivec2 screenSpaceEndPosition = ivec2(rayEndPositionTexture.x * SCR_WIDTH, rayEndPositionTexture.y * SCR_HEIGHT);
    ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
    int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
    rayDirectionTexture /= max(screenSpaceMaxDistance, 0.001f);
    vec3 outColor = TraceRay(ndcPos, rayDirectionTexture, screenSpaceMaxDistance);
    color.xyz += outColor;
    FragColor = vec4(color.xyz, 1.0);
//    vec3 F0 = vec3(0.04);
//    F0 = mix(F0, color.xyz, metallic);
//    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos.xyz)), 0.0), F0);

//
//    float dDepth;
//    vec4 hitPointProj = ray_marching(reflectDir, viewPos.xyz, dDepth);
//
//    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - hitPointProj.xy));
//
//    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
//    float ReflectionMultiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflectDir.z;
//
//    vec3 SSR = texture(ColorBuffer, hitPointProj.xy).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;
////    vec3 SSR = texture(ColorBuffer, hitPointProj.xy).rgb;
//    color.xyz += SSR;
//    FragColor = vec4(color.xyz, 1.0);
}