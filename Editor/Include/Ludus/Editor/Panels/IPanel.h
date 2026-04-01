#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Editor/Core/ProjectSessionContext.h>
#include <Ludus/Editor/Panels/PanelKind.h>

namespace Ludus::Editor::Panels
{
	using PanelHandle = uint32_t;

	class IPanel
	{
	public:
		virtual ~IPanel() = default;

		PanelHandle GetHandle() const { return m_Handle; }

		bool Update(Ludus::Editor::Core::ProjectSessionContext& context)
		{
			if (UsesPanelState())
			{
				m_Open = context.Shell.State.Panels.IsVisible(GetPanelKind());
			}

			if (!m_Open)
			{
				return true;
			}

			auto active = UpdateImpl(context);

			if (UsesPanelState())
			{
				context.Shell.State.Panels.SetVisible(GetPanelKind(), m_Open);
			}

			return active;
		}

		virtual bool IsAvailable(Ludus::Editor::Core::ProjectSessionContext& context)
		{
			return context.Shell.State.Mode == Ludus::Editor::Core::EditorMode::Session;
		}

	protected:
		inline static PanelHandle s_NextHandle = 1;

		PanelHandle m_Handle = s_NextHandle++;
		bool m_Open = true;

		virtual bool UsesPanelState() const { return false; }

		virtual Ludus::Editor::Panels::PanelKind GetPanelKind() const = 0;
		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) = 0;

		std::string CreateUniqueWindowTitle(std::string_view visibleTitle)
		{
			return std::format("{}##{}", visibleTitle, m_Handle);
		}

		std::string CreateWindowTitle(std::string_view visibleTitle)
		{
			return std::format("{}##{}", visibleTitle, visibleTitle);
		}

		std::string CreateWindowTitleWithIcon(std::string_view icon, std::string_view visibleTitle)
		{
			return std::format("{} {}##{}", icon, visibleTitle, visibleTitle);
		}
	};
}
