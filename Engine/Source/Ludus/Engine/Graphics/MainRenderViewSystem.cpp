#include "pch.h"

#include <Ludus/Engine/Graphics/MainRenderViewSystem.h>

namespace Ludus::Engine::Graphics
{
	MainRenderViewSystem::MainRenderViewSystem(
		Ludus::Engine::Runtime::IHostContext& hostContext,
		Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequestRegistry,
		Ludus::Engine::Runtime::ScenePresentationState& scenePresentationState
	) :
		m_HostContext(hostContext),
		m_RenderViewRequestRegistry(renderViewRequestRegistry),
		m_ScenePresentationState(scenePresentationState)
	{ }

	void MainRenderViewSystem::UpdateImpl(float)
	{
		auto& target = m_HostContext.GetMainRenderTarget();
		const auto [width, height] = target.Framebuffer.GetSize();

		Ludus::Engine::Graphics::RenderViewRequest2D renderViewRequest {
			.Camera = std::nullopt,
			.SceneHandle = m_ScenePresentationState.CurrentSceneHandle,
			.Target = &target,
			.ViewportRect = Ludus::Engine::Math::Rect::Create(
				{ 0.0f, 0.0f },
				{ static_cast<float>(width), static_cast<float>(height) }
			)
		};

		m_RenderViewRequestRegistry.Register(renderViewRequest);
	}
}
