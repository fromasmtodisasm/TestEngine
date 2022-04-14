#include "CameraController.hpp"
#include "Minecraft.h"
#include <cmath>
#include <functional>

float CCameraController::MovementSpeed;

bool  CCameraController::OnInputEvent(const SInputEvent& event)
{
	if (Env::Console()->IsOpened())
		return false;
	bool mousePressed = event.deviceType == eIDT_Mouse && event.state == eIS_Pressed;
	bool rotated      = false;
	//if (event.pSymbol != nullptr)
	rotated           = event.keyId == eKI_MouseX || event.keyId == eKI_MouseY; // || event.pSymbol->type == SInputSymbol::EType::Axis;

	////////////////////////
	bool keyPressed   = event.deviceType == eIDT_Keyboard && event.state == eIS_Pressed;
	bool keyReleased  = event.deviceType == eIDT_Keyboard && event.state == eIS_Released;
	// #unreferenced
	//bool control	 = event.modifiers & eMM_Ctrl;
	// #unreferenced
	//bool shift		 = event.modifiers & eMM_Shift;
	// #unreferenced
	//bool alt		 = event.modifiers & eMM_Alt;
	////////////////////////
	if (rotated)
	{
		// TODO: GET DELTA  MOUSE
		delta = Legacy::Vec2(0);
		if (event.keyId == eKI_MouseX || event.keyId == eKI_XI_ThumbRX || event.keyId == eKI_XI_ThumbLX)
			delta.x = event.value;
		else
			delta.y = event.value;
		//ProcessMouseMovement(static_cast<float>(delta.x), -static_cast<float>(delta.y));
		return false;
	}
	else if (keyPressed)
	{
		return OnKeyPress(event.keyId);
	}
	else if (keyReleased)
		return OnKeyReleas(event.keyId);
	return false;
}

bool CCameraController::OnKeyPress(EKeyId key)
{
	m_keys.insert(key);
	return true;
}

bool CCameraController::OnKeyReleas(EKeyId key)
{
	m_keys.erase(key);
	return false;
}

bool CCameraController::InitCVars()
{
	REGISTER_CVAR2("camera_speed", &MovementSpeed, 5.0f, 0, "Speed of camera");
#if 0
    REGISTER_CVAR2("fov", &m_Camera->m_fov, 45.0f, 0, "Camera field of view");
    REGISTER_CVAR2("zfar", &m_Camera->zFar, 10000.f, 0, "Draw distance");
#endif
	return true;
}

void CCameraController::Move(Movement direction, float deltaTime, float value)
{
	ProcessKeyboard(direction, deltaTime, value);
}

void CCameraController::ProcessKeyboard(Movement direction, float deltaTime, float value)
{
	float velocity        = deltaTime * value;
	CurrentCamera()->mode = CCamera::Mode::FLY;

	auto moveForward      = glm::vec3(
	         CurrentCamera()->Front.x,
        CurrentCamera()->mode == CCamera::Mode::FPS ? 0 : CurrentCamera()->Front.y,
	         CurrentCamera()->Front.z);

	if (direction == Movement::FORWARD)
	{
		m_MoveDirection = moveForward;
	}
	if (direction == Movement::BACKWARD)
	{
		m_MoveDirection = -moveForward;
	}
	if (direction == Movement::LEFT)
	{
		m_MoveDirection = CurrentCamera()->Right;
	}
	if (direction == Movement::RIGHT)
	{
		m_MoveDirection = -CurrentCamera()->Right;
	}
	//m_MoveDirection = glm::normalize(m_MoveDirection);
	//m_MoveDirection *= velocity;
	minecraft->player.move(m_MoveDirection, MovementSpeed * velocity);
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.

void CCameraController::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	CurrentCamera()->transform.rotation.y -= xoffset;
	CurrentCamera()->transform.rotation.x += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		auto& x = CurrentCamera()->transform.rotation.x;
		x       = glm::clamp(x, -89.f, 89.f);
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	CurrentCamera()->updateCameraVectors();
}

void CCameraController::Update(float deltatime)
{
	if (Env::Get()->IsDedicated())
	{
		return;
	}

	//minecraft->player.move(MovementSpeed * m_MoveDirection, 1);
	m_MoveDirection = glm::vec3(0);
}

CCamera* CCameraController::CurrentCamera()
{
	return m_Camera[m_CurrentCamera];
}
