#include "SphereObject.h"
#include "../nclgl/Matrix3.h"

SphereObject::SphereObject(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	sphere = AssetsCache::GetInstance()->GetMesh("Sphere.msh");
	LoadAllTextures();
}

SphereObject::~SphereObject()
{
	delete sphere;
}

void SphereObject::DrawSphere()
{
	materials[currentTexture].albedoMap->BindDataToTexture();
	materials[currentTexture].aoMap->BindDataToTexture();
	materials[currentTexture].metallicMap->BindDataToTexture();
	materials[currentTexture].normalMap->BindDataToTexture();
	materials[currentTexture].roughnessMap->BindDataToTexture();

	Matrix4 transformationMatrix = Matrix4::Translation(Vector3(-5.0, -5.0, 2.0));
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ModelMatrix] = transformationMatrix;
	renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
	Matrix4 inverseModelMatrix = transformationMatrix.Inverse();
	Matrix4 transposedInverseModelMatrix = inverseModelMatrix.GetTransposedRotation();
	glUniformMatrix3fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, Matrix3(transposedInverseModelMatrix).values);
	sphere->Draw();
}

void SphereObject::DrawAllSpheres()
{
	Vector3 basePosition(-5.0, -5.0, 2.0);
	for (int i = TextureMaterialType::Iron; i < TextureMaterialType::Nothing; i++)
	{
		materials[i].albedoMap->BindDataToTexture();
		materials[i].aoMap->BindDataToTexture();
		materials[i].metallicMap->BindDataToTexture();
		materials[i].normalMap->BindDataToTexture();
		materials[i].roughnessMap->BindDataToTexture();

		Matrix4 transformationMatrix = Matrix4::Translation(basePosition + Vector3(i * 3.0f, 0.0f, 0.0f));
		std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
		shaderMatricesMap[ModelMatrix] = transformationMatrix;
		renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
		Matrix4 inverseModelMatrix = transformationMatrix.Inverse();
		Matrix4 transposedInverseModelMatrix = inverseModelMatrix.GetTransposedRotation();
		glUniformMatrix3fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, Matrix3(transposedInverseModelMatrix).values);
		sphere->Draw();
	}
}

void SphereObject::HandleTextureEvents()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD0)) { currentTexture = 0; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD1)) { currentTexture = 1; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD2)) { currentTexture = 2; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD3)) { currentTexture = 3; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD4)) { currentTexture = 4; }
}

void SphereObject::ImGuiRender(bool shouldShow)
{
	ImGui::Begin("Sphere Object");
	if (ImGui::Button("Iron Texture or Press NUMPAD 0"))	{ currentTexture = TextureMaterialType::Iron;	 }
	if (ImGui::Button("Gold Texture or Press NUMPAD 1"))	{ currentTexture = TextureMaterialType::Gold;	 }
	if (ImGui::Button("Grass Texture or Press NUMPAD 2"))	{ currentTexture = TextureMaterialType::Grass;	 }
	if (ImGui::Button("Plastic Texture or Press NUMPAD 3")) { currentTexture = TextureMaterialType::Plastic; }
	if (ImGui::Button("Wall Texture or Press NUMPAD 4"))	{ currentTexture = TextureMaterialType::Wall;	 }
	ImGui::End();
}

void SphereObject::LoadAllTextures()
{
	renderer->BindShaderDelegate(shader);
	for (int i = TextureMaterialType::Iron; i < TextureMaterialType::Nothing; i++)
	{
		LoadObjectTextures((TextureMaterialType)i);
	}
}

void SphereObject::LoadObjectTextures(TextureMaterialType materialType)
{
	std::string materialTypeDirectoryNameStrings[TextureMaterialType::Nothing] = { "Iron", "Gold", "Grass", "Plastic", "Wall"};
	std::string textureFileName = "PBR/Materials/" + materialTypeDirectoryNameStrings[materialType];
	std::unordered_map<unsigned int, unsigned int> commonParametersMap =
	{
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
		{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
	};

	materials[materialType].albedoMap	 = AssetsCache::GetInstance()->GetTexture(textureFileName + "/albedo.png", "albedoMap", 3, shader, commonParametersMap);
	materials[materialType].normalMap	 = AssetsCache::GetInstance()->GetTexture(textureFileName + "/normal.png", "normalMap", 4, shader, commonParametersMap);
	materials[materialType].metallicMap  = AssetsCache::GetInstance()->GetTexture(textureFileName + "/metallic.png", "metallicMap", 5, shader, commonParametersMap);
	materials[materialType].roughnessMap = AssetsCache::GetInstance()->GetTexture(textureFileName + "/roughness.png", "roughnessMap", 6, shader, commonParametersMap);
	materials[materialType].aoMap		 = AssetsCache::GetInstance()->GetTexture(textureFileName + "/ao.png", "aoMap", 7, shader, commonParametersMap);
}
