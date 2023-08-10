#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
	public:
		Camera() : yaw(0.0f), pitch(0.0f) {}
		
		Camera(float pitch, float yaw, Vector3 position)
		{
			this->yaw	   = yaw;
			this->pitch	   = pitch;
			this->position = position;
		}

		~Camera() {}

		void UpdateCamera(float dt = 1.0f, bool isGridFocus = false, float currentSpeed = 10.0f);

		Matrix4 BuildViewMatrix();

		Vector3 GetPosition() const   { return position; }
		void SetPosition(Vector3 pos) { position = pos; }

		float GetYaw() const		  { return yaw; }
		void SetYaw(float y)		  { yaw = y; }

		float GetPitch() const		  { return pitch; }
		void SetPitch(float p)		  { pitch = p; }

		bool GetCaptureMouseEvents() const { return captureMouseEvents; }
		void SetCaptureMouseEvents(bool shouldCaptureMouseEvents) { captureMouseEvents = shouldCaptureMouseEvents; }
		void ToggleCaptureMouseEvents() { captureMouseEvents = !captureMouseEvents; }

		void SetFocusOnObject(bool isGridFocus = false, bool shouldCaptureMouseEvents = false);

	protected:
		float	yaw;
		float	pitch;
		Vector3 position;
		bool	captureMouseEvents = false;

	private:
		const Vector3 gridFocusPos	  = Vector3(-5.0f, -5.0f, 28.0f);
		const Vector3 nonGridFocusPos = Vector3(-5.0f, -5.0f, 20.0f);

		void HandleMovements(Vector3 forward, Vector3 right, float speed, bool isGridFocus);
};