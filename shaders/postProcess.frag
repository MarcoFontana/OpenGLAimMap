#version 410 core

out vec4 color;
  
in vec2 TexCoords;

uniform sampler2D sceneTex;

const int nsamples = 20;

subroutine vec4 blur();
subroutine uniform blur blurType;

subroutine(blur) 
vec4 radialBlur()
{
    //radial blur center point
    vec2 center = vec2(0.5, 0.5);
	float blurStart = 0.9;
    float blurWidth = 0.05;

    //sample in a larger area the further apart from the center the original point is
    vec2 uv = TexCoords - center;
    float a = blurWidth * (1.0 / float(nsamples - 1));

    vec4 color = vec4(0.0);

    //sample texture for color in nsamples points
    for(int i = 0; i < nsamples; i++)
    {
        float scale = blurStart + (float(i) * a);
        color += texture(sceneTex, uv * scale + center);
    }
    color /= float(nsamples);

    return color;
}

subroutine(blur) 
vec4 noBlur()
{
    return texture(sceneTex, TexCoords);
}

void main()
{

    color = blurType();

}