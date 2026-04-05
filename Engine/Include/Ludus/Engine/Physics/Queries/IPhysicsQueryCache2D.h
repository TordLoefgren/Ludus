#pragma once

#include <span>
#include <vector>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPair2D.h>

namespace Ludus::Engine::Physics::Queries
{
	using EntityId = Ludus::Engine::Core::EntityId;

	class IPhysicsQueryCache2D
	{
	public:
		virtual ~IPhysicsQueryCache2D() = default;

		virtual void UpdateFromContacts(const std::vector <Ludus::Engine::Physics::Narrowphase::ContactPair2D> contactPairs) = 0;
		virtual std::span<const Ludus::Engine::Physics::Narrowphase::ContactPair2D> GetContacts(EntityId id) const = 0;

		virtual bool IsColliding(EntityId a) const = 0;
		virtual bool IsColliding(EntityId a, EntityId b) const = 0;

		virtual bool IsTriggering(EntityId a) const = 0;
		virtual bool IsTriggering(EntityId a, EntityId b) const = 0;

		virtual void Clear() = 0;
	};
}
