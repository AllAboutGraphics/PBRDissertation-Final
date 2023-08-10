#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"

class Skybox
{
	public:
		Skybox(Renderer* rendererDelegate, Shader* boundShader);
		~Skybox();
		void RenderSkybox(unsigned int& envCubemap);
		void Draw(unsigned int& envCubemap);

	private:
		Mesh*	  cubeForSkybox;
		Shader*   shader;
		Renderer* renderer;
};