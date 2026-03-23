#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Editor/Core/ProjectSessionContext.h>

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
			// Singleton panels can use this hook to be toggled on and off.
			auto* external = GetOpenFlag(context);
			if (external)
			{
				m_Open = *external;
			}

			if (!m_Open)
			{
				return true;
			}

			auto active = UpdateImpl(context);

			if (external)
			{
				*external = m_Open;
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

		virtual bool* GetOpenFlag(Ludus::Editor::Core::ProjectSessionContext& context) { return nullptr; }

		virtual bool UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context) = 0;

		std::string CreateWindowTitle(std::string_view visibleTitle)
		{
			return std::format("{}##{}", visibleTitle, visibleTitle);
		}

		std::string CreateUniqueWindowTitle(std::string_view visibleTitle)
		{
			return std::format("{}##{}", visibleTitle, m_Handle);
		}
	};
}
