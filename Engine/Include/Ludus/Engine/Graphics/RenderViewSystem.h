#pragma once

#include <Ludus/Engine/Graphics/RenderViewConfiguration.h>
#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Engine::Core
{
	struct RenderViewRegistry;
	struct RenderViewRequestRegistry;
	struct SceneRegistry;
}

namespace Ludus::Engine::Runtime
{
	class IHostContext;
}

namespace Ludus::Engine::Graphics
{
	class RenderViewSystem final : public Ludus::Engine::Runtime::ISystem
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Engine::Graphics::RenderViewConfiguration m_RenderViewConfiguration;
		Ludus::Engine::Core::RenderViewRegistry& m_RenderViewRegistry;
		Ludus::Engine::Core::RenderViewRequestRegistry& m_RenderViewRequestRegistry;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;

	public:
		RenderViewSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Engine::Graphics::RenderViewConfiguration renderViewConfiguration,
			Ludus::Engine::Core::RenderViewRegistry& renderViewRegistry,
			Ludus::Engine::Core::RenderViewRequestRegistry& renderViewRequestRegistry,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry
		);

		~RenderViewSystem() = default;

	protected:
		virtual void BeginFrameImpl() override;
		virtual void RenderImpl() override;
	};
}
