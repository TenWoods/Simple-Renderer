#version 440 core

layout(location = 0) out vec4 out_1;

uniform sampler2D m_SSABSSBlur31;
uniform sampler2D m_SSCM;

varying vert {
    vec2 texcoord;
}Vert;

void main()
{
    vec3 shadows = texture(m_SSABSSBlur31, Vert.texcoord).xyz;

    float shadow_f = shadows.x;
    vec4 color = texture(m_SSCM, Vert.texcoord);
    // out_1 = color;
    // out_1 = vec4(shadow_f, shadow_f, shadow_f, 1);
    out_1 = vec4(color.xyz * (shadow_f * 0.7 * color.w + 0.3 * color.w * color.w), 1);
}
