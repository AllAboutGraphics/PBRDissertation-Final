#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"
#include "AssetsCache.h"
#include "..\nclgl\Textures.h"


class SpaceShipObject : public ObjectRepresentable
{
	enum SpaceShipTextureMaterialType
	{
		SpaceShip,
		Iron,
		Gold,
		Grass,
		Plastic,
		Wall,
		Total	//For max count Purpose
	};

	public:
		SpaceShipObject(Renderer* rendererDelegate, Shader* boundShader);
		~SpaceShipObject();
		void Draw(Vector3 translateBy = Vector3(0.0f, 0.0f, 0.0f));
		void HandleTextureEvents() override;
		void UpdateAutoRotate(bool shouldRotate);
		void SetToDefaults();
		void ImGuiRender(bool shouldShow = true) override;

	private:
		Mesh*					  spaceShipMesh;
		Shader*					  shader;
		Renderer*				  renderer;
		TextureMaterialProperties material[SpaceShipTextureMaterialType::Total];
		float					  rotationAngle	   = 0.0f;
		bool					  shouldAutoRotate = false;
		int						  currentTexture   = 0;

		void LoadAllTextures();
		void LoadObjectTextures(SpaceShipTextureMaterialType materialType);
};