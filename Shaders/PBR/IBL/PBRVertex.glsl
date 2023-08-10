#version 330 core
in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main()
{
    TexCoords = texCoord;
    WorldPos = vec3(modelMatrix * vec4(position, 1.0));
    Normal = normalMatrix * normal;   

    gl_Position =  projMatrix * viewMatrix * vec4(WorldPos, 1.0);
}