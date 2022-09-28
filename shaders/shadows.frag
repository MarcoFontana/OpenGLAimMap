#version 410 core

const float PI = 3.14159265359;

out vec4 color;

in vec3 lightDir;
in vec3 vNormal;
in vec3 vViewPosition;
in vec4 posLightPOV;

uniform sampler2D shadowMap;

float Shadows()
{
    //perspective divide
    vec3 projCoords = posLightPOV.xyz / posLightPOV.w;
    //[-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    //shadow bias tuned to avoid shadow acne and shadow "peter panning"
    float bias = 0.001;

    //PCF to smooth shadow edges
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    //sample the depth map in the 3x3 neighbourhood of the current fragment
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            //fragment in shadow if his depth + bias is bigger then the shadow map depth in the same position
            float shadowMapDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > shadowMapDepth  ? 1.0 : 0.0;
        }
    }
    //divide by number of samples used
    shadow /= 9.0;

    //if point is behind the light fustrum far plane then it's not considered in shadow
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

// Schlick-GGX method for geometry obstruction
float G1(float angle, float alpha)
{
    float r = (alpha + 1.0);
    float k = (r*r) / 8.0;

    float num   = angle;
    float denom = angle * (1.0 - k) + k;

    return num / denom;
}

//material values used for the color
struct MaterialColor {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform MaterialColor materialColor;

struct MaterialLight {  
    float ambient;
    float diffuse;
    float specular;
};

uniform float modelAlpha;

uniform MaterialLight materialLight;

float F0 = 0.9f;
float alpha = materialColor.shininess;

vec3 GGX()
{

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightDir.xyz);

    //cosine angle between direction of light and normal
    float NdotL = max(dot(N, L), 0.0);

    //diffusive component: Lambert model
    vec3 lambert = (materialLight.diffuse * materialColor.diffuse) / PI;

    vec3 specular = vec3(0.0);
    float shadow = 0.0;

    //angle must be positive to calculate the specular component
    if(NdotL > 0.0)
    {

        vec3 V = normalize( vViewPosition );
        vec3 H = normalize(L + V);

        //needed parameters
        float NdotH = max(dot(N, H), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float VdotH = max(dot(V, H), 0.0);
        float alphaSquared = alpha * alpha;
        float NdotHSquared = NdotH * NdotH;

        //Geometric attenuation: Smith’s method
        float G2 = G1( NdotV, alpha ) * G1( NdotL, alpha );

        //Microfacets distribution
        float D = alphaSquared;
        float denom = ( NdotHSquared * (alphaSquared - 1.0) + 1.0 );
        D /= PI*denom*denom;

        //Fresnel reflectance: Schlik’s approximation
        vec3 F = vec3(pow(1.0 - VdotH, 5.0));
        F *= (1.0 - F0);
        F += F0;

        //FDG equation
        specular = (F * G2 * D) / (4.0 * NdotV * NdotL);
        shadow = Shadows();
    }

    return  materialLight.ambient * materialColor.ambient + 
            ( 
            (1.0 - shadow) * 
            (lambert + specular) * 
            NdotL
            );
}

void main()
{
    color = vec4(GGX(), modelAlpha);
}
