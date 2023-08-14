#include "LightObjects.h"
#include "../nclgl/Vector4.h"

LightObjects::LightObjects(Renderer* rendererDelegate, Shader* boundShader) : renderer(rendererDelegate), shader(boundShader)
{
	lightSphereMesh = AssetsCache::GetInstance()->GetMesh("Sphere.msh");
	InstantiateLightPositionsAndColours();
	lightUBO = new UniformBufferObject({ boundShader, AssetsCache::GetInstance()->GetShader(SceneShaders::PBRWithoutTextureShader) }, "LightsData", 0, MAX_LIGHTS * sizeof(LightData));
}

LightObjects::~LightObjects()
{
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		delete light[i];
	}
}

struct NewLightStruct
{
	Vector4 lightSwitchAndRadius[MAX_LIGHTS];
	Vector4 lightPositions[MAX_LIGHTS];
	Vector4 lightColors[MAX_LIGHTS];
};
NewLightStruct newLightData;
void LightObjects::Draw()
{
	static float acceleration = 0.0f;
	acceleration += 2.0f;
	if (!moveLights) { acceleration = 0; }
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		renderer->BindShaderDelegate(shader);
		Vector4		lightPosition = lightBasePositions[i] + lightsOffsetVector[i] + Vector4((moveLights ? sin(acceleration * 0.01f) * 10.0f : 0.0f), 0.0f, 0.0f, 0.0f);
		light[i]->SetPosition(lightPosition);
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "isUsingPhongModel"), renderer->GetCurrentModel());
		glUniform1f(glGetUniformLocation(shader->GetProgram(), "specularityPower"), specularityPower);
		
		//lightUBO->UploadData(&light[i][0], sizeof(LightData), i * sizeof(LightData));
		if (light[i]->GetIsLightOn())
		{
			DrawLightObject(Matrix4::Translation(lightPosition.ToVector3()) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)), lightColours[i]);
		}
	}
	
	int idx = 0;
	for (Light* lgt : light)
	{
		newLightData.lightPositions[idx] = lgt->GetPosition();
		newLightData.lightColors[idx] = lgt->GetColour();
		newLightData.lightSwitchAndRadius[idx] = lgt->GetLightSwitchAndRadius();
		idx++;
	}
	lightUBO->UploadData(&newLightData, sizeof(newLightData), 0);
}

void LightObjects::DrawLightObject(Matrix4 transformationMatrix, Vector4 currLightColour)
{
	Vector4 lightObjectColour = currLightColour;
	renderer->BindShaderDelegate(AssetsCache::GetInstance()->GetShader(SceneShaders::LightObject));
	std::unordered_map<ShaderMatricesEnum, Matrix4> shaderMatricesMap;
	shaderMatricesMap[ModelMatrix] = transformationMatrix;
	renderer->UpdateShaderMatricesDelegate(shaderMatricesMap);
	glUniform4fv(glGetUniformLocation(AssetsCache::GetInstance()->GetShader(SceneShaders::LightObject)->GetProgram(), "objectColour"), 1, &lightObjectColour.x);
	lightSphereMesh->Draw();
}

void LightObjects::HandleEvents()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) { moveLights = !moveLights; }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)) { ToggleLight(0); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) { ToggleLight(1); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) { ToggleLight(2); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) { ToggleLight(3); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) { ToggleLight(4); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) { ToggleLight(5); }
	renderer->BindShaderDelegate(shader);
	//glUniform1iv(glGetUniformLocation(shader->GetProgram(), "isLightOn"), 6, &isLightOn[0]);
}

void LightObjects::ImGuiRender(bool shouldShow)
{
	if (ImGui::CollapsingHeader("Light Settings"))
	{
		if (ImGui::Button(moveLights ? "Stable lights" : "Move lights")) { moveLights = !moveLights; }
		ImGui::SameLine();
		ImGui::Text(" Or use 'L' to to toggle between stationary and moving lights.");

		if (ImGui::Button(light[0]->GetIsLightOn() ? "Turn Light 1 off" : "Turn Light 1 on")) { ToggleLight(0); }
		DrawIMGuiLightOffsetSlider(0, "Light 1 Offset");
		if (ImGui::Button(light[1]->GetIsLightOn() ? "Turn Light 2 off" : "Turn Light 2 on")) { ToggleLight(1); }
		DrawIMGuiLightOffsetSlider(1, "Light 2 Offset");
		if (ImGui::Button(light[2]->GetIsLightOn() ? "Turn Light 3 off" : "Turn Light 3 on")) { ToggleLight(2); }
		DrawIMGuiLightOffsetSlider(2, "Light 3 Offset");
		if (ImGui::Button(light[3]->GetIsLightOn() ? "Turn Light 4 off" : "Turn Light 4 on")) { ToggleLight(3); }
		DrawIMGuiLightOffsetSlider(3, "Light 4 Offset");
		if (ImGui::Button(light[4]->GetIsLightOn() ? "Turn Light 5 off" : "Turn Light 5 on")) { ToggleLight(4); }
		DrawIMGuiLightOffsetSlider(4, "Light 5 Offset");
		if (ImGui::Button(light[5]->GetIsLightOn() ? "Turn Light 6 off" : "Turn Light 6 on")) { ToggleLight(5); }
		DrawIMGuiLightOffsetSlider(5, "Light 6 Offset");
		ImGui::Text("Or use keyboard keys 0, 1, 2, 3, 4, 5 to toggle respective lights.");
		if (ImGui::Button("Reset Lights")) { ResetLights(); }
		DrawLightColourSlider();
		if (shouldShow)
		{
			ImGui::Text("Specularity");
			ImGui::SliderFloat("Specularity Power", &specularityPower, 1.0f, 60.0f);
			ImGui::Text("Attenuation");
			DrawLightAttenuationSlider();
		}
	}
}

