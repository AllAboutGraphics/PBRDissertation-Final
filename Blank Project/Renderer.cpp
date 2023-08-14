#include "Renderer.h"
#include "..\nclgl\Light.h"
#include "..\nclgl\HeightMap.h"
#include "..\nclgl\Shader.h"
#include "..\nclgl\Camera.h"
#include "..\nclgl\Matrix3.h"
#include "..\nclgl\Matrix4.h"
#include "SphereObjectsGrid.h"
#include "Skybox.h"
#include "LightObjects.h"
#include "SphereObject.h"
#include "SpaceShipObject.h"
#include "SphereWithoutTextureObject.h"
#include "IMGUI\imgui.h"
#include "IMGUI\imgui_impl_opengl3.h"
#include "IMGUI\imgui_impl_win32.h"
#include "PBRHelper.h"
#include <unordered_map>


AssetsCache* AssetsCache::instance = NULL;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	quad		 = Mesh::GenerateQuad();

	instance	 = AssetsCache::GetInstance();
	if (!(instance->LoadAllShaders())) { init = false; return; }

	sphereObject	= new SphereObject(this, instance->GetShader(SceneShaders::PbrShader));
	spaceShipObject = new SpaceShipObject(this, instance->GetShader(SceneShaders::PbrShader));

	sphereGrid   = new SphereObjectsGrid(this, instance->GetShader(SceneShaders::PBRWithoutTextureShader));
	skybox = new Skybox(this, instance->GetShader(SceneShaders::Background));
	sphereWithoutTextureObject = new SphereWithoutTextureObject(this, instance->GetShader(SceneShaders::PBRWithoutTextureShader));
	lightObjects = new LightObjects(this, instance->GetShader(SceneShaders::PbrShader));

	SetupShaderUniforms();	//Setting pbr shader and background shader default uniform values
	pbrHelper = new PBRHelper(quad, skybox, this);
	
	currentSelectedSkybox = 0;

	// IMGUI
	SetupIMGui(parent);
	currentSelectedObject = sphereObject;

	camera = new Camera(0.0f, 0.0f, Vector3(-5.0f, -5.0f, 20.0f));

	projMatrix = Matrix4::Perspective(0.1f, 100.0f, (float)width / (float)height, 30.0f);
	BindShader(instance->GetShader(SceneShaders::PbrShader));
	UpdateShaderMatrices();
	BindShader(instance->GetShader(SceneShaders::Background));
	UpdateShaderMatrices();

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	init = true;
}

Renderer::~Renderer()
{
	delete camera;
	delete quad;
	delete sphereGrid;
	delete skybox;
	delete lightObjects;
	delete pbrHelper;
}

void Renderer::RenderScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	RenderObject();
	lightObjects->Draw();
	skybox->RenderSkybox(pbrHelper->GetEnvironmentCubeMap(currentSelectedSkybox));
	DisplayMenu();
}

void Renderer::UpdateScene(float dt)
{
	HandleRenderObjectEvents();
	lightObjects->HandleEvents();
	currentSelectedObject->HandleTextureEvents();
	camera->UpdateCamera(dt, currentSelectedObject == sphereGrid || currentObject == All);
	viewMatrix = camera->BuildViewMatrix();
	if (currentObject == Objects::SphereGrid || currentObject == Objects::SphereWithoutTexture) { BindShader(instance->GetShader(SceneShaders::PBRWithoutTextureShader)); }
	else { BindShader(instance->GetShader(SceneShaders::PbrShader)); }
	
	Vector3 cameraPosition = camera->GetPosition();
	glUniform3fv(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "camPos"), 1, &(cameraPosition.x));

	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrHelper->GetIrradianceMap(currentSelectedSkybox));
	// bind prefilter Map data
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pbrHelper->GetPrefilterMap(currentSelectedSkybox));
	// bind LUT IBL data
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, pbrHelper->GetBrdfLUTTexture(currentSelectedSkybox));
}


void Renderer::SetupShaderUniforms()
{
	BindShader(instance->GetShader(SceneShaders::PbrShader));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "irradianceMap"), 0);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "prefilterMap"), 1);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "brdfLUT"), 2);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "albedoMap"), 3);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "normalMap"), 4);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "metallicMap"), 5);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "roughnessMap"), 6);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "aoMap"), 7);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PbrShader)->GetProgram(), "isUsingPhongModel"), GetCurrentModel());

	BindShader(instance->GetShader(SceneShaders::Background));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::Background)->GetProgram(), "environmentMap"), 0);

	BindShader(instance->GetShader(SceneShaders::PBRWithoutTextureShader));
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "irradianceMap"), 0);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "prefilterMap"), 1);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "brdfLUT"), 2);
	glUniform3f(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "albedo"), 0.5f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "ao"), 1.0f);
	glUniform1i(glGetUniformLocation(instance->GetShader(SceneShaders::PBRWithoutTextureShader)->GetProgram(), "isUsingPhongModel"), GetCurrentModel());
}

void Renderer::HandleRenderObjectEvents()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
	{
		currentObject++;
		currentObject %= Objects::TotalObjects;
		UpdateCurrentSelectedObject();
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		camera->ToggleCaptureMouseEvents();
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_H))
	{
		ChangeSkybox();
	}
}

