#pragma once

#include <Ludus/Engine/Persistance/Archives/ArchiveIssue.h>

namespace Ludus::Engine::Persistance::Archives
{
	class IArchiveDiagnostics
	{
	public:
		virtual ~IArchiveDiagnostics() = default;

		virtual void Report(ArchiveIssue issue) = 0;
	};
}
