#pragma once
#include <BlackBox/3DEngine/IStatObj.hpp>
#include <BlackBox/Input/IHardwareMouse.hpp>
#include <BlackBox/Renderer/Camera.hpp>
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/System/ConsoleRegistration.h>

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
		for (int i = 0; i < m_Camera.size(); i++)
		{
			delete m_Camera[i];
		}
	}
	CCameraController() = default;

	CCameraController(CCamera* pCamera)
	    : m_Camera{pCamera}
	{
		InitCVars();
		Env::System()->GetIHardwareMouse()->AddListener(this);
	}

	std::vector<CCamera*> m_Camera;
	size_t                m_CurrentCamera = 0;
	size_t                m_RenderCamera  = 0;
	//CCamera::Mode mode = CCamera::Mode::FPS;

	// Inherited via IInputEventListener
	virtual bool          OnInputEvent(const SInputEvent& event) override;
	bool                  OnKeyPress(EKeyId key);
	bool                  OnKeyReleas(EKeyId key);
	void                  InitCVars();
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Movement direction, float deltaTime, float value = 1.0f);
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void                  ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

	void                  update(float deltatime);

	CCamera*              CurrentCamera();

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