void Renderer::RenderObject()
{
	lightObjects->UpdateShaderReference(AssetsCache::GetInstance()->GetShader(SceneShaders::PbrShader));
	switch (currentObject)
	{
		case Sphere:
			sphereObject->DrawSphere();
			break;
		case SpaceShip:
			spaceShipObject->Draw();
			break;
		case All:
			sphereObject->DrawAllSpheres();
			spaceShipObject->SetToDefaults();
			spaceShipObject->Draw(Vector3(-10.0f, 0.0f, 0.0f));
			break;
		case SphereGrid:
			lightObjects->UpdateShaderReference(AssetsCache::GetInstance()->GetShader(SceneShaders::PBRWithoutTextureShader));
			sphereGrid->DrawSpheres();
			break;
		case SphereWithoutTexture:
			lightObjects->UpdateShaderReference(AssetsCache::GetInstance()->GetShader(SceneShaders::PBRWithoutTextureShader));
			sphereWithoutTextureObject->DrawSphere();
			break;
	}
}

void Renderer::UpdateCurrentSelectedObject()
{
	currentObject = (Objects)(currentObject);
	switch (currentObject)
	{
		case Sphere:
			currentSelectedObject = sphereObject;
			break;
		case SpaceShip:
			currentSelectedObject = spaceShipObject;
			break;
		case All:
			currentSelectedObject = spaceShipObject;
			break;
		case SphereGrid:
			currentSelectedObject = sphereGrid;
			break;
		case SphereWithoutTexture:
			currentSelectedObject = sphereWithoutTextureObject;
			break;
	}
}

void Renderer::ChangeSkybox()
{
	currentSelectedSkybox++;
	currentSelectedSkybox %= MAX_SKYMAPS;
}

void Renderer::SwitchModel()
{
	isUsingPhongModel = (isUsingPhongModel == 1) ? 0 : 1;
}

//IMGUI
void Renderer::SetupIMGui(Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_InitForOpenGL(window.GetHandle());
	ImGui_ImplOpenGL3_Init();
}

void Renderer::DisplayMenu()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin(isUsingPhongModel == 1 ? "Phong Shading" : "PBR");
	PrintFPS();
	if (ImGui::CollapsingHeader("Select Model"))
	{
		if (ImGui::Button(isUsingPhongModel == 1 ? "   Use PBR   " : "Use Phong Shading")) { SwitchModel(); }
		if (isUsingPhongModel != 1)
		{
			ImGui::SameLine();
			if (ImGui::Button(isUsingPhongModel == 0 ? " Use Oren Nayar " : " Use regular PBR "))
			{
				isUsingPhongModel = isUsingPhongModel == 0 ? 2 : 0;
			}
		}
	}
	if (ImGui::CollapsingHeader("Objects Settings"))
	{
		if (ImGui::Button("Sphere With Textures")) { currentObject = 0; }
		ImGui::SameLine();
		if (ImGui::Button("Spaceship")) { currentObject = 1; }
		ImGui::SameLine();
		if (ImGui::Button("All Textured")) { currentObject = 2; }
		if (ImGui::Button("Sphere Grid without texture")) { currentObject = 3; }
		ImGui::SameLine();
		if (ImGui::Button("Sphere without texture")) { currentObject = 4; }
		UpdateCurrentSelectedObject();
		ImGui::Text("Or press 'C' to change the object.");
	}
	lightObjects->ImGuiRender(isUsingPhongModel == 1);
	if (ImGui::Button(camera->GetCaptureMouseEvents() ? "Freeze Mouse" : "Un-freeze Mouse")) { camera->ToggleCaptureMouseEvents(); }
	ImGui::SameLine();
	std::string mouseMovementCaptureString = " Or press 'P' to ";
	mouseMovementCaptureString += (camera->GetCaptureMouseEvents() ? "freeze" : "un-freeze");
	mouseMovementCaptureString += " mouse movement updates.";
	ImGui::Text(mouseMovementCaptureString.c_str());
	if (ImGui::Button("Set Focus on Object")) { camera->SetFocusOnObject(currentObject == 2 || currentObject == 3); }
	ImGui::SameLine();
	ImGui::Text(" Use 'F' to set focus on object.");
	currentSelectedObject->ImGuiRender(currentObject != 2);
	if (ImGui::Button("Change Skybox")) { ChangeSkybox(); }
	ImGui::SameLine();
	ImGui::Text(" Use 'H' to change the skybox.");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::PrintFPS()
{
	static int   ms_per_frame_idx = 0;
	static float ms_per_frame[60] = { 0 };
	static float ms_per_frame_accum = 0.0f;
	ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
	ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
	ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
	ms_per_frame_idx = (ms_per_frame_idx + 1) % 60;
	const float ms_per_frame_avg = ms_per_frame_accum / 60;
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);
	ImGui::PopStyleColor();
}

//Callbacks
void Renderer::BindShaderDelegate(Shader* currShader)
{
	BindShader(currShader);
}

void Renderer::UpdateShaderMatricesDelegate(std::unordered_map<ShaderMatricesEnum, Matrix4> matricesMap)
{
	for (std::pair<ShaderMatricesEnum, Matrix4> item : matricesMap)
	{
		switch (item.first)
		{
		case ProjectionMatrix:
			projMatrix = item.second;
			break;
		case ModelMatrix:
			modelMatrix = item.second;
			break;
		case ViewMatrix:
			viewMatrix = item.second;
			break;
		case TextureMatrix:
			textureMatrix = item.second;
			break;
		case ShadowMatrix:
			shadowMatrix = item.second;
			break;
		}
	}
	UpdateShaderMatrices();
}