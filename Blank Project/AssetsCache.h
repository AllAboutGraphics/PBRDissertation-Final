#pragma once

#include <unordered_map>
#include "../nclgl/Shader.h"

enum SceneShaders
{
	PbrShader,
	EquirectangularToCubeMapShader,
	IrradianceShader,
	PrefilterShader,
	BrdfShader,
	Background,
	LightObject,
	PBRWithoutTextureShader,
	TotalShaders	//For max count
};

struct TextureMaterialProperties
{
	Textures* albedoMap;
	Textures* normalMap;
	Textures* metallicMap;
	Textures* roughnessMap;
	Textures* aoMap;
};

class AssetsCache
{
	private:
		AssetsCache() {}
		static AssetsCache* instance;
		std::unordered_map<SceneShaders, Shader*>  shadersCache;
		std::unordered_map<std::string, Mesh*>	   meshCache;
		std::unordered_map<std::string, Textures*> texturesCache;

		std::unordered_map<unsigned int, unsigned int> commonParametersMap =
		{
			{ GL_TEXTURE_WRAP_S, GL_REPEAT },
			{ GL_TEXTURE_WRAP_T, GL_REPEAT },
			{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
			{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
		};
	
	public:
		AssetsCache(const AssetsCache& obj) = delete;

		static AssetsCache* GetInstance()
		{
			if (instance == NULL) { instance = new AssetsCache(); }
			return instance;
		}

		Shader* GetShader(SceneShaders shaderType)
		{
			if (shadersCache.find(shaderType) == shadersCache.end())
			{
				switch (shaderType)
				{
					case PbrShader:
						shadersCache[shaderType] = new Shader("PBRVertex.glsl", "PBRFragment.glsl");
						break;
					case EquirectangularToCubeMapShader:
						shadersCache[shaderType] = new Shader("CubemapVertex.glsl", "EquirectangularToCubemapFragment.glsl");
						break;
					case IrradianceShader:
						shadersCache[shaderType] = new Shader("CubemapVertex.glsl", "IrradianceConvolutionFragment.glsl");
						break;
					case PrefilterShader:
						shadersCache[shaderType] = new Shader("CubemapVertex.glsl", "PrefilterFragment.glsl");
						break;
					case BrdfShader:
						shadersCache[shaderType] = new Shader("BRDFVertex.glsl", "BRDFFragment.glsl");
						break;
					case Background:
						shadersCache[shaderType] = new Shader("BackgroundVertex.glsl", "BackgroundFragment.glsl");
						break;
					case LightObject:
						shadersCache[shaderType] = new Shader("LightObjectVertex.glsl", "LightObjectFragment.glsl");
						break;
					case PBRWithoutTextureShader:
						shadersCache[shaderType] = new Shader("PBRVertex.glsl", "PBRWithoutTextureFragment.glsl");
						break;
				}
				if (!shadersCache[shaderType]->LoadSuccess()) { return NULL; }
			}
			return shadersCache[shaderType];
		}

		bool LoadAllShaders()
		{
			for (int i = SceneShaders::PbrShader; i < SceneShaders::TotalShaders; i++)
			{
				if (GetShader((SceneShaders)i) == NULL) { return false; }
			}
			return true;
		}

		Mesh* GetMesh(std::string meshFileName)
		{
			if (meshCache.find(meshFileName) == meshCache.end())
			{
				meshCache[meshFileName] = Mesh::LoadFromMeshFile(meshFileName);
			}
			return meshCache[meshFileName];
		}

		Textures* GetTexture(std::string textureFileName, std::string uniformName, unsigned int uniformBindIndex, Shader* boundShader, std::unordered_map<unsigned int, unsigned int> parametersMap, TextureType textureType = TextureType::OrdinaryTexture, bool shouldRepeatTexture = false)
		{
			if (texturesCache.find(textureFileName) == texturesCache.end())
			{
				texturesCache[textureFileName] = new Textures(textureFileName, uniformName, uniformBindIndex, boundShader, parametersMap, textureType, shouldRepeatTexture);
			}
			return texturesCache[textureFileName];
		}
};