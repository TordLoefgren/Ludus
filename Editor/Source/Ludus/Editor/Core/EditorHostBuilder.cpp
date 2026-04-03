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
		LUDUS_ASSERT(m_RuntimeOptions, "Cannot build editor host without runtime options.");
		LUDUS_ASSERT(m_WindowOptions, "Cannot build editor host without window options.");

		m_ApplicationHostBuilder
			.WithRuntimeOptions(std::move(*m_RuntimeOptions))
			.WithWindowOptions(std::move(*m_WindowOptions));

		if (m_UseImGui)
		{
			Ludus::UI::Systems::RegisterImGui(m_ApplicationHostBuilder);
		}

		auto host = m_ApplicationHostBuilder.Build();

		if (m_UseEditor)
		{
			host->AddUpdateSystem<Ludus::Editor::Core::EditorSystem>(
				*host,
				std::move(m_EditorConfiguration),
				std::move(m_StartupOptions)
			);
		}

		return host;
	}

	EditorHostBuilder EditorHostBuilder::Create()
	{
		return EditorHostBuilder();
	}

	EditorHostBuilder& EditorHostBuilder::WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration editorConfiguration)
	{
		m_EditorConfiguration = std::move(editorConfiguration);

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::WithRuntimeOptions(Ludus::Engine::Runtime::RuntimeOptions runtimeOptions)
	{
		m_RuntimeOptions = std::move(runtimeOptions);

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::WithStartupOptions(EditorStartupOptions startupOptions)
	{
		m_StartupOptions = std::move(startupOptions);

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::WithWindowOptions(Ludus::Engine::Windowing::WindowOptions windowOptions)
	{
		m_WindowOptions = std::move(windowOptions);

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::UseEditor()
	{
		if (m_UseEditor)
		{
			LUDUS_LOG_WARN("Invalid operation: Cannot add editor more than once.");
		}

		m_UseEditor = true;
		m_UseImGui = true;

		return *this;
	}

	EditorHostBuilder& EditorHostBuilder::UseEditorHostDefaults()
	{
		if (!m_RuntimeOptions)
		{
			m_RuntimeOptions = Ludus::Engine::Runtime::RuntimeOptions(Ludus::Editor::Core::DefaultEditorExecutionMask);
		}

		if (!m_WindowOptions)
		{
			m_WindowOptions = Ludus::Engine::Windowing::WindowOptions(
				1920,
				1080,
				"Ludus Editor",
				true,
				"Resources/LudusIcon.png"
			);
		}

		return *this;
	}
}
