#version 440 core
out float PCSS;

in vec2 Texcoord;

uniform sampler2D ShadowMap;
uniform sampler2D DepthMap;
uniform sampler2D NormalBuffer;
uniform vec3 uLightPos;
uniform mat4 lightVP;
uniform mat4 inverseProj;
uniform mat4 inverseView;

#define SHADOW_MAP_SIZE 1024.
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
        float cloestDepth = texture(ShadowMap, sampleCoord).x;
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
    float depth = texture(ShadowMap, shadowCoord.xy).x;
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

void main()
{
    vNormal = texture(NormalBuffer, Texcoord).xyz;
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    float depth = texture(DepthMap, Texcoord).x;
    depth *= 2.0;
    depth -= 1.0;
    vec4 clipPos = vec4(screenPos, depth, 1.0);
    vec4 viewPos = inverseProj * clipPos;
    viewPos /= viewPos.w;
    vec4 worldPos = (inverseView * viewPos);
    worldPos /= worldPos.w;
    vFragPos = worldPos.xyz;
    vec4 shadowCoord = lightVP * worldPos;
    shadowCoord /= shadowCoord.w;
    shadowCoord.xyz = shadowCoord.xyz * 0,5 + 0.5;
    float currentDepth = shadowCoord.z;
    float closeDepth = texture(ShadowMap, shadowCoord.xy).r;
    float shadow = currentDepth - 0.00005 > closeDepth ? 1.0 : 0.0;
    if (shadowCoord.x > 1.0 || shadowCoord.x < 0.0 || shadowCoord.y > 1.0 || shadowCoord.y < 0.0)
    {
        shadow = 0.0;
    }
    PCSS = shadow;
    // STEP 1: avgblocker depth
    float avgBlockerDepth = findBlocker(shadowCoord.xy, shadowCoord.z);
    if(avgBlockerDepth < -EPS)
    {
        PCSS = 1.0f;
    }

    // STEP 2: penumbra size
    float penumbraSize = max(shadowCoord.z - avgBlockerDepth, 0.0)/ avgBlockerDepth * LIGHT_SIZE_UV;

    // STEP 3: filtering
    float pcfBiasC = .08;
    PCSS = PCF(shadowCoord, pcfBiasC, penumbraSize);
}