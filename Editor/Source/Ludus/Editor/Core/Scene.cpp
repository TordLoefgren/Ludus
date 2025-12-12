#include "pch.h"

#include <Ludus/Editor/Core/Scene.h>

namespace Ludus::Editor::Core
{
	void Scene::OnAttachImpl()
	{
		// Inspector panel test.
		Ludus::Engine::Physics::Core::LayerMask::AddLayer("First Layer", 1);
		Ludus::Engine::Physics::Core::LayerMask::AddLayer("Second Layer", 2);

		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(
			handle,
			0,
			Ludus::Engine::Physics::Core::LayerMask::FromIndex(1)
		);
		ecs.AttachRigidBody(handle, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachSprite(handle, Ludus::Engine::Graphics::Shape::Rect, Ludus::Engine::Graphics::Colors::Orange);
		ecs.AttachTransform(handle, { 0.0f, 4.f }, 1.0f);
		ecs.AttachText(handle, "");

		auto handle2 = ecs.AddEntity();
		ecs.AttachCollider(
			handle2,
			1,
			Ludus::Engine::Physics::Core::LayerMask::FromIndex(0),
			false
		);
		ecs.AttachRigidBody(handle2, { 0.0f }, Ludus::Engine::Physics::Core::BodyType::Static);
		ecs.AttachSprite(handle2, Ludus::Engine::Graphics::Shape::Rect, Ludus::Engine::Graphics::Colors::Blue);
		ecs.AttachTransform(handle2, { 0.0f, -4.0f }, { 6.0f, 1.0f });
		ecs.AttachText(handle2, "");

		auto handle3 = ecs.AddEntity();
		ecs.AttachCamera(handle3, 10.0f);
		ecs.AttachTransform(handle3, { 0.0f, 0.0f }, { 1.0f, 1.0f });
	}

	void Scene::UpdateImpl(float deltaTime)
	{ }
}
