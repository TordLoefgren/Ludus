#pragma once

#include <algorithm>
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
		std::vector<PanelHandle> m_ScheduledRemovals;

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

		void ScheduleRemove(PanelHandle handle)
		{
			m_ScheduledRemovals.emplace_back(handle);
		}

		bool ApplyRemovals()
		{
			if (m_ScheduledRemovals.empty())
			{
				return false;
			}

			const auto previousSize = m_Panels.size();

			std::erase_if(
				m_Panels,
				[this](const std::unique_ptr<IPanel>& panel)
				{
					return std::ranges::find(m_ScheduledRemovals, panel->GetHandle()) != m_ScheduledRemovals.end();
				}
			);

			m_ScheduledRemovals.clear();
			return m_Panels.size() != previousSize;
		}


		size_t GetSize() const { return m_Panels.size(); }
	};
}
