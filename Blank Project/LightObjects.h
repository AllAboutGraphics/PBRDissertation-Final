#pragma once

#include "..\nclgl\Shader.h"
#include "Renderer.h"
#include "..\nclgl\UniformBufferObject.h"

#define MAX_LIGHTS 6

class LightObjects : public ObjectRepresentable
{
	public:
		LightObjects(Renderer* rendererDelegate, Shader* boundShader);
		~LightObjects();
		void Draw();
		void DrawLightObject(Matrix4 transformationMatrix, Vector4 currLightColour);
		void HandleEvents();
		void UpdateShaderReference(Shader* shaderRef) { shader = shaderRef; }
		void ImGuiRender(bool shouldShow = true) override;

	private:
		Mesh*			   lightSphereMesh;
		Shader*			   shader;
		Renderer*		   renderer;
		Light*			   light[MAX_LIGHTS];
		bool			   moveLights = false;
		Vector4			   lightsOffsetVector[MAX_LIGHTS];
		Vector4			   lightColours[MAX_LIGHTS];
		Vector4			   lightBasePositions[MAX_LIGHTS];
		const Vector4      defaultLightColour	 = Vector4(0.96f, 0.93f, 0.83f, 1.0f);
		const float		   lightFactor			 = 300.0f;
		float			   specularityPower		 = 60.0f;

		//unsigned int	   lightsUBO;
		UniformBufferObject* lightUBO;

		void InstantiateLightPositionsAndColours();
		void ToggleLight(int index);
		void DrawIMGuiLightOffsetSlider(int lightIndex, std::string label);
		void ResetLights();
		void SetLightColour();
		void DrawLightColourSlider();
		void DrawLightAttenuationSlider();
};