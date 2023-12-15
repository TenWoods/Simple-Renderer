#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2D PositionBuffer;
uniform sampler2D VisibilityBuffer;
uniform sampler2D ShadowResult;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 uLightPos;
uniform mat4 lightVP;

const int MAXLENGTH = 1000;
const int width = 1024;
const int height = 1024;
const int MAX_STEP = 10000;
const float MAX_THICKNESS = 0.0001;
const int MAX_MIPMAP_LEVEL = 4;
const float MIN_SPECULAR_POWER = 0.1;
const float MAX_SPECULAR_POWER = 1024.0;

#define FADE_BORDER_START		0.8
#define FADE_BORDER_END			1.0
#define FADE_MIRROR_FACTOR		10.0
#define saturate(x) clamp(x, 0.0, 1.0)

#define SHADOW_MAP_SIZE 2048.
#define NUM_SAMPLES 50
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define EPS 1e-3
#define FRUSTUM_SIZE 400.
#define LIGHT_WORLD_SIZE 5.
#define LIGHT_SIZE_UV LIGHT_WORLD_SIZE / FRUSTUM_SIZE
#define PI 3.141592653589793
#define PI2 6.283185307179586

vec3 vNormal;
vec3 vFragPos;
vec2 poissonDisk[NUM_SAMPLES];

highp float rand_2to1(vec2 uv )
{
    // 0 - 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
    return fract(sin(sn) * c);
}

void poissonDiskSamples( const in vec2 randomSeed )
{

    float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
    float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

    float angle = rand_2to1( randomSeed ) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

float findBlocker(vec2 uv, float zReceiver)
{
    const int radius = 40;
    const vec2 texelSize = vec2(1.0/2048.0, 1.0/2048.0);
    float cnt = 0.0, blockerDepth = 0.0;
    int flag = 0;
    for(int ns = 0;ns < BLOCKER_SEARCH_NUM_SAMPLES;++ns)
    {
        vec2 sampleCoord = (vec2(radius) * poissonDisk[ns]) * texelSize + uv;
        float cloestDepth = texture(ShadowResult, sampleCoord).x;
        if(zReceiver - 0.002 > cloestDepth)
        {
            blockerDepth += cloestDepth;
            cnt += 1.0;
            flag = 1;
        }
    }
    if(flag == 1)
    {
        return blockerDepth / cnt;
    }
    return 1.0;
}

float getShadowBias(float c, float filterRadiusUV)
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float fragSize = (1. + ceil(filterRadiusUV)) * (FRUSTUM_SIZE / SHADOW_MAP_SIZE / 2.);
    return max(fragSize, fragSize * (1.0 - dot(normal, lightDir))) * c;
}

float useShadowMap(vec4 shadowCoord, float biasC, float filterRadiusUV)
{
    float depth = texture(ShadowResult, shadowCoord.xy).x;
    float cur_depth = shadowCoord.z;
    float bias = getShadowBias(biasC, filterRadiusUV);
    if(cur_depth - bias >= depth + EPS)
    {
        return 0.;
    }
    else
    {
        return 1.0;
    }
}

float PCF(vec4 coords, float biasC, float filterRadiusUV)
{
    poissonDiskSamples(coords.xy);
    float visibility = 0.0;
    for(int i = 0; i < NUM_SAMPLES; i++)
    {
        vec2 offset = poissonDisk[i] * filterRadiusUV;
        float shadowDepth = useShadowMap(coords + vec4(offset, 0., 0.), biasC, filterRadiusUV);
        if(coords.z > shadowDepth + EPS)
        {
            visibility++;
        }
    }
    return 1.0 - visibility / float(NUM_SAMPLES);
}

vec3 IntersectionDepthPlane(vec3 origin, vec3 direction, float t)
{
    return origin + direction * t;
}

vec2 GetCellCount(vec2 size, float level)
{
    return floor(size / (level > 0.0 ? exp2(level) : 1.0));
}

vec2 GetCell(vec2 pos, vec2 cell_count)
{
    return vec2(floor(pos * cell_count));
}

vec3 IntersectCellBoundary(vec3 o, vec3 d, vec2 cell, vec2 cell_count, vec2 crossStep, vec2 crossOffset)
{
    vec3 intersection = vec3(0.0f);
    vec2 index = cell + crossStep;
    vec2 boundary = index / cell_count;
    boundary += crossOffset;

    vec2 delta = boundary - o.xy;
    delta /= d.xy;
    float t = min(delta.x, delta.y);

    intersection = IntersectionDepthPlane(o, d, t);
    return intersection;
}

