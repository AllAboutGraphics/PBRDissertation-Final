#pragma once

#include "Vector3.h"
#include "Vector4.h"

struct LightData
{
	Vector4 lightSwitchAndRadius;		//On/Off->X and radius -> Y
	Vector4 position;
	Vector4	colour;
};

class Light
{
	public:
		Light() {}
		Light(const Vector4& position, const Vector4& colour, const float& radius, const float& isLightOn)
		{
			lightData.lightSwitchAndRadius.x = isLightOn;
			lightData.position				 = position;
			lightData.colour				 = colour;
			lightData.lightSwitchAndRadius.y = radius;
		}
		~Light() {}

		LightData& GetLightData()				  { return lightData; }

		Vector4& GetPosition()					  { return lightData.position; }
		void SetPosition(const Vector4& pos)	  { lightData.position = pos; }

		float& GetRadius()						  { return lightData.lightSwitchAndRadius.y; }
		void SetRadius(const float& rad)		  { lightData.lightSwitchAndRadius.y = rad; }
		float& GetRadiusReference()				  { return lightData.lightSwitchAndRadius.y; }

		Vector4& GetColour()					  { return lightData.colour; }
		void SetColour(const Vector4& col)		  { lightData.colour = col; }

		bool GetIsLightOn()						  { return lightData.lightSwitchAndRadius.x != 0.0f; }
		void SetIsLightOn(const float& islightOn) { lightData.lightSwitchAndRadius.x = islightOn; }

		Vector4& GetLightSwitchAndRadius()		  { return lightData.lightSwitchAndRadius; }

	protected:
		LightData lightData;
};