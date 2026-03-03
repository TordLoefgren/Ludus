#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Editor/Panels/PanelContext.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/UI/AssetBrowser.h>

namespace Ludus::Editor::Panels
{
	class ProjectPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		Ludus::UI::AssetBrowser m_AssetBrowser;

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Panels::PanelContext& context) override { return &context.ActivePanelState.ShowProjectPanel; }
		virtual bool UpdateImpl(Ludus::Editor::Panels::PanelContext& context) override;

		void Refresh(std::optional<Ludus::Engine::Core::ProjectContext> projectContext);

		void Initialize(const std::filesystem::path& directory) { m_AssetBrowser.FromDirectory(directory); }
		void Clear() { m_AssetBrowser.Clear(); }
	};
}
