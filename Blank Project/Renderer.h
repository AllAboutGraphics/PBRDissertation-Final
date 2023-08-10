#pragma once

#include "..\nclgl\OGLRenderer.h"
#include "..\nclgl\Textures.h"
#include "AssetsCache.h"
#include "ObjectRepresentable.h"

#define STB_IMAGE_IMPLEMENTATION

class Camera;
class Shader;
class HeightMap;
class SphereObjectsGrid;
class Skybox;
class LightObjects;
class SphereObject;
class SpaceShipObject;
class SphereWithoutTextureObject;
class PBRHelper;

enum ShaderMatricesEnum
{
	ProjectionMatrix,	
	ModelMatrix,
	ViewMatrix,	
	TextureMatrix,
	ShadowMatrix
};

enum Objects
{
	Sphere,
	SpaceShip,
	All,
	SphereGrid,
	SphereWithoutTexture,
	TotalObjects	//For MaxCount Purpose
};

class Renderer : public OGLRenderer
{
	public:
		Renderer(Window& parent);
		~Renderer();
		void RenderScene() override;
		void UpdateScene(float dt) override;
		void SetupShaderUniforms();
		int GetCurrentModel() const { return ((int)isUsingPhongModel); }

	protected:
		SphereObjectsGrid*			sphereGrid;
		Skybox*						skybox;
		LightObjects*				lightObjects;
		SphereObject*				sphereObject;
		SphereWithoutTextureObject* sphereWithoutTextureObject;
		SpaceShipObject*			spaceShipObject;
		Mesh*						quad;
		Camera*						camera;
		
		PBRHelper*					pbrHelper;

		AssetsCache*				instance = NULL;

		int							currentObject = 0;
		ObjectRepresentable*		currentSelectedObject;

		int							isUsingPhongModel = false;
		int							currentSelectedSkybox = 0;

	private:
		void HandleRenderObjectEvents();
		void RenderObject();
		void UpdateCurrentSelectedObject();

		void ChangeSkybox();

		void SwitchModel();

		// IMGUI
		void SetupIMGui(Window& window);
		void DisplayMenu();

		void PrintFPS();

		//Callbacks
	public:
		void BindShaderDelegate(Shader* currShader);
		void UpdateShaderMatricesDelegate(std::unordered_map<ShaderMatricesEnum, Matrix4> matricesMap = std::unordered_map<ShaderMatricesEnum, Matrix4>());
};
