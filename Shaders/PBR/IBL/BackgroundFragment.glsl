#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 envColour = textureLod(environmentMap, WorldPos, 0.0).rgb;
    
    envColour = envColour / (envColour + vec3(1.0));
    envColour = pow(envColour, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColour, 1.0);
}
