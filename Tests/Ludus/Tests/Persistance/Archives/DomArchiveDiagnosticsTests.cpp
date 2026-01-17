#include "pch.h"

#include <string>
#include <variant>

#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>

namespace Ludus::Tests::Persistance::Archives
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using ArchiveAction = Ludus::Engine::Persistance::Archives::ArchiveAction;
	using ArchiveSeverity = Ludus::Engine::Persistance::Archives::ArchiveSeverity;

	static constexpr auto Path = "Path";
	static constexpr auto Message = "Message";

	TEST(DomArchiveDiagnostics, Report_AddIssueToCollection)
	{
		// Arrange.
		DomArchiveDiagnostics diagnostics;

		// Act.
		diagnostics.Report({ ArchiveSeverity::Warn, ArchiveAction::ReadFailed, Path, Message });
		diagnostics.Report({ ArchiveSeverity::Warn, ArchiveAction::ReadFailed, Path, Message });
		diagnostics.Report({ ArchiveSeverity::Error, ArchiveAction::ReadFailed, Path, Message });
		diagnostics.Report({ ArchiveSeverity::Error, ArchiveAction::ReadFailed, Path, Message });

		// Assert.
		ASSERT_EQ(diagnostics.GetIssues().size(), 4);
	}

	TEST(DomArchiveDiagnostics, HasErrors_ReturnTrue_WhenIssuesContainErrors)
	{
		// Arrange.
		DomArchiveDiagnostics diagnostics;
		diagnostics.Report({ ArchiveSeverity::Error, ArchiveAction::ReadFailed, Path, Message });

		// Act.
		const auto hasErrors = diagnostics.HasErrors();

		// Assert.
		ASSERT_TRUE(hasErrors);
	}

	TEST(DomArchiveDiagnostics, HasErrors_ReturnFalse_WhenIssuesDoesNotContainErrors)
	{
		// Arrange.
		DomArchiveDiagnostics diagnostics;
		diagnostics.Report({ ArchiveSeverity::Warn, ArchiveAction::ReadFailed, Path, Message });

		// Act.
		const auto hasErrors = diagnostics.HasErrors();

		// Assert.
		ASSERT_FALSE(hasErrors);
	}

	TEST(DomArchiveDiagnostics, HasWarnings_ReturnTrue_WhenIssuesContainWarnings)
	{
		// Arrange.
		DomArchiveDiagnostics diagnostics;
		diagnostics.Report({ ArchiveSeverity::Warn, ArchiveAction::ReadFailed, Path, Message });

		// Act.
		const auto hasWarnings = diagnostics.HasWarnings();

		// Assert.
		ASSERT_TRUE(hasWarnings);
	}

	TEST(DomArchiveDiagnostics, HasWarnings_ReturnFalse_WhenIssuesDoesNotContainWarnings)
	{
		// Arrange.
		DomArchiveDiagnostics diagnostics;
		diagnostics.Report({ ArchiveSeverity::Error, ArchiveAction::ReadFailed, Path, Message });

		// Act.
		const auto hasWarnings = diagnostics.HasWarnings();

		// Assert.
		ASSERT_FALSE(hasWarnings);
	}
}
