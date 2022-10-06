#pragma once
#include <BlackBox/3DEngine/IStatObj.hpp>
#include <BlackBox/Input/IHardwareMouse.hpp>
#include <BlackBox/Renderer/Camera.hpp>
#include <BlackBox/Renderer/IRender.hpp>

enum class Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
class CCameraController : public IInputEventListener, public IHardwareMouseEventListener
{
public:
public:
	~CCameraController()
	{
		RemoveCameras();
	}
	void RemoveCameras()
	{
		for (int i = 0; i < m_Camera.size(); i++)
		{
			delete m_Camera[i];
		}
	}
	CCameraController() = default;
	CCameraController(CCamera* pCamera)
	    : m_Camera{pCamera}
	{
		Env::System()->GetIHardwareMouse()->AddListener(this);
	}

	std::vector<CCamera*> m_Camera;
	size_t                m_CurrentCamera = 0;
	size_t                m_RenderCamera  = 0;
	//CCamera::Mode mode = CCamera::Mode::FPS;

	// Inherited via IInputEventListener
	virtual bool          OnInputEvent(const SInputEvent& event) override
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
	bool OnKeyPress(EKeyId key)
	{
		m_keys.insert(key);
		return true;
	}
	bool OnKeyReleas(EKeyId key)
	{
		m_keys.erase(key);
		return false;
	}
	void InitCVars()
	{
#if 0
    REGISTER_CVAR2("cam_speed", &m_Camera->MovementSpeed, 5.0f, 0, "Speed of camera");
    REGISTER_CVAR2("fov", &m_Camera->m_fov, 45.0f, 0, "Camera field of view");
    REGISTER_CVAR2("zfar", &m_Camera->zFar, 10000.f, 0, "Draw distance");
#endif
	}
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Movement direction, float deltaTime, float value = 1.0f);
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		//CurrentCamera()->updateCameraVectors();
		//if (IsFreezed) return;
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

	CCamera* CurrentCamera()
	{
		return m_Camera[m_CurrentCamera];
	}

	CCamera* RenderCamera()
	{
		return m_Camera[m_RenderCamera];
	}

	void AddCamera(CCamera* pCamera)
	{
		m_Camera.push_back(pCamera);
	}

	void SetRenderCamera(size_t n)
	{
		assert(n < m_Camera.size());
		m_RenderCamera = n;
		Env::Renderer()->SetCamera(*m_Camera[n]);
	}
	void SetMovementCamera(size_t n)
	{
		assert(n < m_Camera.size());
		m_CurrentCamera = n;
	}

public:
	Legacy::Vec3     velocity = Legacy::Vec3(10);
	std::set<EKeyId> m_keys;
	Legacy::Vec2     delta;
	float            MovementSpeed;
	const float      SCROLL_SPEED    = 2.0f;
	const float      MOUSE_SPEED     = 1.5f;
	const float      MOUSE_SENSIVITY = 0.05f;
	bool             IsFreezed       = false;

	void             Freeze(bool val) { IsFreezed = val; }

	// Inherited via IHardwareMouseEventListener

	void             OnHardwareMouseEvent(int iX, int iY, EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0)
	{
		if (eHardwareMouseEvent == HARDWAREMOUSEEVENT_WHEEL)
		{
			if (wheelDelta > 0)
			{
				ProcessKeyboard(Movement::FORWARD, 0.5);
			}
			else
			{
				ProcessKeyboard(Movement::BACKWARD, 0.5);
			}
		}
	}
};
