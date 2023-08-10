#include "SpaceShipObject.h"
#include "../nclgl/Matrix3.h"

SpaceShipObject::SpaceShipObject(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	spaceShipMesh = AssetsCache::GetInstance()->GetMesh("PBR/SpaceShip.msh");
	//LoadObjectTextures();
	LoadAllTextures();
}

SpaceShipObject::~SpaceShipObject()
{
	delete spaceShipMesh;
}

void SpaceShipObject::Draw(Vector3 translateBy)
{
	material[currentTexture].albedoMap->BindDataToTexture();
	material[currentTexture].aoMap->BindDataToTexture();
	material[currentTexture].metallicMap->BindDataToTexture();
	material[currentTexture].normalMap->BindDataToTexture();
	material[currentTexture].roughnessMap->BindDataToTexture();
	
	rotationAngle += shouldAutoRotate ? 0.1f : 0.0f;
	
	Matrix4 transformationMatrix = Matrix4::Translation(Vector3(-5.0, -5.0, -2.0) + translateBy) * Matrix4::Rotation(rotationAngle, Vector3(0.0f, 1.0f, 0.0f));
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ModelMatrix] = transformationMatrix;
	renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
	Matrix4 inverseModelMatrix = transformationMatrix.Inverse();
	Matrix4 transposedInverseModelMatrix = inverseModelMatrix.GetTransposedRotation();
	glUniformMatrix3fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, Matrix3(transposedInverseModelMatrix).values);
	spaceShipMesh->Draw();
}

void SpaceShipObject::HandleTextureEvents()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
	{
		shouldAutoRotate = !shouldAutoRotate;
		if (!shouldAutoRotate) { rotationAngle = 0.0f; }
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD0)) { currentTexture = SpaceShipTextureMaterialType::SpaceShip; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD1)) { currentTexture = SpaceShipTextureMaterialType::Iron; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD2)) { currentTexture = SpaceShipTextureMaterialType::Gold; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD3)) { currentTexture = SpaceShipTextureMaterialType::Grass; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD4)) { currentTexture = SpaceShipTextureMaterialType::Plastic; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_NUMPAD5)) { currentTexture = SpaceShipTextureMaterialType::Wall; }
}

void SpaceShipObject::UpdateAutoRotate(bool shouldRotate)
{
	shouldAutoRotate = shouldRotate;
	if (!shouldAutoRotate) { rotationAngle = 0.0f; }
}

void SpaceShipObject::SetToDefaults()
{
	UpdateAutoRotate(false);
	currentTexture = 0;
}

void SpaceShipObject::ImGuiRender(bool shouldShow)
{
	if (shouldShow)
	{
		ImGui::Begin("Spaceship Object");
		if (ImGui::Button(shouldAutoRotate ? "Stop Rotating" : "Rotate"))
		{
			UpdateAutoRotate(!shouldAutoRotate);
		}
		if (ImGui::Button("Spaceship Texture or Press NUMPAD 0")) { currentTexture = SpaceShipTextureMaterialType::SpaceShip; }
		if (ImGui::Button("Iron Texture or Press NUMPAD 1")) { currentTexture = SpaceShipTextureMaterialType::Iron; }
		if (ImGui::Button("Gold Texture or Press NUMPAD 2")) { currentTexture = SpaceShipTextureMaterialType::Gold; }
		if (ImGui::Button("Grass Texture or Press NUMPAD 3")) { currentTexture = SpaceShipTextureMaterialType::Grass; }
		if (ImGui::Button("Plastic Texture or Press NUMPAD 4")) { currentTexture = SpaceShipTextureMaterialType::Plastic; }
		if (ImGui::Button("Wall Texture or Press NUMPAD 5")) { currentTexture = SpaceShipTextureMaterialType::Wall; }
		ImGui::End();
	}
}

void SpaceShipObject::LoadAllTextures()
{
	renderer->BindShaderDelegate(shader);
	for (int i = SpaceShipTextureMaterialType::SpaceShip; i < SpaceShipTextureMaterialType::Total; i++)
	{
		LoadObjectTextures((SpaceShipTextureMaterialType)i);
	}
}

void SpaceShipObject::LoadObjectTextures(SpaceShipTextureMaterialType materialType)
{
	std::string materialTypeDirectoryNameStrings[SpaceShipTextureMaterialType::Total] = { "SpaceShip", "Iron", "Gold", "Grass", "Plastic", "Wall" };
	std::string textureFileName = "PBR/Materials/" + materialTypeDirectoryNameStrings[materialType];
	std::unordered_map<unsigned int, unsigned int> commonParametersMap =
	{
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
		{GL_TEXTURE_MAG_FILTER, GL_LINEAR}
	};

	/*material.albedoMap    = new Textures(textureFileName + "/albedo.png", "albedoMap", 3, shader, commonParametersMap);
	material.normalMap    = new Textures(textureFileName + "/normal.png", "normalMap", 4, shader, commonParametersMap);
	material.metallicMap  = new Textures(textureFileName + "/metallic.png", "metallicMap", 5, shader, commonParametersMap);
	material.roughnessMap = new Textures(textureFileName + "/roughness.png", "roughnessMap", 6, shader, commonParametersMap);
	material.aoMap		  = new Textures(textureFileName + "/ao.png", "aoMap", 7, shader, commonParametersMap);*/

	material[materialType].albedoMap	= AssetsCache::GetInstance()->GetTexture(textureFileName + "/albedo.png", "albedoMap", 3, shader, commonParametersMap);
	material[materialType].normalMap	= AssetsCache::GetInstance()->GetTexture(textureFileName + "/normal.png", "normalMap", 4, shader, commonParametersMap);
	material[materialType].metallicMap  = AssetsCache::GetInstance()->GetTexture(textureFileName + "/metallic.png", "metallicMap", 5, shader, commonParametersMap);
	material[materialType].roughnessMap = AssetsCache::GetInstance()->GetTexture(textureFileName + "/roughness.png", "roughnessMap", 6, shader, commonParametersMap);
	material[materialType].aoMap	    = AssetsCache::GetInstance()->GetTexture(textureFileName + "/ao.png", "aoMap", 7, shader, commonParametersMap);
}