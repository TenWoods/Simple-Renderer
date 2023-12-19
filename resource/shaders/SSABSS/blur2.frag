#version 440 core

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;

uniform sampler2D m_SSABSSBlur11;
uniform sampler2D m_SSABSSBlur12;
uniform sampler2D m_SSABSSPreCal2;
uniform float m_Edge;

uniform float m_Texw;
uniform float m_Texh;

in vec2 texcoord;

float normaldist(float sigma, float x){
    return exp(-x * x / (2 * sigma * sigma));
}

void main()
{
    vec3 blur;
    float kernelNormal_left, kernelNormal_right, kernel_sum;
    vec4 shadowdep_left, shadowdep_right;

    vec2 position = texcoord;
    vec4 origin = texture(m_SSABSSBlur11, position);
    float w_p = origin.y * 20;

    float sigma = w_p;
    int range = max(int(sigma * 5), 1);
    vec2 axisvec = (texture(m_SSABSSPreCal2, position).xy - 0.5) * 2; //minor dir
    kernel_sum = normaldist(sigma , 0);

    vec2 texcodl = axisvec / m_Texw, texcodr = -axisvec / m_Texh;
    //consider the gradient of the surface for bilateral filter
    //float singrad = sqrt(1 - pow(origin.z, 2));
    float singrad;
    float ol = origin.w - texture(m_SSABSSBlur11, position + 2 * texcodl).w;
    float or = origin.w - texture(m_SSABSSBlur11, position + 2 * texcodr).w;
    if (abs(ol) < abs(or)) {
        singrad = ol / 2.0;
    }
    else {
        singrad = -or / 2.0;
    }

    shadowdep_left = texture(m_SSABSSBlur12, position).xyzw;
    blur = shadowdep_left.xyz * kernel_sum;

    for (int i = 1; i <= range; i++) {
        float kerneli = normaldist(sigma, i);
        shadowdep_left = texture(m_SSABSSBlur12, position + i * texcodl).xyzw;
        shadowdep_right = texture(m_SSABSSBlur12, position + i * texcodr).xyzw;
        kernelNormal_left = kerneli * clamp(normaldist(m_Edge, shadowdep_left.w - origin.w + singrad * i), 0.0, 1.0);
        kernelNormal_right = kerneli * clamp(normaldist(m_Edge, shadowdep_right.w - origin.w - singrad * i), 0.0, 1.0);

        kernel_sum += kernelNormal_left + kernelNormal_right;

        blur += shadowdep_left.xyz * kernelNormal_left + shadowdep_right.xyz * kernelNormal_right;
    }

    float blurbeforenormalize = blur.y;

    out_1 = vec4(blur.xy, kernel_sum, origin.w);
    out_2 = vec4(blur.z, origin.yzw);
    // out_1= vec4(1.0, 0.0, 0.0, 1.0);
}
