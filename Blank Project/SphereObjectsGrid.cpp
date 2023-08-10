#include "SphereObjectsGrid.h"
#include "../nclgl/Matrix3.h"

SphereObjectsGrid::SphereObjectsGrid(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	renderer->BindShaderDelegate(shader);
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
}

SphereObjectsGrid::~SphereObjectsGrid()
{
	delete sphere;
}

void SphereObjectsGrid::DrawSpheres()
{
	int	  gridSize = 7;
	float spacing  = 2.1f;

	renderer->BindShaderDelegate(shader);
	
	for (int row = 0; row < gridSize; ++row)
	{
		float metallicValue = (float)row / (float)gridSize;
		for (int col = 0; col < gridSize; ++col)
		{
			float roughnessValue = Clamp((float)col / (float)gridSize, 0.05f, 1.0f);

			Matrix4 transformationMatrix = Matrix4::Translation(Vector3(((col - (gridSize / 2)) * (spacing + 0.5f)) - 5.0f, ((row - (gridSize / 2)) * spacing) - 5.0f, 0.0f));
			DrawSphereMaterial(metallicValue, roughnessValue, transformationMatrix);
		}
	}
}

void SphereObjectsGrid::DrawSphereMaterial(float metallicValue, float roughnessValue, Matrix4 transformationMatrix)
{
	renderer->BindShaderDelegate(shader);
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ModelMatrix] = transformationMatrix;
	renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
	Matrix4 inverseModelMatrix = transformationMatrix.Inverse();
	Matrix4 transposedInverseModelMatrix = inverseModelMatrix.GetTransposedRotation();
	glUniformMatrix3fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, Matrix3(transposedInverseModelMatrix).values);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "metallic"), metallicValue);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "roughness"), roughnessValue);
	sphere->Draw();
}

void SphereObjectsGrid::ImGuiRender(bool shouldShow)
{
}
