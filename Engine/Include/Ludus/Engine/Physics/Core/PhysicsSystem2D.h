#pragma once

#include <Ludus/Engine/Physics/Core/PhysicsConfiguration2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsPipeline2D.h>
#include <Ludus/Engine/Physics/Core/PhysicsWorld2D.h>
#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Engine::Core
{
	struct EntityComponentSystem;
	struct SceneRegistry;
}

namespace Ludus::Engine::Physics::Queries
{
	class IPhysicsQueryCache2D;
}

namespace Ludus::Engine::Physics::Core
{
	class PhysicsSystem2D final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		PhysicsWorld2D m_PhysicsWorld;
		PhysicsPipeline2D m_PhysicsPipeline;
		int m_SubSteps;

		Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D* m_Queries = nullptr;

	public:
		PhysicsSystem2D(
			PhysicsConfiguration2D& physicsConfiguration,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry
		);

		void PullEntityComponents(Ludus::Engine::Core::EntityComponentSystem& entityComponentSystem);

		virtual void FixedUpdateImpl(float fixedTime) override;
	};
}
