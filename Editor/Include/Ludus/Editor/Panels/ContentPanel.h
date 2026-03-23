#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/UI/AssetBrowser.h>

namespace Ludus::Editor::Panels
{
	class ContentPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		Ludus::UI::AssetBrowser m_AssetBrowser;

	public:
		virtual bool* GetOpenFlag(Ludus::Editor::Core::ProjectSessionContext& context) override { return &context.Shell.State.ActivePanelState.ShowContentPanel; }
		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;

		void Initialize(const std::filesystem::path& directory) { m_AssetBrowser.FromDirectory(directory); }
		void Clear() { m_AssetBrowser.Clear(); }
	};
}
