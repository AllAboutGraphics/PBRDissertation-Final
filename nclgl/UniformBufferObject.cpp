#include "UniformBufferObject.h"

UniformBufferObject::UniformBufferObject(std::vector<Shader*> linkedShadersList, std::string blockName, unsigned int blockBindingId, int dataSize, unsigned int arrayBindingPointIndex, unsigned int bTarget): bindTarget(bTarget)
{
	for (Shader* shader : linkedShadersList)
	{
		unsigned int programLocation = glGetUniformBlockIndex(shader->GetProgram(), "LightsData");
		glUniformBlockBinding(shader->GetProgram(), programLocation, blockBindingId);
	}
	glGenBuffers(1, &uboID);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsUBO);
	glBindBuffer(bTarget, uboID);
	glBufferData(bTarget, dataSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(bTarget, 0);
	glBindBufferRange(bTarget, 0, uboID, 0, dataSize);
}

UniformBufferObject::~UniformBufferObject()
{
}

void UniformBufferObject::UploadData(void* data, int dataSize, GLintptr offset)
{
	glBindBuffer(bindTarget, uboID);
	glBufferSubData(bindTarget, offset, dataSize, data);
	glBindBuffer(bindTarget, 0);
}
