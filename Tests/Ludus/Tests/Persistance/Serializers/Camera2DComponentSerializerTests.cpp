#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/Camera2DComponentSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Camera2DComponentSerializer = Ludus::Engine::Persistance::Serializers::Camera2DComponentSerializer;

	TEST(Camera2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Camera2DComponent camera(1, 10.0f, -1);
		DomSaveArchive archive;

		// Act.
		Camera2DComponentSerializer::Save(archive, camera);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& cameraObject = AsObject(rootObject.at("Camera2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(cameraObject.at("OwnerHandle")));
		const auto orthographicSize = std::get<float>(AsValue(cameraObject.at("OrthographicSize")));
		const auto priority = std::get<int>(AsValue(cameraObject.at("Priority")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(orthographicSize, 10.0f);
		ASSERT_EQ(priority, -1);
	}

	TEST(Camera2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto orthographicSize = 10.0f;
		const auto priority = -1;

		auto camera = Ludus::Engine::Components::Camera2DComponent(ownerHandle, orthographicSize, priority);

		DomSaveArchive saveArchive;
		Camera2DComponentSerializer::Save(saveArchive, camera);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = Camera2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& cameraResult = result.GetValue();
		ASSERT_EQ(cameraResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(cameraResult.OrthographicSize, orthographicSize);
		ASSERT_EQ(cameraResult.Priority, priority);
	}

	TEST(Camera2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Camera2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Camera2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& cameraResult = result.GetValue();
		ASSERT_EQ(cameraResult.OwnerHandle, 1);
		ASSERT_EQ(cameraResult.OrthographicSize, 10.0f);
		ASSERT_EQ(cameraResult.Priority, -1);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 2);
	}

	TEST(Camera2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Camera2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Camera2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Camera2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Camera2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
