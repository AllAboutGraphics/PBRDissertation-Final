#include <iostream>
#include "Textures.h"
#include "glad\glad.h"
#include "..\nclgl\stb_image.h"
#include "common.h"

Textures::Textures(std::string textureFileName, std::string uniformName, unsigned int uniformBindingIndex, Shader* shader, std::unordered_map<unsigned int, unsigned int> parametersMap, TextureType textureType, bool shouldRepeatTextures)
{
	shaderReference	  = shader;
	uniformBoundIndex = uniformBindingIndex;
	this->textureType = textureType;
	if (textureType == OrdinaryTexture)
	{
		uniformVariableId = LoadTexture((TEXTUREDIR + textureFileName).c_str(), parametersMap);
		SetupTexture(textureFileName, uniformName);
	}
	else if (textureType == HDRTexture)
	{
		stbi_set_flip_vertically_on_load(true);
		uniformVariableId = LoadHDREnvironmentMap((TEXTUREDIR + textureFileName).c_str(), parametersMap);
	}
}

unsigned int Textures::LoadTexture(const char* fileName, std::unordered_map<unsigned int, unsigned int> parametersMap)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(fileName, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		SetTextureParameters(parametersMap);

		stbi_image_free(data);
		stbi_set_flip_vertically_on_load(false);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << fileName << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int Textures::LoadHDREnvironmentMap(const char* fileName, std::unordered_map<unsigned int, unsigned int> parametersMap)
{
	unsigned int textureID;
	int width, height, nrComponents;
	float* data = stbi_loadf(fileName, &width, &height, &nrComponents, 0);

	if (data)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		SetTextureParameters(parametersMap);

		stbi_image_free(data);
		stbi_set_flip_vertically_on_load(false);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
		return 0;
	}
	return textureID;
}

void Textures::SetupTexture(std::string textureFileName, std::string uniformName)
{
	glUniform1i(glGetUniformLocation(shaderReference->GetProgram(), uniformName.c_str()), uniformBoundIndex);
}

void Textures::SetTextureParameters(std::unordered_map<unsigned int, unsigned int> parametersMap)
{
	for (std::pair<unsigned int, unsigned int> parameter : parametersMap)
	{
		glTexParameteri(GL_TEXTURE_2D, parameter.first, parameter.second);
	}
}

void Textures::BindDataToTexture()
{
	glActiveTexture(GL_TEXTURE0 + uniformBoundIndex);
	glBindTexture(GL_TEXTURE_2D, uniformVariableId);
}