#version 440 core
out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2D PositionBuffer;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

const int MAXLENGTH = 1000;
const int width = 1024;
const int height = 1024;
const int MAX_STEP = 10000;
const float MAX_THICKNESS = 0.0001;
const int MAX_MIPMAP_LEVEL = 4;

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
    int hit_index = -1;
    for (int i = 0; i < max_dist && i < MAX_STEP; i+=4)
    {
//        rayPos += i * dir;
//        float rayDepth = texture(DepthBuffer, rayPos.xy).r;
//        rayDepth *= 2.0;
//        rayDepth -= 1.0;
//        if (rayDepth <= rayPos.z && rayPos.z - rayDepth <= MAX_THICKNESS)
//        {
//            //vec3 rayResult = BinarySearch(rayPos, dir);
//            result = texture(ColorBuffer, rayPos.xy).xyz;
//            break;
//        }
        vec3 rayPos0 = rayPos;
        vec3 rayPos1 = rayPos + dir;
        vec3 rayPos2 = rayPos + 2.0 * dir;
        vec3 rayPos3 = rayPos + 3.0 * dir;

        float depth0 = texture(DepthBuffer, rayPos0.xy).x * 2.0 - 1.0;
        float depth1 = texture(DepthBuffer, rayPos1.xy).x * 2.0 - 1.0;
        float depth2 = texture(DepthBuffer, rayPos2.xy).x * 2.0 - 1.0;
        float depth3 = texture(DepthBuffer, rayPos3.xy).x * 2.0 - 1.0;

        hit_index = (rayPos0.z >= depth0 && rayPos0.z - depth0 <= MAX_THICKNESS) ? i+0 : hit_index;
        hit_index = (rayPos1.z >= depth1 && rayPos1.z - depth1 <= MAX_THICKNESS) ? i+1 : hit_index;
        hit_index = (rayPos2.z >= depth2 && rayPos2.z - depth2 <= MAX_THICKNESS) ? i+2 : hit_index;
        hit_index = (rayPos3.z >= depth3 && rayPos3.z - depth3 <= MAX_THICKNESS) ? i+3 : hit_index;

        if (hit_index != -1)
            break;
        rayPos = rayPos3 + dir;
    }
    if (hit_index != -1)
    {
        vec3 intersection = origin + hit_index * dir;
        result = texture(ColorBuffer, intersection.xy).xyz;
    }
    return result;
}

void main()
{
    vec4 normal = texture(NormalBuffer, Texcoord);
//    normal.xyz *= 2.0;
//    normal.xyz -= 1.0;
    vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
    //vec3 viewNormal = normal.xyz;
    //vec4 color = vec4(texture(ColorBuffer, Texcoord).xyz, 1.0);
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    //calculate position
    vec3 worldPos = texture(PositionBuffer, Texcoord).xyz;
//    vec2 screenPos = Texcoord * 2.0 - 1.0;
//    float depth = texture(DepthBuffer, Texcoord).x;
//    depth *= 2.0;
//    depth -= 2.0;
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
    if (Hiztrace(beginClipPos.xyz, reflectDirClip, maxDistance,resultPos))
    {
        vec3 rayColor = texture(ColorBuffer, resultPos.xy).xyz;
        color.xyz = rayColor.xyz;
    }


    //vec3 rayColor = LinearTrace(beginClipPos.xyz, reflectDirClip, maxDistance);
    //color.xyz += rayColor.xyz * 0.5;
    FragColor = color;
}