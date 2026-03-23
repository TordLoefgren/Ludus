#include "pch.h"

#include <memory>
#include <utility>

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/EditorHostBuilder.h>
#include <Ludus/Editor/Core/EditorSystem.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeOptions.h>
#include <Ludus/Engine/Windowing/WindowOptions.h>
#include <Ludus/UI/Systems/ImGuiModule.h>

namespace Ludus::Editor::Core
{
	std::unique_ptr<Ludus::Engine::Runtime::ApplicationHost> EditorHostBuilder::Build()
	{
		return m_ApplicationHostBuilder.Build();
	}

	EditorHostBuilder EditorHostBuilder::Create()
	{
		return EditorHostBuilder();
	}

	EditorHostBuilder& EditorHostBuilder::WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration)
	{
		m_EditorConfiguration = editorConfiguration;

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::AddDefaultEngine()
	{
		m_RuntimeOptions = Ludus::Engine::Runtime::RuntimeOptions(Ludus::Editor::Core::DefaultEditorExecutionMask);
		m_WindowOptions = Ludus::Engine::Windowing::WindowOptions(1920, 1080, "Ludus Editor", true, "Resources/LudusIcon.png");

		Ludus::UI::Systems::RegisterImGui(m_ApplicationHostBuilder);

		m_ApplicationHostBuilder
			.WithRuntimeOptions(m_RuntimeOptions)
			.WithWindowOptions(m_WindowOptions);

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::AddEditorSystem()
	{
		m_ApplicationHostBuilder.Configure([editorConfiguration = std::move(m_EditorConfiguration)](Ludus::Engine::Runtime::ApplicationHost& host) mutable
		{
			host.AddUpdateSystem<Ludus::Editor::Core::EditorSystem>(host, std::move(editorConfiguration));
		});

		return *this;
	}
}
