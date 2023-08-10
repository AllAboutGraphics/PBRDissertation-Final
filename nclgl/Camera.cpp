#include "Camera.h"
#include "Window.h"
#include<algorithm>

void Camera::UpdateCamera(float dt, bool isGridFocus, float currentSpeed)
{
    if (captureMouseEvents)
    {
        pitch -= Window::GetMouse()->GetRelativePosition().y;
        yaw -= Window::GetMouse()->GetRelativePosition().x;
    }

    pitch = std::min(pitch, 90.0f);
    pitch = std::max(-90.0f, pitch);

    if (yaw < 0.0f)   { yaw += 360.0f; }
    if (yaw > 360.0f) { yaw -= 360.0f; }

    // Calculating the ROTATION made on mouse movement
    Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
    // Calculating FORWARD direction (-ve Z axis) vector based on the Current ROTATION
    Vector3 forward  = rotation * Vector3(0, 0, -1);
    // Calculating RIGHT direction (+ve X axis) vector based on the current ROTATION
    Vector3 right    = rotation * Vector3(1, 0, 0);
    // Fixing Movement units per Frame. Setting it to 30 units per second
    float speed      = currentSpeed * dt;  // dt is the time since last frame drawn.

    HandleMovements(forward, right, speed, isGridFocus);
}

Matrix4 Camera::BuildViewMatrix()
{
    return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
           Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
           Matrix4::Translation(-position);
}

void Camera::SetFocusOnObject(bool isGridFocus, bool shouldCaptureMouseEvents)
{
    captureMouseEvents = shouldCaptureMouseEvents;
    yaw                = 0.0f;
    pitch              = 0.0f;
    position           = isGridFocus ? gridFocusPos : nonGridFocusPos;
}

void Camera::HandleMovements(Vector3 forward, Vector3 right, float speed, bool isGridFocus)
{
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))     { position   += (forward * speed); }  //Forward
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))     { position   -= (forward * speed); }  //Backward
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))     { position   -= (right   * speed); }  //Left
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))     { position   += (right   * speed); }  //Right
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) { position.y += speed;             }  //Up
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) { position.y -= speed;             }  //Down
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_F))     { SetFocusOnObject(isGridFocus);   }  //Focus
}