#pragma once

class CPhysicsDebugger final : public btIDebugDraw
{
public:
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	void reportErrorWarning(const char* warningString) override;
	void draw3dText(const btVector3& location, const char* textString) override;
	void setDebugMode(int debugMode) override;
	int  getDebugMode() const override;

public:
	DebugDrawModes m_Mode{DBG_DrawWireframe};
};

struct SPhysicsInitParams
{
	btDispatcher*             dispatcher;
	btBroadphaseInterface*    broadPhase;
	btConstraintSolver*       constraintSolver;
	btCollisionConfiguration* collisionConfiguration;

	void                      Create()
	{
		collisionConfiguration = DEBUG_NEW btDefaultCollisionConfiguration();
		dispatcher             = DEBUG_NEW             btCollisionDispatcher(collisionConfiguration);
		broadPhase             = DEBUG_NEW             btDbvtBroadphase();
		constraintSolver       = new btSequentialImpulseConstraintSolver;
	}
	~SPhysicsInitParams()
	{
		SAFE_DELETE(collisionConfiguration);
		SAFE_DELETE(dispatcher);
		SAFE_DELETE(broadPhase);
		SAFE_DELETE(constraintSolver);
	}
};