#pragma once

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Panels
{
	class HierarchyPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		Ludus::Engine::Core::Random m_Random { };

		void DrawSceneContextMenu(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene);
		void DrawEntityRow(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::EntityHandle entityHandle);
		void DrawSceneRow(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene);

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Panels::PanelContext& context) override { return &context.ActivePanelState.ShowHierarchyPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
