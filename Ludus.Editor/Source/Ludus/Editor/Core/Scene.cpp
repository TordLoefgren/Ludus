#include "Ludus/Editor/Core/Scene.h"

namespace Ludus::Editor::Core
{
	void Scene::OnAttachImpl()
	{
		// Inspector panel test.
		auto [currentWidth, currentHeight] = m_SystemContext->Window.GetFramebufferSize();

		Ludus::Engine::LayerMask::AddLayer("First Layer", 1);
		Ludus::Engine::LayerMask::AddLayer("Second Layer", 2);

		auto& ecs = m_SystemContext->EntityComponentSystem;

		auto handle = ecs.AddEntity();
		ecs.AttachCollider(
			handle,
			1,
			Ludus::Engine::LayerMask::FromIndex(2),
			true
		);
		ecs.AttachRigidBody(handle, { 0.0f }, Ludus::Physics::Core::BodyType::Static);
		ecs.AttachSprite(handle, Ludus::Graphics::Shape::Rect, Ludus::Graphics::Colors::Orange);
		ecs.AttachTransform(handle, { currentWidth * 0.5f, currentHeight * 0.5f }, 300.0f);
		ecs.AttachText(handle, "");
	}

	void Scene::UpdateImpl(float deltaTime)
	{ }
}
