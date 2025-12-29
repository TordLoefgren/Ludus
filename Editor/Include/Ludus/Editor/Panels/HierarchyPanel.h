#pragma once

#include <Ludus/Editor/Core/EditorSelection.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Panels
{
	struct SceneToolBarActions;
	struct SceneContextMenuActions;
	struct EntityRowActions;
	struct EntityRowCommands;

	class HierarchyPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		std::optional<Ludus::Engine::Core::SceneHandle> m_SelectedSceneHandle = std::nullopt;

		const static SceneToolBarActions DrawSceneToolBar(std::span<const Ludus::Engine::Core::Scene> scenes, std::optional<Ludus::Engine::Core::SceneHandle> sceneHandle);
		static std::optional<Ludus::Engine::Core::SceneHandle> ApplySceneToolBarActions(const SceneToolBarActions& actions, Ludus::Editor::Panels::PanelContext& context);

		const static SceneContextMenuActions DrawSceneContextMenu(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene);
		static void ApplySceneContextMenuActions(const SceneContextMenuActions& actions, Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene);

		static const EntityRowActions DrawEntityRow(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::EntityHandle entityHandle);
		void AddToEntityRowCommands(EntityRowCommands& commands, const EntityRowActions& actions, Ludus::Engine::Core::EntityHandle entityHandle);
		static void ApplyEntityRowCommands(const EntityRowCommands& commands, Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene);

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Panels::PanelContext& context) override { return &context.ActivePanelState.ShowHierarchyPanel; }

		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;
	};
}