bool CrossedCellBoundary(vec2 cellIdxA, vec2 cellIdxB)
{
    return cellIdxA.x != cellIdxB.x || cellIdxA.y != cellIdxB.y;
}

vec2 GetMipmapResolution(vec2 screen_size, int mipmap_level)
{
    return screen_size * pow(0.5, mipmap_level);
}

float GetMinDepthPlane(vec2 pos, float level)
{
    return textureLod(DepthBuffer, pos, level).x * 2.0 - 1.0;
}

float RoughnessToSpecularPower(float roughness)
{
    return mix(MAX_SPECULAR_POWER, MIN_SPECULAR_POWER, pow(roughness, 0.1));
}

float RoughnessToConeAngle(float roughness)
{
    return mix(0.0, 1.0, roughness);
}

float SpecularPowerToConeAngle(float specularPower)
{
    if (specularPower < MAX_SPECULAR_POWER)
    {
        /* Based on phong reflection model */
        const float xi = 0.244;
        float exponent = 1.0 / (specularPower + 1.0);
        return acos(pow(xi, exponent));
    }
    return 0.0;
}

float IsoscelesTriangleOpposite(float adjacentLength, float coneTheta)
{
    /*
    Simple trig and algebra - soh, cah, toa - tan(theta) = opp/adj,
    opp = tan(theta) * adj, then multiply by 2 for isosceles triangle base
    */
    return 2.0 * tan(coneTheta) * adjacentLength;
}

float IsoscelesTriangleInRadius(float a, float h)
{
    float h4 = h*4.0;
    return (a * (sqrt(a*a + h4*h) - a)) / max(h4, 0.00001);
}

float IsoscelesTriangleNextAdjacent(float adjacentLength, float incircleRadius)
{
    /*
    Subtract the diameter of the incircle
    to get the adjacent side of the next level on the cone
    */
    return adjacentLength - (incircleRadius * 2.0);
}

vec4 ConeSampleWeightedColor(vec2 samplePos, float mipLevel)
{
    /* Sample color buffer with pre-integrated visibility */
    vec3 color = textureLod(ColorBuffer, samplePos, mipLevel).rgb;
    float visibility = textureLod(VisibilityBuffer, samplePos, mipLevel).r;
    return vec4(color * visibility, visibility);
}

bool Hiztrace(vec3 origin, vec3 direction, float maxDistance, out vec3 intersection)
{
    vec2 crossStep = vec2(direction.x >= 0 ? 1 : -1, direction.y >=0 ? 1 : -1);
    vec2 crossOffset = crossStep / vec2(width, height) / 128;
    crossStep = clamp(crossStep, 0.0, 1.0);

    vec3 rayPos = origin;
    float minZ = rayPos.z;
    float maxZ = rayPos.z + direction.z * maxDistance;
    float deltaZ = maxZ - minZ;

    vec3 o = rayPos;
    vec3 d = direction * maxDistance;

    int startLevel = 2;
    int stopLevel = 0;

    vec2 startCellCount = vec2(GetCellCount(vec2(width, height), startLevel));
    vec2 rayCell = GetCell(rayPos.xy, startCellCount);
    rayPos = IntersectCellBoundary(rayPos, direction, rayCell, startCellCount, crossStep, crossOffset * 128.0 * 2.0);

    int level = startLevel;
    int iter = 0;
    bool isBackward = direction.z < 0;
    float dir = isBackward ? -1.0 : 1.0;
    while (level >= stopLevel && rayPos.z * dir <= maxZ * dir && ++iter < MAX_STEP)
    {
        vec2 cellCount = vec2(GetCellCount(vec2(width, height), level));
        vec2 oldCellIdx = GetCell(rayPos.xy, cellCount);

        float cell_minZ = GetMinDepthPlane((oldCellIdx+0.5f)/cellCount, level);
        vec3 tmpRay = ((cell_minZ > rayPos.z) && !isBackward) ? IntersectionDepthPlane(o, d, (cell_minZ - minZ) / deltaZ) : rayPos;
        vec2 newCellIdx = GetCell(tmpRay.xy, cellCount);
        float thickness = level == 0 ? (rayPos.z - cell_minZ) : 0.0;
        bool crossed = (isBackward && (cell_minZ > rayPos.z)) || (thickness > MAX_THICKNESS) || CrossedCellBoundary(oldCellIdx, newCellIdx);
        rayPos = crossed ? IntersectCellBoundary(o, d, oldCellIdx, cellCount, crossStep, crossOffset) : tmpRay;
        level = crossed ? min(MAX_MIPMAP_LEVEL, level + 1) : level - 1;
    }
    bool intersected = (level < stopLevel);
    intersection = intersected ? rayPos : vec3(0.0);
    return intersected;
}

