#pragma once

#include <Ludus/Engine/Entity.h>
#include <Ludus/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Physics::Queries
{
	using EntityHandle = Ludus::Engine::EntityHandle;

	class IPhysicsQueryCache2D
	{
	public:
		virtual ~IPhysicsQueryCache2D() = default;

		virtual void UpdateFromContacts(const std::vector <Ludus::Physics::Narrowphase::ContactPair2D> contactPairs) = 0;

		virtual bool IsColliding(EntityHandle a) const = 0;
		virtual bool IsColliding(EntityHandle a, EntityHandle b) const = 0;

		virtual bool IsTriggering(EntityHandle a) const = 0;
		virtual bool IsTriggering(EntityHandle a, EntityHandle b) const = 0;

		virtual void Clear() = 0;
	};
}
