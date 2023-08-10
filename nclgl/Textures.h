#pragma once
#include <string>
#include <unordered_map>
#include "Shader.h"

enum TextureType
{
	OrdinaryTexture,
	HDRTexture
};

class Textures
{
	public:
		Textures(std::string textureFileName, std::string uniformName, unsigned int uniformBindingIndex, Shader* shader, std::unordered_map<unsigned int, unsigned int> parametersMap, TextureType textureType = OrdinaryTexture, bool shouldRepeatTextures = false);
		void BindDataToTexture();
		void UpdateUniformBoundIndex(unsigned int& updatedBoundIndex) { uniformBoundIndex = updatedBoundIndex; }
		void UpdateShaderReference(Shader* updatedShaderReference) { shaderReference = updatedShaderReference; }

	private:
		unsigned int uniformVariableId;
		unsigned int uniformBoundIndex;
		Shader*		 shaderReference;
		TextureType  textureType;

		unsigned int LoadTexture(const char* fileName, std::unordered_map<unsigned int, unsigned int> parametersMap);
		unsigned int LoadHDREnvironmentMap(const char* fileName, std::unordered_map<unsigned int, unsigned int> parametersMap);
		void SetupTexture(std::string textureFileName, std::string uniformName);
		void SetTextureParameters(std::unordered_map<unsigned int, unsigned int> parametersMap);
};