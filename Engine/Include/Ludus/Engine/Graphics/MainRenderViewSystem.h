#pragma once

#include <Ludus/Engine/Core/RenderViewRequestRegistry.h>
#include <Ludus/Engine/Graphics/RenderViewRequest2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/ScenePresentation.h>

namespace Ludus::Engine::Graphics
{
	class MainRenderViewSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Engine::Core::RenderViewRequestRegistry& m_RenderViewRequestRegistry;
		Ludus::Engine::Runtime::ScenePresentation& m_ScenePresentation;

	public:
		MainRenderViewSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequestRegistry,
			Ludus::Engine::Runtime::ScenePresentation& scenePresentation
		);

		~MainRenderViewSystem() = default;

	protected:
		virtual void UpdateImpl(float) override;
	};
}
