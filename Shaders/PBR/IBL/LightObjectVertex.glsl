#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

void main()
{
	mat4 mvp	= projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
}