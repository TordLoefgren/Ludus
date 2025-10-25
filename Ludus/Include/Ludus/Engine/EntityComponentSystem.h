#pragma once

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/EntityRegistry.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Engine
{
	struct EntityComponentSystem
	{
	private:
		Ludus::Engine::EntityRegistry m_Entities;

	public:
		Ludus::Engine::ColliderRegistry Colliders;
		Ludus::Engine::TransformRegistry Transforms;

		EntityHandle AddEntity()
		{
			return m_Entities.CreateEntity();
		}

		bool DestroyEntity(EntityHandle handle)
		{
			Colliders.RemoveByOwner(handle);
			Transforms.RemoveByOwner(handle);
			return m_Entities.DestroyEntity(handle);
		}

		void AttachCollider(
			EntityHandle handle,
			Ludus::Physics::Index layer,
			LayerMask collidesWith = LayerMask::GetEmpty(),
			bool isStatic = false
		)
		{
			Colliders.Add(handle, layer, collidesWith, isStatic);
		}

		void AttachTransform(
			EntityHandle handle,
			Ludus::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(handle, position, scale, rotation);
		}

		const size_t GetEntityCount() { return m_Entities.GetCount(); }
	};
}
