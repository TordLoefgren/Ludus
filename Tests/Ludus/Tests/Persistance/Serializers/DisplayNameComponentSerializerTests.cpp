#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/DisplayNameComponentSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using DisplayNameComponentSerializer = Ludus::Engine::Persistance::Serializers::DisplayNameComponentSerializer;

	TEST(DisplayNameComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::DisplayNameComponent displayName(1, "Name");
		DomSaveArchive archive;

		// Act.
		DisplayNameComponentSerializer::Save(archive, displayName);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& displayNameObject = AsObject(rootObject.at("DisplayName"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(displayNameObject.at("OwnerHandle")));
		const auto value = std::get<std::string>(AsValue(displayNameObject.at("Value")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(value, "Name");
	}

	TEST(DisplayNameComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto value = "Name";

		auto displayName = Ludus::Engine::Components::DisplayNameComponent(ownerHandle, value);

		DomSaveArchive saveArchive;
		DisplayNameComponentSerializer::Save(saveArchive, displayName);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = DisplayNameComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(displayNameResult.Value, value);
	}

	TEST(DisplayNameComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("DisplayName");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = DisplayNameComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& displayNameResult = result.GetValue();
		ASSERT_EQ(displayNameResult.OwnerHandle, 1);
		ASSERT_EQ(displayNameResult.Value, "");

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 1);
	}

	TEST(DisplayNameComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("DisplayName");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = DisplayNameComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(DisplayNameComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = DisplayNameComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
