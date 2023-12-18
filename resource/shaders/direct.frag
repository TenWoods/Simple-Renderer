#version 440 core

in vec2 Texcoord;

layout (location = 0) out vec4 DirectColor;
layout (location = 1) out vec4 ViewPosition;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform mat4 inverseProj;
uniform mat4 inverseView;
uniform mat4 view;

uniform sampler2D BaseColorMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;
uniform sampler2D PositionMap;
uniform sampler2D ShadowResult;

const float PI = 3.14159265359;
const float near = 0.1;
const float far  = 1000.0;

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

vec3 CalculateLighting(vec3 baseColor, vec3 F0, vec3 viewDir, vec3 lightDir, vec3 normal, float metallic, float roughness)
{
    vec3 Lo = vec3(0.0);
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    vec3 halfVec = normalize(viewDir + lightDir);
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

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec4 baseColor = texture(BaseColorMap, Texcoord);
    vec4 normal = texture(NormalMap, Texcoord);
    float depth = texture(DepthMap, Texcoord).x;
    depth *= 2.0;
    depth -= 1.0;
    float metallic = baseColor.w;
    float roughness = normal.w;
    vec2 screenPos = Texcoord * 2.0 - 1.0;
    vec4 clipPos = vec4(screenPos, depth, 1.0);
    vec4 viewPos = inverseProj * clipPos;
    viewPos /= viewPos.w;
    vec3 worldPos = (inverseView * viewPos).xyz;
    //vec3 worldPos = texture(PositionMap, Texcoord).xyz;
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor.xyz, metallic);
    //vec3 viewNormal = vec3(vec4(normal.xyz, 0.0) * inverseView);
    vec3 cameraDir = normalize(cameraPos - worldPos);
    //vec4 viewLightPos = view * vec4(lightPos, 1.0);
    vec3 lightDir = lightPos.xyz - worldPos.xyz;
    vec3 result = CalculateLighting(baseColor.xyz, F0, cameraDir, lightDir, normal.xyz, metallic, roughness);
    vec3 shadows = texture(ShadowResult, Texcoord).xyz;
    float shadow_f = shadows.x;
    result.xyz = result.xyz * (shadow_f * 0.7  + 0.3);
    DirectColor = vec4(result, 1.0);
    ViewPosition = vec4(viewPos.xyz, 1.0);
}
