#version 440 core

layout (location = 0) out vec4 ColorBuffer;
layout (location = 1) out vec4 NormalBuffer;
layout (location = 2) out float DepthBuffer;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec2 Texcoord;
} fs_in;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

uniform sampler2D BaseColorMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicRoughnessMap;
uniform sampler2D AOMap;

const float PI = 3.14159265359;

float near = 0.1;
float far  = 1000.0;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(NormalMap, fs_in.Texcoord).xyz * 2.0 - 1.0;
    //    vec3 Q1 = dFdx(fs_in.FragPos);
    //    vec3 Q2 = dFdy(fs_in.FragPos);
    //    vec2 st1 = dFdx(fs_in.Texcoord);
    //    vec2 st2 = dFdy(fs_in.Texcoord);

    vec3 N = normalize(fs_in.Normal);
    vec3 T = normalize(fs_in.Tangent);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculate_point(vec3 baseColor, vec3 F0, vec3 viewDir, vec3 normal, float metallic, float roughness)
{
    vec3 Lo = vec3(0.0);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 halfVec = normalize(viewDir + lightDir);
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    float D = DistributionGGX(normal, halfVec, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(max(dot(halfVec, viewDir), 0.0), F0);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, lightDir), 0.0);
    Lo += (kD * baseColor / PI + specular) * radiance * NdotL;
    return Lo;
}

void main()
{
    vec3 baseColor = texture(BaseColorMap, fs_in.Texcoord).xyz;
    vec3 mrTex = texture(MetallicRoughnessMap, fs_in.Texcoord).xyz;
    float metallic = mrTex.x;
    float roughness = mrTex.y;
    vec3 normal = getNormalFromMap();
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);
    vec3 Lo = vec3(0.0);

    float depth = gl_FragCoord.z;

    Lo += calculate_point(baseColor, F0, viewDir, normal, metallic, roughness);

    vec3 ambient = vec3(0.03) * baseColor;
    vec3 color = ambient + Lo;
    color = pow(color, vec3(1.0/2.2));

    ColorBuffer = vec4(color, metallic);
    NormalBuffer = vec4(normal, roughness);
    DepthBuffer = depth;
}