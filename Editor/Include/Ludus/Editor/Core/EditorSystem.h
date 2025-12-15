#pragma once

#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Engine/Core/ISystem.h>

namespace Ludus::Editor::Core
{
	class EditorSystem final : public Ludus::Engine::Core::ISystem
	{
	private:
		EditorContext m_EditorContext;
		EditorConfiguration m_EditorConfiguration;
		Ludus::Editor::Panels::PanelRegistry m_PanelRegistry;

	public:
		EditorSystem(EditorConfiguration editorConfiguration);
		~EditorSystem() = default;

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void UpdateImpl(float deltaTime) override;
	};
}
