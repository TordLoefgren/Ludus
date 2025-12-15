#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include <Ludus/Editor/Panels/PanelContext.h>

namespace Ludus::Editor::Panels
{

	class IPanel
	{
	public:
		virtual ~IPanel() = default;

		void Update(PanelContext& context) { UpdateImpl(context); }

	protected:
		using PanelHandle = uint32_t;
		inline static PanelHandle s_NextHandle = 1;

		PanelHandle m_Handle = s_NextHandle++;
		bool m_Open = true;

		virtual void UpdateImpl(PanelContext& context) = 0;

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
