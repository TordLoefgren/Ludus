#include "pch.h"

#include <Ludus/Editor/Core/Scene.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>

namespace Ludus::Editor::Core
{
	void Scene::OnAttachImpl()
	{
		auto& ecs = m_SystemContext->EntityComponentSystem;

		// Inspector panel test.
		Ludus::Engine::Physics::Core::LayerMask::AddLayer("First Layer", 1);
		Ludus::Engine::Physics::Core::LayerMask::AddLayer("Second Layer", 2);

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(
			handle,
			0,
			Ludus::Engine::Physics::Core::LayerMask::FromIndex(1)
		);
		ecs.AttachRigidBody(handle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachSprite(handle, Ludus::Engine::Graphics::Shape::Rect, Ludus::Engine::Graphics::Colors::White);
		ecs.AttachTransform(handle, { 0.0f, 5.f }, 1.0f);
		ecs.AttachText(handle, "");

		auto handle2 = ecs.AddEntity();
		ecs.AttachCollider(
			handle2,
			1,
			Ludus::Engine::Physics::Core::LayerMask::FromIndex(0),
			false
		);
		ecs.AttachRigidBody(handle2, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachSprite(handle2, Ludus::Engine::Graphics::Shape::Rect, Ludus::Engine::Graphics::Colors::White);
		ecs.AttachTransform(handle2, { 0.0f, -5.0f }, { 6.0f, 1.0f });
		ecs.AttachText(handle2, "");
	}

	void Scene::UpdateImpl(float deltaTime)
	{ }
}