vec3 HizConeTrace(vec2 intersectPos, float roughness)
{
    float specularPower = RoughnessToSpecularPower(roughness);
    float coneTheta = SpecularPowerToConeAngle(specularPower);
    vec2 deltaPos = intersectPos - Texcoord.xy;

    float adjacentLength = length(deltaPos);
    vec2 adjacentUnit = normalize(deltaPos);

    vec4 reflectionColor = vec4(0.0);

    /* Append offset to adjacent length, so we have our first inner circle */
    adjacentLength += IsoscelesTriangleOpposite(adjacentLength, coneTheta);
    for (int i = 0; i < 7; i++)
    {
        /* Intersection length is the adjacent side, get the opposite side using trigonometry */
        float oppositeLength = IsoscelesTriangleOpposite(adjacentLength, coneTheta);

        /* Calculate in-radius of the isosceles triangle now */
        float incircleSize = IsoscelesTriangleInRadius(oppositeLength, adjacentLength);

        /* Get the sample position in screen space */
        vec2 samplePos = Texcoord + adjacentUnit * (adjacentLength - incircleSize);

        /*
		Convert the in-radius into screen space and then check what power N
		we have to raise 2 to reach it. That power N becomes our mip level to sample from.
		*/
        float mipLevel = log2(incircleSize * max(width, height));

        /*
		Read color and accumulate it using trilinear filtering (blending in xy and mip direction) and weight it.
		Uses pre-convolved image, pre-integrated visibility buffer and Hi-Z buffer. It checks if cone sphere is below,
		in between, or above the Hi-Z minimum and maximum and weights it together with transparency.
		Visibility is accumulated in the alpha channel.
		*/
        reflectionColor += ConeSampleWeightedColor(samplePos, mipLevel);

        if (reflectionColor.a > 1.0)
            break;

        /* Calculate next smaller triangle that approximates the cone in screen space */
        adjacentLength = IsoscelesTriangleNextAdjacent(adjacentLength, incircleSize);
    }
//    /* Normalize reflection color (if visiblity is greater than 1.0 */
//    reflectionColor /= reflectionColor.a;
    return reflectionColor.xyz;
}


//vec3 LinearTrace(vec3 origin, vec3 direction, float maxDistance)
//{
//    vec3 end = origin + direction * maxDistance;
//    vec3 dp = end - origin;
//    ivec2 originScreen = ivec2(origin.xy * ivec2(width, height));
//    ivec2 endScreen = ivec2(end.xy * ivec2(width, height));
//    ivec2 dpScreen = endScreen - originScreen;
//    int max_dist = max(abs(dpScreen.x), abs(dpScreen.y));
//    dp /= max_dist;
//
//    vec3 rayPos = origin + dp;
//    vec3 dir = dp;
//    vec3 result = vec3(0.0);
//    int hit_index = -1;
//    for (int i = 0; i < max_dist && i < MAX_STEP; i+=4)
//    {
////        rayPos += i * dir;
////        float rayDepth = texture(DepthBuffer, rayPos.xy).r;
////        rayDepth *= 2.0;
////        rayDepth -= 1.0;
////        if (rayDepth <= rayPos.z && rayPos.z - rayDepth <= MAX_THICKNESS)
////        {
////            //vec3 rayResult = BinarySearch(rayPos, dir);
////            result = texture(ColorBuffer, rayPos.xy).xyz;
////            break;
////        }
//        vec3 rayPos0 = rayPos;
//        vec3 rayPos1 = rayPos + dir;
//        vec3 rayPos2 = rayPos + 2.0 * dir;
//        vec3 rayPos3 = rayPos + 3.0 * dir;
//
//        float depth0 = texture(DepthBuffer, rayPos0.xy).x * 2.0 - 1.0;
//        float depth1 = texture(DepthBuffer, rayPos1.xy).x * 2.0 - 1.0;
//        float depth2 = texture(DepthBuffer, rayPos2.xy).x * 2.0 - 1.0;
//        float depth3 = texture(DepthBuffer, rayPos3.xy).x * 2.0 - 1.0;
//
//        hit_index = (rayPos0.z >= depth0 && rayPos0.z - depth0 <= MAX_THICKNESS) ? i+0 : hit_index;
//        hit_index = (rayPos1.z >= depth1 && rayPos1.z - depth1 <= MAX_THICKNESS) ? i+1 : hit_index;
//        hit_index = (rayPos2.z >= depth2 && rayPos2.z - depth2 <= MAX_THICKNESS) ? i+2 : hit_index;
//        hit_index = (rayPos3.z >= depth3 && rayPos3.z - depth3 <= MAX_THICKNESS) ? i+3 : hit_index;
//
//        if (hit_index != -1)
//            break;
//        rayPos = rayPos3 + dir;
//    }
//    if (hit_index != -1)
//    {
//        vec3 intersection = origin + hit_index * dir;
//        result = texture(ColorBuffer, intersection.xy).xyz;
//    }
//    return result;
//}

