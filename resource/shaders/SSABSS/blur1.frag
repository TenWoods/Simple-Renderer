#version 440 core

layout(location = 0) out vec4 out_1;
layout(location = 1) out vec4 out_2;

uniform sampler2D m_SSABSSPreCal1;
uniform sampler2D m_SSABSSPreCal2;
uniform float m_Edge;
uniform float m_Texw;
uniform float m_Texh;

in vec2 texcoord;

float normaldist(float sigma, float x)
{
    return exp(-x * x / (2 * sigma * sigma));
}

void main()
{
    vec3 blur;
    float kernelNormal_left, kernelNormal_right, kernel_sum;
    vec2 shadowdep_left, shadowdep_right;

    vec2 position = vec2(texcoord);
    vec4 origin = texture(m_SSABSSPreCal1, position);
    float w_p = origin.y * 20;
    float width_c = origin.z;
    w_p *= width_c; //minor weight

    //long->short axis < 1; short->long axis > 1

    float sigma = w_p;
    int range = max(int(sigma * 5), 1);
    vec2 axisvec = (texture(m_SSABSSPreCal2, position).zw - 0.5) * 2; //minor dir

    vec2 texcodl = axisvec / m_Texw, texcodr = -axisvec / m_Texh;
    //consider the gradient of the surface for bilateral filter
    float singrad;
    float ol = origin.w - texture(m_SSABSSPreCal1, position + 2 * texcodl).w;
    float or = origin.w - texture(m_SSABSSPreCal1, position + 2 * texcodr).w;
    if (abs(ol) < abs(or))
    {
        singrad = ol / 2.0;
    }
    else
    {
        singrad = -or / 2.0;
    }

    shadowdep_left = texture(m_SSABSSPreCal1, position).xw;
    kernel_sum = 1; // 1 is equivalent to normaldist(sigma , 0);

    blur.x = shadowdep_left.x * kernel_sum;
    for (float i = 1; i <= range; i++)
    {
        float kerneli = normaldist(sigma, i);
        shadowdep_left = texture(m_SSABSSPreCal1, position + i * texcodl).xw;
        shadowdep_right = texture(m_SSABSSPreCal1, position + i * texcodr).xw;
        kernelNormal_left = kerneli * clamp(normaldist(m_Edge, shadowdep_left.y - origin.w + singrad * i), 0.0, 1.0);
        kernelNormal_right = kerneli * clamp(normaldist(m_Edge, shadowdep_right.y - origin.w - singrad * i), 0.0, 1.0);

        kernel_sum += kernelNormal_left + kernelNormal_right;

        blur.x += shadowdep_left.x * kernelNormal_left + shadowdep_right.x * kernelNormal_right;
    }

    float blurbeforenormalize = blur.x;
    out_1 = vec4(blur.x, origin.yzw);
    out_2 = vec4(origin.x, blurbeforenormalize, kernel_sum, origin.w);
}