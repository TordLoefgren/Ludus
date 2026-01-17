#pragma once

#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>

namespace Ludus::Engine::Persistance::Archives
{
	struct ArchiveIssue
	{
		ArchiveSeverity Severity;
		ArchiveAction Action;
		std::string Path;
		std::string Message;
	};
}
