#pragma once

#define MAX_SKYMAPS 2

#include "..\nclgl\Textures.h"
#include "Renderer.h"

class Skybox;

class PBRHelper
{
	public:
		PBRHelper(Mesh* quadRef, Skybox* skyboxRef, Renderer* rendererDelegate);
		~PBRHelper();

		Matrix4 GetCaptureProjection() const { return captureProjection; }
		Matrix4* GetCaptureViews()	   const { return captureViews;		 }
		void SetupFrameBuffer(/*unsigned int& captureFBO, unsigned int& captureRBO*/);
		void LoadHDRTextures();
		Textures* LoadHDREnvironmentMap(Shader* shader, std::string fileName);
		void CreateCubeMap(unsigned int& cubeMapID, int width = 512, int height = 512, bool shouldGenerateMipMaps = false);
		void CreateProjectionAndViewMatricesForCubemapFaceDirections();

		void InitPBR(int index);

		Textures* GetHdrTexture(int index)			   { return hdrTexture[index]; }
		unsigned int& GetCaptureFBO()				   { return captureFBO; }
		unsigned int& GetCaptureRBO()				   { return captureRBO; }
		unsigned int& GetEnvironmentCubeMap(int index) { return envCubemap[index]; }
		unsigned int& GetIrradianceMap(int index)	   { return irradianceMap[index]; }
		unsigned int& GetPrefilterMap(int index)	   { return prefilterMap[index]; }
		unsigned int& GetBrdfLUTTexture(int index)	   { return brdfLUTTexture[index]; }

	private:
		Renderer*	 renderer;
		AssetsCache* instance = NULL;
		Mesh*		 quad;
		Skybox*		 skybox;
		Matrix4		 captureProjection;
		Matrix4*	 captureViews;

		Textures* hdrTexture[MAX_SKYMAPS];

		unsigned int captureFBO;
		unsigned int captureRBO;
		unsigned int envCubemap[MAX_SKYMAPS];
					 
		unsigned int irradianceMap[MAX_SKYMAPS];
					 
		unsigned int prefilterMap[MAX_SKYMAPS];
		unsigned int brdfLUTTexture[MAX_SKYMAPS];

		void ConvertHDREquirectangularMapToCubeMap(int index);

		//Irradiance Cube Map for Diffuse calculation
		void CreateIrradianceCubeMap(int index);
		void SetUniformsInIrradianceShader(int index);
		//IBL Specular
		void PerformMonteCarloSimulationOnPrefilterMap(int index);
		void BindLUT(int index);
};