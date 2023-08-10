#include "SphereWithoutTextureObject.h"
#include "../nclgl/Matrix3.h"

SphereWithoutTextureObject::SphereWithoutTextureObject(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	sphere		   = AssetsCache::GetInstance()->GetMesh("Sphere.msh");
	roughnessValue = 0.05f;
	metallicValue  = 0.05f;
}

SphereWithoutTextureObject::~SphereWithoutTextureObject()
{
}

void SphereWithoutTextureObject::DrawSphere()
{
	renderer->BindShaderDelegate(shader);
	Matrix4 transformationMatrix = Matrix4::Translation(Vector3(-5.0, -5.0, 2.0));
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

void SphereWithoutTextureObject::HandleTextureEvents()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))    { roughnessValue = Clamp(roughnessValue + 0.001f, 0.05f, 1.0f); }
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))  { roughnessValue = Clamp(roughnessValue - 0.001f, 0.05f, 1.0f); }
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))  { metallicValue  = Clamp(metallicValue - 0.001f, 0.05f, 1.0f); }
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) { metallicValue  = Clamp(metallicValue + 0.001f, 0.05f, 1.0f); }
}

void SphereWithoutTextureObject::ImGuiRender(bool shouldShow)
{
	if (!renderer->GetCurrentModel() || renderer->GetCurrentModel() == 2)
	{
		ImGui::Begin("Sphere Object without texture");
		ImGui::SliderFloat(" Roughness", &roughnessValue, 0.05f, 1.0f);
		ImGui::SameLine();
		ImGui::Text("Arrow UP and Arrow DOWN");
		ImGui::SliderFloat(" Metallic", &metallicValue, 0.05f, 1.0f);
		ImGui::SameLine();
		ImGui::Text("Arrow LEFT and Arrow RIGHT");
		ImGui::End();
	}
}