#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/UI/Elements/ContentBrowser.h>

namespace Ludus::Editor::Panels
{
	class ContentPanel final : public Ludus::Editor::Panels::IPanel
	{
	private:
		Ludus::UI::Elements::ContentBrowser m_ContentBrowser;

	public:
		virtual Ludus::Editor::Panels::PanelKind GetPanelKind() const override { return Ludus::Editor::Panels::PanelKind::Content; }
		virtual bool UsesVisibilityState() const override { return true; }
		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) override;

		void Initialize(const std::filesystem::path& directory) { m_ContentBrowser.FromDirectory(directory); }
		void Clear() { m_ContentBrowser.Clear(); }
	};
}
