#pragma once

#include <vector>
#include "Shader.h"

class UniformBufferObject
{
	public:
		UniformBufferObject(std::vector<Shader*> linkedShadersList, std::string blockName, unsigned int blockBindingId, int dataSize, unsigned int arrayBindingPointIndex = 0, unsigned int bTarget = GL_UNIFORM_BUFFER);
		~UniformBufferObject();

		void UploadData(void* data, int dataSize, GLintptr offset);

	private:
		unsigned int uboID;
		unsigned int bindTarget;
};