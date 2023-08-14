#version 330 core
out vec4 FragColour;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

// lights
struct LightData
{
//    vec4 isLightOn;
    vec4 lightSwitchAndRadius;
    vec4 lightPositions;
    vec4 lightColors;
//    vec4 lightRadius;
};

layout(std140) uniform LightsData
{
//    LightData lightData[6];
    vec4 lightSwitchAndRadius[6];
    vec4 lightPositions[6];
    vec4 lightColors[6];
};

uniform vec3 camPos;

uniform int isUsingPhongModel;

uniform float specularityPower;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normalVector, vec3 halfwayVector, float roughness)
{
    float a                                   = roughness * roughness;
    float aSquared                            = a * a;
    float normalHalfwayVectorsCosAngle        = max(dot(normalVector, halfwayVector), 0.0);
    float normalHalfwayVectorsCosAngleSquared = normalHalfwayVectorsCosAngle * normalHalfwayVectorsCosAngle;

    float numerator                           = aSquared;
    float denominator                         = (normalHalfwayVectorsCosAngleSquared * (aSquared - 1.0) + 1.0);
    denominator                               = PI * denominator * denominator;

    return numerator / denominator;
}

float GeometrySchlickGGX(float normalViewDirVectorsCosAngle, float roughness)
{
    float r           = (roughness + 1.0);
    float k           = (r*r) / 8.0;

    float numerator   = normalViewDirVectorsCosAngle;
    float denominator = normalViewDirVectorsCosAngle * (1.0 - k) + k;

    return numerator / denominator;
}

float GeometrySmith(vec3 normalVector, vec3 viewDirVector, vec3 lightDirVector, float roughness)
{
    float normalViewDirVectorsCosAngle  = max(dot(normalVector, viewDirVector), 0.0);
    float normalLightDirVectorsCosAngle = max(dot(normalVector, lightDirVector), 0.0);
    float normalViewDirVectorsGGX       = GeometrySchlickGGX(normalViewDirVectorsCosAngle, roughness);
    float normalLightDirVectorsGGX      = GeometrySchlickGGX(normalLightDirVectorsCosAngle, roughness);

    return normalLightDirVectorsGGX * normalViewDirVectorsGGX;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 OrenNayar(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, vec3 albedo)
{
    float NdotV   = clamp(dot(surfaceNormal, viewDirection), 0.0, 1.0);
	float angleVN = acos(NdotV);
	float NdotL   = clamp(dot(surfaceNormal, lightDirection), 0.0, 1.0);
    float angleLN = acos(NdotL);

	float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
	float gamma = cos(angleVN - angleLN);

    float roughness2 = roughness * roughness;

	float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.57));
    float B = 0.45 * (roughness2 / (roughness2 + 0.09));
	float C = sin(alpha) * tan(beta);

    vec3 diffuse = albedo * (NdotL * (A + ((B * max(0.0, gamma)) * C))) / PI;
    return diffuse;
}

vec4 PBR()
{
    vec3 normalVector     = normalize(Normal);
    vec3 viewDirVector    = normalize(camPos - WorldPos);
    vec3 reflectionVector = reflect(-viewDirVector, normalVector); 

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 6; ++i) 
    {
        if(lightSwitchAndRadius[i].x == 0) { continue; }
        vec3 lightDirVector   = normalize(lightPositions[i].xyz - WorldPos);
        vec3 halfwayDirVector = normalize(viewDirVector + lightDirVector);
        float distance        = length(lightPositions[i].xyz - WorldPos);
        float attenuation     = 1.0 / (distance * distance);
        vec3 radiance         = lightColors[i].xyz * attenuation;

        float NDF = DistributionGGX(normalVector, halfwayDirVector, roughness);   
        float G   = GeometrySmith(normalVector, viewDirVector, lightDirVector, roughness);    
        vec3  F   = fresnelSchlick(max(dot(halfwayDirVector, viewDirVector), 0.0), F0);        
        
        vec3  numerator    = NDF * G * F;
        float denominator  = 4.0 * max(dot(normalVector, viewDirVector), 0.0) * max(dot(normalVector, lightDirVector), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3  specular     = numerator / denominator;
        
        vec3 kS  = F;
        vec3 kD  = vec3(1.0) - kS;
        kD      *= 1.0 - metallic;	                
            
        float normalLightDirsCosAngle = max(dot(normalVector, lightDirVector), 0.0);        

        if(isUsingPhongModel == 2)
        {
            Lo += (kD * OrenNayar(lightDirVector, viewDirVector, normalVector, roughness, albedo) + specular) * radiance * normalLightDirsCosAngle;
        }
        else
        {
            Lo += (kD * albedo / PI + specular) * radiance * normalLightDirsCosAngle;
        }
    }   
    
    vec3 F = fresnelSchlickRoughness(max(dot(normalVector, viewDirVector), 0.0), F0, roughness);
    
    vec3 kS  = F;
    vec3 kD  = 1.0 - kS;
    kD      *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, normalVector).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor          = textureLod(prefilterMap, reflectionVector,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf                      = texture(brdfLUT, vec2(max(dot(normalVector, viewDirVector), 0.0), roughness)).rg;
    vec3 specular                  = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient                   = (kD * diffuse + specular) * ao;
    
    vec3 color                     = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    return vec4(color , 1.0);
}

vec4 PhongShading()
{
    vec3 psLightColours[6];
    vec4 finalColour;
    for(int i = 0; i < 6; i++)
    {
        psLightColours[i].x = lightColors[i].x / 300.0;
        psLightColours[i].y = lightColors[i].y / 300.0;
        psLightColours[i].z = lightColors[i].z / 300.0;
    }
    for(int i = 0 ; i < 6 ; i++)
    {
        if(lightSwitchAndRadius[i].x == 0) { continue; }
        vec3 incident	  = normalize(lightPositions[i].xyz - WorldPos);
	    vec3 viewDir	  = normalize(camPos - WorldPos);
	    vec3 halfDir	  = normalize(incident + viewDir);

	    //Diffuse
	    vec3 diffuse	  = albedo;
	    float lambert	  = max(dot(incident, Normal), 0.0);
	    float dist		  = length(lightPositions[i].xyz - WorldPos);
	    float attenuation = 1.0 - clamp(dist / lightSwitchAndRadius[i].y, 0.0, 1.0);

	    //Specularity
	    float specFactor  = clamp(dot(halfDir, Normal), 0.0, 1.0);
	    specFactor		  = pow(specFactor, specularityPower);


	    //Phong Shading Model
	    vec3 surface	  = (diffuse.rgb * psLightColours[i].rgb);
	    finalColour.rgb	 += surface * lambert * attenuation;	//diffuse
	    finalColour.rgb	 += (psLightColours[i].rgb * specFactor) * attenuation * 0.33;	// Specular
	    finalColour.rgb	 += surface * 0.1;	//Ambient
    }
    return finalColour;
}

void main()
{
    if(isUsingPhongModel == 1) { FragColour = PhongShading(); }
    else { FragColour = PBR(); }
}