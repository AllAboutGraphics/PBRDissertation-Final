#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normalVector, vec3 halfVector, float roughness)
{
    float a                                = roughness * roughness;
    float aSquared                         = a * a;
    float normalHalfVectorsCosAngle        = max(dot(normalVector, halfVector), 0.0);
    float normalHalfVectorsCosAngleSquared = normalHalfVectorsCosAngle * normalHalfVectorsCosAngle;

    float numerator   = aSquared;
    float denominator = (normalHalfVectorsCosAngleSquared * (aSquared - 1.0) + 1.0);
    denominator       = PI * denominator * denominator;

    return numerator / denominator;
}


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

vec3 ImportanceSampleGGX(vec2 Xi, vec3 normalDirVector, float roughness)
{
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	vec3 halfwayVector;
	halfwayVector.x = cos(phi) * sinTheta;
	halfwayVector.y = sin(phi) * sinTheta;
	halfwayVector.z = cosTheta;
	
	vec3 up        = abs(normalDirVector.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, normalDirVector));
	vec3 bitangent = cross(normalDirVector, tangent);
	
	vec3 sampleVec = tangent * halfwayVector.x + bitangent * halfwayVector.y + normalDirVector * halfwayVector.z;
	return normalize(sampleVec);
}

void main()
{		
    vec3 normalDirVector     = normalize(WorldPos);
    // make the simplifying assumption that V equals R equals the normal 
    vec3 reflectionDirVector = normalDirVector;
    vec3 viewDirVector       = reflectionDirVector;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor   = vec3(0.0);
    float totalWeight       = 0.0;
    
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi                = Hammersley(i, SAMPLE_COUNT);
        vec3 halfwayBiasVector = ImportanceSampleGGX(Xi, normalDirVector, roughness);
        vec3 L  = normalize(2.0 * dot(viewDirVector, halfwayBiasVector) * halfwayBiasVector - viewDirVector);

        float NdotL = max(dot(normalDirVector, L), 0.0);
        if(NdotL > 0.0)
        {
            float distributionGgx        = DistributionGGX(normalDirVector, halfwayBiasVector, roughness);
            float normalHalfwayCosAngle  = max(dot(normalDirVector, halfwayBiasVector), 0.0);
            float halfwayViewDirCosAngle = max(dot(halfwayBiasVector, viewDirVector), 0.0);
            float pdf                    = distributionGgx * normalHalfwayCosAngle / (4.0 * halfwayViewDirCosAngle) + 0.0001; 

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel    = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample   = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}
