#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"

class SphereObjectsGrid : public ObjectRepresentable
{
	public:
		SphereObjectsGrid(Renderer* rendererDelegate, Shader* boundShader);
		~SphereObjectsGrid();
		void DrawSpheres();
		void DrawSphereMaterial(float metallicValue, float roughnessValue, Matrix4 transformationMatrix);
		void ImGuiRender(bool shouldShow) override;
		void HandleTextureEvents() override;

	private:
		Mesh*	  sphere;
		Shader*   shader;
		Renderer* renderer;
		//float Clamp(float value, float minimum, float maximum);
};