#include "PBRHelper.h"
#include "Skybox.h"

PBRHelper::PBRHelper(Mesh* quadRef, Skybox* skyboxRef, Renderer* rendererDelegate) : quad(quadRef), skybox(skyboxRef), renderer(rendererDelegate)
{
	instance = AssetsCache::GetInstance();
	SetupFrameBuffer();
	LoadHDRTextures();
	for (int i = 0; i < MAX_SKYMAPS; i++) { InitPBR(i); }
}

PBRHelper::~PBRHelper()
{
}

void PBRHelper::SetupFrameBuffer(/*unsigned int& captureFBO, unsigned int& captureRBO*/)
{
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
}

void PBRHelper::LoadHDRTextures()
{
	hdrTexture[0] = LoadHDREnvironmentMap(instance->GetShader(SceneShaders::Background), "PBR/HDR/HDRSkyCloud.hdr");
	hdrTexture[1] = LoadHDREnvironmentMap(instance->GetShader(SceneShaders::Background), "PBR/HDR/newport_loft.hdr");
}

Textures* PBRHelper::LoadHDREnvironmentMap(Shader* shader, std::string fileName)
{
	std::unordered_map<unsigned int, unsigned int> commonParametersMap =
	{
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_LINEAR },
		{ GL_TEXTURE_MAG_FILTER, GL_LINEAR }
	};
	Textures* hdrTexture = new Textures(fileName, "", 0, shader, commonParametersMap, HDRTexture);
	return hdrTexture;
}

void PBRHelper::CreateCubeMap(unsigned int& cubeMapID, int width, int height, bool shouldGenerateMipMaps)
{
	glGenTextures(1, &cubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, shouldGenerateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (shouldGenerateMipMaps) { glGenerateMipmap(GL_TEXTURE_CUBE_MAP); }
}

void PBRHelper::CreateProjectionAndViewMatricesForCubemapFaceDirections()
{
	captureProjection = Matrix4::Perspective(0.1f, 10.0f, 1.0f, 90.0f);
	captureViews = new Matrix4[6];
	captureViews[0] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
	captureViews[1] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
	captureViews[2] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	captureViews[3] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
	captureViews[4] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f));
	captureViews[5] = Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f));
}

void PBRHelper::ConvertHDREquirectangularMapToCubeMap(int index)
{
	renderer->BindShaderDelegate(instance->GetShader(SceneShaders::EquirectangularToCubeMapShader));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::EquirectangularToCubeMapShader)->GetProgram(), "equirectangularMap"), 0);

	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ProjectionMatrix] = captureProjection;
	//renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);

	hdrTexture[index]->BindDataToTexture();

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shaderMatricesMap[ViewMatrix] = captureViews[i];
		renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap[index], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		skybox->Draw(envCubemap[index]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap[index]);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void PBRHelper::CreateIrradianceCubeMap(int index)
{
	CreateCubeMap(irradianceMap[index], 32, 32);

	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap[index]);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
}

void PBRHelper::SetUniformsInIrradianceShader(int index)
{
	renderer->BindShaderDelegate(instance->GetShader(SceneShaders::IrradianceShader));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::IrradianceShader)->GetProgram(), "environmentMap"), 0);
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ProjectionMatrix] = captureProjection;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap[index]);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shaderMatricesMap[ViewMatrix] = captureViews[i];
		renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap[index], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Draw(envCubemap[index]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBRHelper::PerformMonteCarloSimulationOnPrefilterMap(int index)
{
	renderer->BindShaderDelegate(instance->GetShader(SceneShaders::PrefilterShader));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PrefilterShader)->GetProgram(), "environmentMap"), 0);
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ProjectionMatrix] = captureProjection;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap[index]);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(instance->GetShader(SceneShaders::PrefilterShader)->GetProgram(), "roughness"), roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shaderMatricesMap[ViewMatrix] = captureViews[i];
			renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap[index], mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			skybox->Draw(envCubemap[index]);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBRHelper::BindLUT(int index)
{
	glGenTextures(1, &brdfLUTTexture[index]);

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture[index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture[index], 0);

	glViewport(0, 0, 512, 512);
	renderer->BindShaderDelegate(instance->GetShader(SceneShaders::BrdfShader));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBRHelper::InitPBR(int index)
{
	CreateCubeMap(envCubemap[index]);
	CreateProjectionAndViewMatricesForCubemapFaceDirections();
	ConvertHDREquirectangularMapToCubeMap(index);
	//Irradiance Cube Map for Diffuse calculation
	CreateIrradianceCubeMap(index);
	SetUniformsInIrradianceShader(index);

	//IBL Specular
	CreateCubeMap(prefilterMap[index], 128, 128, true);
	PerformMonteCarloSimulationOnPrefilterMap(index);
	BindLUT(index);
}