void LightObjects::HandleTextureEvents()
{
}

void LightObjects::InstantiateLightPositionsAndColours()
{
	float leftX = -12.0f;
	float topY  =  2.0f;
	float gap	=  12.0f;
	const Vector4 basePositions[MAX_LIGHTS] = { Vector4(leftX,		 topY,		 10.0f, 0.0f),
												Vector4(leftX + gap, topY,		 10.0f, 0.0f),
												Vector4(leftX,		 topY - gap, 10.0f, 0.0f),
												Vector4(leftX + gap, topY - gap, 10.0f, 0.0f),
												Vector4(-5.0f,		 -8.0f,		  2.0f, 0.0f),
												Vector4(-5.0f,		 -2.0f,		  2.0f, 0.0f) };
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		lightColours[i]		  = defaultLightColour;
		lightBasePositions[i] = basePositions[i];
		lightsOffsetVector[i] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	light[0] = new Light(lightBasePositions[0], defaultLightColour * lightFactor, defaultRadius, 1.0f);
	light[1] = new Light(lightBasePositions[1], defaultLightColour * lightFactor, defaultRadius, 1.0f);
	light[2] = new Light(lightBasePositions[2], defaultLightColour * lightFactor, defaultRadius, 1.0f);
	light[3] = new Light(lightBasePositions[3], defaultLightColour * lightFactor, defaultRadius, 1.0f);
	light[4] = new Light(lightBasePositions[4], defaultLightColour * lightFactor, defaultRadiusForCloserLights, 1.0f);	//Bottom
	light[5] = new Light(lightBasePositions[5], defaultLightColour * lightFactor, defaultRadiusForCloserLights, 1.0f);	//Top
}

void LightObjects::ToggleLight(int index)
{
	light[index]->SetIsLightOn(light[index]->GetIsLightOn() ? 0.0f : 1.0f);
}

void LightObjects::DrawIMGuiLightOffsetSlider(int lightIndex, std::string label)
{
	ImGui::SameLine();
	ImGui::SliderFloat3(label.c_str(), &lightsOffsetVector[lightIndex].x, -80.0f, 80.0f);
}

void LightObjects::ResetLights()
{
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		light[i]->SetIsLightOn(1.0f);
		lightsOffsetVector[i] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		lightColours[i] = defaultLightColour;
		light[i]->SetColour(defaultLightColour);
		light[i]->SetRadius(MAX_LIGHTS - i > 2 ? defaultRadius : defaultRadiusForCloserLights);
		moveLights = false;
	}
}

void LightObjects::SetLightColour()
{
	for (int i = 0; i < MAX_LIGHTS; i++) { light[i]->SetColour(lightColours[i] * lightFactor); }
}

void LightObjects::DrawLightColourSlider()
{
	ImGui::Text("Light Colour");
	ImGui::SliderFloat3("Light 1", &(lightColours[0].x), 0.0f, 1.0f);
	ImGui::SliderFloat3("Light 2", &(lightColours[1].x), 0.0f, 1.0f);
	ImGui::SliderFloat3("Light 3", &(lightColours[2].x), 0.0f, 1.0f);
	ImGui::SliderFloat3("Light 4", &(lightColours[3].x), 0.0f, 1.0f);
	ImGui::SliderFloat3("Light 5", &(lightColours[4].x), 0.0f, 1.0f);
	ImGui::SliderFloat3("Light 6", &(lightColours[5].x), 0.0f, 1.0f);
	SetLightColour();
}

void LightObjects::DrawLightAttenuationSlider()
{
	ImGui::SliderFloat("Light 1 Attenuation", &light[0]->GetRadiusReference(), 0.0f, 30.0f);
	ImGui::SliderFloat("Light 2 Attenuation", &light[1]->GetRadiusReference(), 0.0f, 30.0f);
	ImGui::SliderFloat("Light 3 Attenuation", &light[2]->GetRadiusReference(), 0.0f, 30.0f);
	ImGui::SliderFloat("Light 4 Attenuation", &light[3]->GetRadiusReference(), 0.0f, 30.0f);
	ImGui::SliderFloat("Light 5 Attenuation", &light[4]->GetRadiusReference(), 0.0f, 30.0f);
	ImGui::SliderFloat("Light 6 Attenuation", &light[5]->GetRadiusReference(), 0.0f, 30.0f);

}
