#pragma once

#include <Ludus/Engine/Graphics/Renderer2D.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Graphics/RenderingOptions.h>
#include <Ludus/Engine/Graphics/RenderingPipeline2D.h>
#include <Ludus/Engine/Graphics/Shader.h>
#include <Ludus/Engine/Runtime/ISystem.h>
#include <Ludus/Engine/Runtime/RuntimeEnvironment.h>

namespace Ludus::Engine::Core
{
	class AssetManager;
	struct RenderViewRegistry;
	struct SceneRegistry;
}

namespace Ludus::Engine::Graphics
{
	class RenderingSystem2D final : public Ludus::Engine::Runtime::ISystem
	{
		RenderingOptions m_RenderingOptions;
		RenderingConfiguration2D& m_RenderingConfiguration;
		Ludus::Engine::Core::AssetManager& m_AssetManager;
		Ludus::Engine::Core::RenderViewRegistry& m_RenderViewRegistry;
		Ludus::Engine::Core::SceneRegistry& m_SceneRegistry;
		Shader m_Shader;
		Renderer2D m_Renderer;
		RenderingPipeline2D m_RenderingPipeline;

	public:
		RenderingSystem2D(
			RenderingOptions renderingOptions,
			RenderingConfiguration2D& renderingConfiguration,
			Ludus::Engine::Core::AssetManager& assetManager,
			Ludus::Engine::Core::RenderViewRegistry& renderViewRegistry,
			Ludus::Engine::Core::SceneRegistry& sceneRegistry,
			const Ludus::Engine::Runtime::RuntimeEnvironment& runtimeEnvironment
		);

		~RenderingSystem2D() = default;

	protected:
		virtual void RenderImpl() override;
	};
}
