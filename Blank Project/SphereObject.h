#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"
#include "AssetsCache.h"
#include "..\nclgl\Textures.h"
#include "ObjectRepresentable.h"


class SphereObject : public ObjectRepresentable
{
	enum TextureMaterialType
	{
		Iron,
		Gold,
		Grass,
		Plastic,
		Wall,
		Nothing	//For max count Purpose
	};

	public:
		SphereObject(Renderer* rendererDelegate, Shader* boundShader);
		~SphereObject();
		void DrawSphere();
		void DrawAllSpheres();
		void HandleTextureEvents() override;
		void ImGuiRender(bool shouldShow = true) override;

	private:
		Mesh*					  sphere;
		Shader*					  shader;
		Renderer*				  renderer;
		TextureMaterialProperties materials[TextureMaterialType::Nothing];
		int						  currentTexture = 0;

		void LoadAllTextures();
		void LoadObjectTextures(TextureMaterialType materialType);
};