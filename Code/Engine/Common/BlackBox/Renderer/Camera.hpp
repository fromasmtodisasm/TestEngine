#pragma once

// Std. Includes
#include <iostream>
#include <vector>

#include <BlackBox/Math/MathHelper.hpp>
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/System/IConsole.hpp>
#include <BlackBox/System/ISystem.hpp>

//#include "Cry_Geo.h"

struct ICVar;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

// Default camera values
//const float YAW		   = -90.0f;
const float PITCH      = 0.0f;
const float SPEED      = 0.1f;
const float SENSITIVTY = 0.25f;
const float ZOOM       = 45.0f;

namespace Detail
{
	template<typename GenType>
	GenType GetNormalized(const GenType& v)
	{
		return glm::normalize(v)
	}

#if 0
	struct AABB
	{
		Vec3 min;
		Vec3 max;
	};

	struct Plane
	{
		//plane-equation: n.x*x+n.y*y+n.z*z+d>0 is in front of the plane
		Legacy::Vec3 n; //!< normal
		f32          d; //!< distance

		Plane(){};

		ILINE Plane(const Plane& p)
		{
			n = p.n;
			d = p.d;
		}
		ILINE Plane(const Legacy::Vec3& normal, const f32& distance)
		{
			n = normal;
			d = distance;
		}

		//! set normal and dist for this plane and  then calculate plane type
		ILINE void Set(const Legacy::Vec3& vNormal, const f32 fDist)
		{
			n = vNormal;
			d = fDist;
		}

		ILINE void SetPlane(const Legacy::Vec3& normal, const Legacy::Vec3& point)
		{
			n = normal;
			d = -(glm::dot(point, normal));
		}

		ILINE friend Plane GetPlane(const Legacy::Vec3& normal, const Legacy::Vec3& point)
		{
			return Plane(normal, -glm::dot(point, normal));
		}

		/*!
	*
	* Constructs the plane by tree given Legacy::Vec3s (=triangle) 
	*
	* Example 1:
	* \code
	*  Legacy::Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
	*  Plane  plane;
	*  plane.CalculatePlane(v0,v1,v2);
	* \endcode
	*
	* Example 2:
	* \code
	*  Legacy::Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
	*  Plane  plane=CalculatePlane(v0,v1,v2);
	* \endcode
	*
	*/
		ILINE void SetPlane(const Legacy::Vec3& v0, const Legacy::Vec3& v1, const Legacy::Vec3& v2)
		{
			n = GetNormalized(glm::cross((v1 - v0) , (v0 - v2))); //vector cross-product
			d = -glm::dot(n , v0);                            //calculate d-value
		}
		ILINE friend Plane GetPlane(const Legacy::Vec3& v0, const Legacy::Vec3& v1, const Legacy::Vec3& v2)
		{
			Plane p;
			p.n = GetNormalized(glm::cross((v1 - v0) , (v0 - v2))); //vector cross-product
			p.d = -glm::dot(p.n , v0);                          //calculate d-value
			return p;
		}

		/*!
	*
	* Computes signed distance from point to plane.
	* This is the standart plane-equation: d=Ax*By*Cz+D.
	* The normal-vector is assumed to be normalized.
	* 
	* Example:
	*  Legacy::Vec3 v(1,2,3);
	*  Plane  plane=CalculatePlane(v0,v1,v2);
	*  f32 distance = plane|v;
	*
	*/
		ILINE f32         operator|(const Legacy::Vec3& point) const { return glm::dot(n , point) + d; }

		//! check for equality between two planes
		ILINE friend bool operator==(const Plane& p1, const Plane& p2)
		{
			if (fabsf(p1.n.x - p2.n.x) > 0.0001f) return (false);
			if (fabsf(p1.n.y - p2.n.y) > 0.0001f) return (false);
			if (fabsf(p1.n.z - p2.n.z) > 0.0001f) return (false);
			if (fabsf(p1.d - p2.d) < 0.01f) return (true);
			return (false);
		}
	};
#endif
} // namespace Detail

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class CCamera
{
public:
	//////////////////////////////////////////////////////////////////////
	enum
	{
		FR_PLANE_NEAR,
		FR_PLANE_FAR,
		FR_PLANE_RIGHT,
		FR_PLANE_LEFT,
		FR_PLANE_TOP,
		FR_PLANE_BOTTOM,
		FRUSTUM_PLANES
	};
	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};

	enum class Mode
	{
		FPS,
		FLY
	} mode = CCamera::Mode::FLY;
	enum class Type
	{
		Perspective,
		Ortho
	} type = Type::Perspective;

