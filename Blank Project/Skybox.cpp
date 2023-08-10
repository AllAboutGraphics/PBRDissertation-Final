#include "Skybox.h"

Skybox::Skybox(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	cubeForSkybox = Mesh::GenerateCubeForCubeMap();
}

Skybox::~Skybox()
{
	delete cubeForSkybox;
}

void Skybox::RenderSkybox(unsigned int& envCubemap)
{
	renderer->BindShaderDelegate(shader);
	renderer->UpdateShaderMatricesDelegate();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	Draw(envCubemap);
}

void Skybox::Draw(unsigned int& envCubemap)
{
	cubeForSkybox->Draw();
}
