#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"
#include "AssetsCache.h"

class SphereWithoutTextureObject : public ObjectRepresentable
{
	public:
		SphereWithoutTextureObject(Renderer* rendererDelegate, Shader* boundShader);
		~SphereWithoutTextureObject();
		void DrawSphere();
		void HandleTextureEvents() override;
		void ImGuiRender(bool shouldShow) override;

	private:
		Mesh*	  sphere;
		Shader*	  shader;
		Renderer* renderer;
		float	  metallicValue;
		float	  roughnessValue;
};