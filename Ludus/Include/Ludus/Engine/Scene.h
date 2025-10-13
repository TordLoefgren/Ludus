#pragma once

#include <Ludus/Engine/ColliderRegistry.h>
#include <Ludus/Engine/GameObjectRegistry.h>
#include <Ludus/Engine/TransformRegistry.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Engine
{
	struct Scene
	{
	private:
		Ludus::Engine::GameObjectRegistry m_GameObjects;

	public:
		Ludus::Engine::ColliderRegistry Colliders;
		Ludus::Engine::TransformRegistry Transforms;

		GameObjectHandle AddGameObject()
		{
			return m_GameObjects.CreateGameObject();
		}

		bool DestroyGameObject(GameObjectHandle handle)
		{
			Colliders.RemoveByOwner(handle);
			Transforms.RemoveByOwner(handle);
			return m_GameObjects.DestroyGameObject(handle);
		}

		void AttachCollider(
			GameObjectHandle handle,
			Ludus::Physics::Index layer,
			LayerMask collidesWith = LayerMask::GetEmpty(),
			bool isStatic = false
		)
		{
			Colliders.Add(handle, layer, collidesWith, isStatic);
		}

		void AttachTransform(
			GameObjectHandle handle,
			Ludus::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(handle, position, scale, rotation);
		}

		const size_t GetGameObjectCount() { return m_GameObjects.GetCount(); }
	};
}
