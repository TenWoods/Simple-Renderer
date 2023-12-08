#version 440 core

layout(location = 0) out vec4 out_1;

uniform sampler2D m_SSABSSBlur21;
uniform sampler2D m_SSABSSBlur22;
uniform sampler2D m_SSABSSPreCal2;
uniform float m_Edge;

uniform float m_Texw;
uniform float m_Texh;

varying vert {
    vec2 texcoord;
}Vert;

float normaldist(float sigma, float x){
    return exp(-x * x / (2 * sigma * sigma));
}

void main()
{
    vec2 blur;
    float kernelNormal_left, kernelNormal_right, kernel_sum;
    vec3 shadowdep_left, shadowdep_right;

    vec2 position = Vert.texcoord;
    vec4 origin = texture(m_SSABSSBlur22, position);
    vec4 originsub = texture(m_SSABSSBlur21, position);
    float w_p = origin.y * 20;
    float width_c = origin.z;
    w_p *= width_c;

    float sigma = w_p;
    int range = max(int(sigma * 5), 1);
    vec2 axisvec = (texture(m_SSABSSPreCal2, position).zw - 0.5) * 2; //minor dir
    kernel_sum = normaldist(sigma , 0);

    vec2 texcodl = axisvec / m_Texw, texcodr = -axisvec / m_Texh;
    //consider the gradient of the surface for bilateral filter
    float singrad;
    float ol = origin.w - texture(m_SSABSSBlur22, position + 2 * texcodl).w;
    float or = origin.w - texture(m_SSABSSBlur22, position + 2 * texcodr).w;
    if (abs(ol) < abs(or)) {
        singrad = ol / 2.0;
    }
    else {
        singrad = -or / 2.0;
    }

    shadowdep_left = texture(m_SSABSSBlur21, position).xzw;
    blur = shadowdep_left.xy * kernel_sum;

    for (float i = 1; i <= range; i++) {
        float kerneli = normaldist(sigma, i);
        shadowdep_left = texture(m_SSABSSBlur21, position + i * texcodl).xzw;
        shadowdep_right = texture(m_SSABSSBlur21, position + i * texcodr).xzw;
        kernelNormal_left = kerneli * clamp(normaldist(m_Edge, shadowdep_left.z - origin.w + singrad * i), 0.0, 1.0);
        kernelNormal_right = kerneli * clamp(normaldist(m_Edge, shadowdep_right.z - origin.w - singrad * i), 0.0, 1.0);

        kernel_sum += kernelNormal_left + kernelNormal_right;

        blur += shadowdep_left.xy * kernelNormal_left + shadowdep_right.xy * kernelNormal_right;
    }

    float result = (blur.x + originsub.y) / (origin.x + blur.y);
    out_1 = vec4(result, origin.yzw);
    // out_1= vec4(1.0, 0.0, 0.0, 1.0);
}
