#version 330 core
in vec3 position;

out vec3 WorldPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main()
{
    WorldPos    = position;  
    gl_Position =  projMatrix * viewMatrix * vec4(WorldPos, 1.0);
}