#version 330 core
out vec2 FragColor;
in vec2 TexCoords;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 normalVector, float roughness)
{
	float a = roughness * roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	vec3 halfwayVector;
	halfwayVector.x = cos(phi) * sinTheta;
	halfwayVector.y = sin(phi) * sinTheta;
	halfwayVector.z = cosTheta;
	
	vec3 up        = abs(normalVector.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, normalVector));
	vec3 bitangent = cross(normalVector, tangent);
	
	vec3 sampleVec = tangent * halfwayVector.x + bitangent * halfwayVector.y + normalVector * halfwayVector.z;
	return normalize(sampleVec);
}

float GeometrySchlickGGX(float normalViewVectorsCosAngle, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float numerator   = normalViewVectorsCosAngle;
    float denominator = normalViewVectorsCosAngle * (1.0 - k) + k;

    return numerator / denominator;
}

float GeometrySmith(vec3 normalVector, vec3 viewDirVector, vec3 lightDirVector, float roughness)
{
    float normalViewDirVectorsCosAngle  = max(dot(normalVector, viewDirVector), 0.0);
    float normalLightDirVectorsCosAngle = max(dot(normalVector, lightDirVector), 0.0);
    float normalViewDirsGGX             = GeometrySchlickGGX(normalViewDirVectorsCosAngle, roughness);
    float normalLightDirsGGX            = GeometrySchlickGGX(normalLightDirVectorsCosAngle, roughness);

    return normalLightDirsGGX * normalViewDirsGGX;
}

vec2 IntegrateBRDF(float normalViewVectorsCosAngle, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - normalViewVectorsCosAngle * normalViewVectorsCosAngle);
    V.y = 0.0;
    V.z = normalViewVectorsCosAngle;

    float A = 0.0;
    float B = 0.0; 

    vec3 normalVector = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, normalVector, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySmith(normalVector, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * normalViewVectorsCosAngle);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main() 
{
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}