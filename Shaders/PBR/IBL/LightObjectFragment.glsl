#version 330 core

uniform vec4 objectColour;

out vec4 fragColour;
void main(void)
{
	fragColour = objectColour;
}