public:
	// Camera Attributes
	Transform    transform;
	Legacy::Vec3 Front;
	Legacy::Vec3 Up;
	Legacy::Vec3 Right;
	Legacy::Vec3 WorldUp;
	// Camera options
	float        MovementSpeed;
	float        Zoom;
	float        m_fov = 45.0f;
	float        Ratio = 16.0f / 9;
	float        zNear = 0.1f;
	float        zFar  = 10000.f;

	Plane        m_frustum[FRUSTUM_PLANES];

	// Constructor with vectors
	CCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.f, float pitch = PITCH)
	    : Front(glm::vec3(0.0f, 0.0f, 0.0f))
	    , MovementSpeed(10.f)
	    , Zoom(ZOOM)
	{
		this->transform.position   = position;
		this->WorldUp              = up;
		this->transform.rotation.y = yaw;
		this->transform.rotation.x = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	    : Front(glm::vec3(0.0f, 0.0f, 0.0f))
	    , MovementSpeed(10.f)
	    , Zoom(ZOOM)
	{
		this->transform.position   = glm::vec3(posX, posY, posZ);
		this->WorldUp              = glm::vec3(upX, upY, upZ);
		this->transform.rotation.y = yaw;
		this->transform.rotation.x = pitch;
		this->updateCameraVectors();
	}

	void Update()
	{
		updateCameraVectors();
	}

	void Init(int width, int height, float fov = 45.f)
	{
	}
	void SetAngle(Legacy::Vec3 ang)
	{
	}
	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	Legacy::Mat4 GetViewMatrix() const
	{
		//return glm::lookAt(this->transform.position, this->transform.position + this->Front, this->Up);
		return glm::lookAtLH(this->transform.position, this->transform.position + this->Front, this->Up);
	}
	Legacy::Mat4 GetProjectionMatrix() const
	{
		Legacy::Vec4d v;
		Env::Renderer()->GetViewport(&v.x, &v.y, &v.z, &v.w);
		if (type == Type::Perspective)
		{
#ifdef USE_REVERSED_Z_BUFFER
			auto MakeInfReversedZProjLH = [](float fovY_radians, float aspectWbyH, float zNear)->glm::mat4
			{
				float f = 1.0f / tan(fovY_radians / 2.0f);
				return glm::mat4(
				    f / aspectWbyH, 0.0f, 0.0f, 0.0f,
				    0.0f, f, 0.0f, 0.0f,
				    0.0f, 0.0f, 0.0f, +1.0f,
				    0.0f, 0.0f, zNear, 0.0f);
			};
			return MakeInfReversedZProjLH(glm::radians(m_fov), float(v.z) / float(v.w), zNear);
#else
			//return glm::perspective(glm::radians(m_fov), (float)(v.z) / (float)(v.w), zNear, zFar);
			return glm::perspectiveFovLH_ZO(glm::radians(m_fov), (float)v.z, (float)v.w, zNear, zFar);
#endif
		}
		else
		{
			return glm::ortho(0.f, 160.f, 0.f, 90.f, 0.1f, 500.f);
		}
	}

	Legacy::Vec3 GetPos() const
	{
		return transform.position;
	}
	Legacy::Vec3 GetAngles() const
	{
		return transform.rotation;
	}

	void SetPos(glm::vec3 pos)
	{
		transform.position = pos;
	}

	void SetAngles(glm::vec3 ang)
	{
		transform.rotation = ang;
	}

	inline void  SetFov(float fov) { m_fov = fov; }
	inline float GetFov() const { return (m_fov); }

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void         ProcessMouseScroll(float yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x     = cos(glm::radians(this->transform.rotation.y)) * cos(glm::radians(this->transform.rotation.x));
		front.y     = sin(glm::radians(this->transform.rotation.x));
		front.z     = sin(glm::radians(this->transform.rotation.y)) * cos(glm::radians(this->transform.rotation.x));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
	}
	Frustum createFrustumFromCamera(const CCamera& cam, float aspect, float fovY,
	                                float zNear, float zFar)
	{
		//struct ToPlane
		//{
		//	Plane m_Plane;
		//	ToPlane(Legacy::Vec3, Legacy::Vec3)
		//	    : m_Plane()
		//	{
		//	}
		//	Plane& operator Plane()
		//	{
		//		return m_Plane;
		//	}
		//};

		Frustum frustum;
#if 0
		const float     halfVSide    = zFar * tanf(fovY * .5f);
		const float     halfHSide    = halfVSide * aspect;
		const glm::vec3 frontMultFar = zFar * cam.Front;

		frustum.nearFace             = {cam.GetPos() + zNear * cam.Front, cam.Front};
		frustum.farFace              = {cam.GetPos() + frontMultFar, -cam.Front};
		frustum.rightFace            = {cam.GetPos(),
                             glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide)};
		frustum.leftFace             = {cam.GetPos(),
                            glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up)};
		frustum.topFace              = {cam.GetPos(),
                           glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide)};
		frustum.bottomFace           = {cam.GetPos(),
                              glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right)};

#endif
		return frustum;
	}
	//AABB-frustum test
	bool IsAABBVisibleFast(const AABB& aabb) const;
};

//////////////////////////////////////////////////////////////////////
/*!
* Very fast approach to check if an AABB and the camera-frustum overlap, or if the AABB 
* is totally inside the camera-frustum. The bounding-box of the AABB is assumed to be 
* in world-space. This test can reject even such AABBs that overlap a frustum-plane far 
* outside the view-frustum. 
* IMPORTANT: this function is only usefull if you really need hierachical-culling. 
* It is about 30% slower then "IsAABBVisibleFast(aabb)"   
*
* Example:
*  int InOut=camera.IsAABBVisible_hierarchical(aabb);
*
* return values:
*  CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
*  CULL_OVERLAP     = AABB intersects the borders of the frustum, further checks necessary
*/
//////////////////////////////////////////////////////////////////////
#if 0
__forceinline bool CCamera::IsAABBVisibleFast(const AABB& aabb) const
{
	float       d;
	const Vec3* pAABB = &aabb.min;
	for (int i = 0; i < FRUSTUM_PLANES; i++)
	{
		d = -m_frustum[i].d;
		if (m_frustum[i].n.x >= 0)
			d += m_frustum[i].n.x * aabb.min.x;
		else
			d += m_frustum[i].n.x * aabb.max.x;
		if (m_frustum[i].n.y >= 0)
			d += m_frustum[i].n.y * aabb.min.y;
		else
			d += m_frustum[i].n.y * aabb.max.y;
		if (m_frustum[i].n.z >= 0)
			d += m_frustum[i].n.z * aabb.min.z;
		else
			d += m_frustum[i].n.z * aabb.max.z;
		if (d > 0) return false;
	}
	return true;
}
#endif

class COrthoCamera : public CCamera
{
};
