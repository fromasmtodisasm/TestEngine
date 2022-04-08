#include "CameraController.hpp"
#include "Minecraft.h"


// Inherited via IInputEventListener

bool CCameraController::OnInputEvent(const SInputEvent& event)
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

void CCameraController::InitCVars()
{
	REGISTER_CVAR2("cs", &MovementSpeed, 5.0f, 0, "Speed of camera");
#if 0
    REGISTER_CVAR2("fov", &m_Camera->m_fov, 45.0f, 0, "Camera field of view");
    REGISTER_CVAR2("zfar", &m_Camera->zFar, 10000.f, 0, "Draw distance");
#endif
}

void CCameraController::ProcessKeyboard(Movement direction, float deltaTime, float value)
{
	float velocity      = 100 * deltaTime * value;

	auto  moveDirection = glm::vec3(
        CurrentCamera()->Front.x,
        /*CurrentCamera()->mode == CCamera::Mode::FPS ? 0 : */CurrentCamera()->Front.y,
        CurrentCamera()->Front.z);

	if (direction == Movement::FORWARD)
	{
		minecraft->player.move(moveDirection, velocity);
	}
	if (direction == Movement::BACKWARD)
	{
		minecraft->player.move(moveDirection, -velocity);
	}
	if (direction == Movement::LEFT)
	{
		minecraft->player.move(CurrentCamera()->Right, velocity);
	}
	if (direction == Movement::RIGHT)
	{
		minecraft->player.move(CurrentCamera()->Right, -velocity);
	}
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
		if (CurrentCamera()->transform.rotation.x > 89.0f)
			CurrentCamera()->transform.rotation.x = 89.0f;
		if (CurrentCamera()->transform.rotation.x < -89.0f)
			CurrentCamera()->transform.rotation.x = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	CurrentCamera()->updateCameraVectors();
}

void CCameraController::update(float deltatime)
{
	if (Env::Get()->IsDedicated())
	{
		return;
	}

	//ImGui
	// #unreferenced
	//float mult			 = m_keys.find(eKI_LShift) != m_keys.end() ? 3.f : 1.f;
	// #unreferenced
	//float rotation_speed = 15.f * deltatime * mult;
	//float rotSpeed = deltatime * 5.f;//m_rotAngle;
	static Legacy::Vec3 impulse = Legacy::Vec3(0.f, 10.f, 0.f);
	// #unreferenced
	//float move_speed	= deltatime * mult;
	for (auto& key : m_keys)
	{
		switch (key)
		{
		case eKI_Space:
			velocity += impulse;
			break;
		case eKI_W:
			//ProcessKeyboard(Movement::FORWARD, move_speed);
			break;
		case eKI_S:
			//ProcessKeyboard(Movement::BACKWARD, move_speed);
			break;
		case eKI_A:
			//ProcessKeyboard(Movement::LEFT, move_speed);
			break;
		case eKI_D:
			//ProcessKeyboard(Movement::RIGHT, move_speed);
			break;
		case eKI_Up:
			//ProcessMouseMovement(0.f, rotation_speed);
			break;
		case eKI_Down:
			//ProcessMouseMovement(0.f, -rotation_speed);
			break;
		case eKI_Left:
			//ProcessMouseMovement(-rotation_speed, 0.f);
			break;
		case eKI_Right:
			//ProcessMouseMovement(rotation_speed, 0.f);
			break;
		default:; //GameObject::update(deltatime);
		}
	}
#if 0
    GetISystem()->GetIScriptSystem()->BeginCall("Player", "Update");
    GetISystem()->GetIScriptSystem()->PushFuncParam(m_pScript);
    GetISystem()->GetIScriptSystem()->PushFuncParam(deltatime);
    GetISystem()->GetIScriptSystem()->EndCall();
#endif
}

CCamera* CCameraController::CurrentCamera()
{
	return m_Camera[m_CurrentCamera];
}
