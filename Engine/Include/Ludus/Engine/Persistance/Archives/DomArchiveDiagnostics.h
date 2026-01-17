#pragma once

#include <algorithm>
#include <span>
#include <vector>

#include <Ludus/Engine/Persistance/Archives/ArchiveIssue.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/IArchiveDiagnostics.h>

namespace Ludus::Engine::Persistance::Archives
{
	class DomArchiveDiagnostics final : public IArchiveDiagnostics
	{
	private:
		std::vector<ArchiveIssue> m_Issues;

	public:
		virtual void Report(ArchiveIssue issue) override
		{
			m_Issues.push_back(std::move(issue));
		}

		std::span<const ArchiveIssue> GetIssues() const { return m_Issues; }

		std::vector<ArchiveIssue> GetErrors()
		{
			std::vector<ArchiveIssue> result;
			result.reserve(m_Issues.size());

			for (const auto& issue : m_Issues)
			{
				if (issue.Severity == ArchiveSeverity::Error)
				{
					result.push_back(issue);
				}
			}

			return result;
		}

		std::vector<ArchiveIssue> GetWarnings()
		{
			std::vector<ArchiveIssue> result;
			result.reserve(m_Issues.size());

			for (const auto& issue : m_Issues)
			{
				if (issue.Severity == ArchiveSeverity::Warn)
				{
					result.push_back(issue);
				}
			}

			return result;
		}

		bool HasErrors() const
		{
			auto errors = std::find_if(
				m_Issues.begin(),
				m_Issues.end(),
				[](const ArchiveIssue& issue)
				{
					return issue.Severity == ArchiveSeverity::Error;
				}
			);

			return errors != m_Issues.end();
		}

		bool HasWarnings() const
		{
			auto warnings = std::find_if(
				m_Issues.begin(),
				m_Issues.end(),
				[](const ArchiveIssue& issue)
				{
					return issue.Severity == ArchiveSeverity::Warn;
				}
			);

			return warnings != m_Issues.end();
		}
	};
}
