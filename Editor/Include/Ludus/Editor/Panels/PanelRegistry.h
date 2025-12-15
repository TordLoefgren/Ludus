#pragma once

#include <memory>
#include <span>
#include <vector>

#include <Ludus/Editor/Panels/IPanel.h>

namespace Ludus::Editor::Panels
{
	struct PanelRegistry
	{
	private:
		std::vector<std::unique_ptr<IPanel>> m_Panels;

	public:
		std::span<const std::unique_ptr<IPanel>> View() const
		{
			return m_Panels;
		}

		void Register(std::unique_ptr<IPanel> panel)
		{
			m_Panels.push_back(std::move(panel));
		}

		void Clear()
		{
			m_Panels.clear();
		}

		size_t GetSize() const { return m_Panels.size(); }
	};
}
