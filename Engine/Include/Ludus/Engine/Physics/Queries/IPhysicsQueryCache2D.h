#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Engine::Physics::Queries
{
	using EntityHandle = Ludus::Engine::Core::EntityHandle;

	class IPhysicsQueryCache2D
	{
	public:
		virtual ~IPhysicsQueryCache2D() = default;

		virtual void UpdateFromContacts(const std::vector <Ludus::Engine::Physics::Narrowphase::ContactPair2D> contactPairs) = 0;
		virtual std::span<const Ludus::Engine::Physics::Narrowphase::ContactPair2D> GetContacts(EntityHandle handle) const = 0;

		virtual bool IsColliding(EntityHandle a) const = 0;
		virtual bool IsColliding(EntityHandle a, EntityHandle b) const = 0;

		virtual bool IsTriggering(EntityHandle a) const = 0;
		virtual bool IsTriggering(EntityHandle a, EntityHandle b) const = 0;

		virtual void Clear() = 0;
	};
}