float PCSS(vec4 shadowCoord)
{
    // STEP 1: avgblocker depth
    float avgBlockerDepth = findBlocker(shadowCoord.xy, shadowCoord.z);
    if(avgBlockerDepth < -EPS)
    {
        return 1.0f;
    }

    // STEP 2: penumbra size
    float penumbraSize = max(shadowCoord.z - avgBlockerDepth, 0.0)/ avgBlockerDepth * LIGHT_SIZE_UV;

    // STEP 3: filtering
    float pcfBiasC = .08;
    return PCF(shadowCoord, pcfBiasC, penumbraSize);
}

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
    vNormal = normal.xyz;
    float roughness = normal.w;
//    normal.xyz *= 2.0;
//    normal.xyz -= 1.0;
    vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
    //vec3 viewNormal = normal.xyz;
    vec4 color = texture(ColorBuffer, Texcoord);
    color.w = 1.0;
    //vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    //calculate position
    //vec3 worldPos = texture(PositionBuffer, Texcoord).xyz;
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    float depth = texture(DepthBuffer, Texcoord).x;
    depth *= 2.0;
    depth -= 1.0;
    vec4 clipPos = vec4(screenPos, depth, 1.0);
    vec4 viewPos = inverseProj * clipPos;
    viewPos /= viewPos.w;
    vec3 worldPos = (inverseView * viewPos).xyz;
    vFragPos = worldPos.xyz;
    vec4 beginViewPos = view * vec4(worldPos, 1.0);
    vec4 beginClipPos = projection * beginViewPos;
    beginClipPos /= beginClipPos.w;
    vec4 reflectDir = vec4(normalize(reflect(beginViewPos.xyz, viewNormal)), 0.0);
    vec4 endViewPos = beginViewPos + reflectDir * MAXLENGTH;
    //endViewPos /= (endViewPos.z < 0 ? endViewPos.z : 1.0);  //?
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

    vec3 resultPos = vec3(0.0);
    if (Hiztrace(beginClipPos.xyz, reflectDirClip, maxDistance, resultPos))
    {
        //vec3 rayColor = textureLod(ColorBuffer, resultPos.xy, 0.0).xyz;
        vec3 rayColor = HizConeTrace(resultPos.xy, roughness);
//        float boundary = distance(resultPos.xy, vec2(0.5)) * 2.0;
//        float fadeOnBorder = 1.0 - saturate((boundary - FADE_BORDER_START) / (FADE_BORDER_END - FADE_BORDER_START));
//
//        /* Compute fading on mirror (rays towards the camera) */
//        float fadeOnMirror = saturate(reflectDir.z * FADE_MIRROR_FACTOR);
//        rayColor.rgb *= (fadeOnBorder * fadeOnMirror);
        color.xyz += 0.5 * rayColor.xyz;
    }
//    vec4 shadowCoord = lightVP * vec4(worldPos, 1.0);
//    shadowCoord /= shadowCoord.w;
//    shadowCoord.xyz = shadowCoord.xyz * 0.5 + 0.5;
//    float shadow = PCSS(shadowCoord);
//    vec3 shadows = texture(ShadowResult, Texcoord).xyz;
//    float shadow_f = shadows.y;
//    color.xyz = color.xyz * (shadow_f * 0.7 * color.w + 0.3 * color.w * color.w);
    FragColor = color;
}