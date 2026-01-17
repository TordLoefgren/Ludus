#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/Transform2DComponentSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Transform2DComponentSerializer = Ludus::Engine::Persistance::Serializers::Transform2DComponentSerializer;

	using Ludus::Engine::Persistance::Archives::AsObject;
	using Ludus::Engine::Persistance::Archives::AsValue;

	TEST(Transform2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Transform2DComponent transform(1, { 5, 10 }, { 2, 4 }, 45);
		DomSaveArchive archive;

		// Act.
		Transform2DComponentSerializer::Save(archive, transform);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& transformObject = AsObject(rootObject.at("Transform2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(transformObject.at("OwnerHandle")));

		const auto& positionObject = AsObject(transformObject.at("Position"));
		const auto positionX = std::get<float>(AsValue(positionObject.at("X")));
		const auto positionY = std::get<float>(AsValue(positionObject.at("Y")));

		const auto& scaleObject = AsObject(transformObject.at("Scale"));
		const auto scaleX = std::get<float>(AsValue(scaleObject.at("X")));
		const auto scaleY = std::get<float>(AsValue(scaleObject.at("Y")));

		const auto rotation = std::get<float>(AsValue(transformObject.at("Rotation")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(positionX, 5.0f);
		ASSERT_EQ(positionY, 10.0f);
		ASSERT_EQ(scaleX, 2.0f);
		ASSERT_EQ(scaleY, 4.0f);
		ASSERT_EQ(rotation, 45.0f);
	}

	TEST(Transform2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto positionX = 5.0f;
		const auto positionY = 10.0f;
		const auto scaleX = 2.0f;
		const auto scaleY = 4.0f;
		const auto rotation = 45.0f;

		auto transform = Ludus::Engine::Components::Transform2DComponent(
			ownerHandle, { positionX, positionY }, { scaleX, scaleY }, rotation
		);

		DomSaveArchive saveArchive;
		Transform2DComponentSerializer::Save(saveArchive, transform);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = Transform2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& transformResult = result.GetValue();
		ASSERT_EQ(transformResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(transformResult.Position.X, positionX);
		ASSERT_EQ(transformResult.Position.Y, positionY);
		ASSERT_EQ(transformResult.Scale.X, scaleX);
		ASSERT_EQ(transformResult.Scale.Y, scaleY);
		ASSERT_EQ(transformResult.Rotation, rotation);
	}

	TEST(Transform2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Transform2D");

		saveArchive.WriteUint32("OwnerHandle", 1);

		saveArchive.BeginObject("Position");
		saveArchive.EndObject();

		saveArchive.BeginObject("Scale");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Transform2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& transformResult = result.GetValue();
		ASSERT_EQ(transformResult.OwnerHandle, 1);
		ASSERT_EQ(transformResult.Position.X, 0.0f);
		ASSERT_EQ(transformResult.Position.Y, 0.0f);
		ASSERT_EQ(transformResult.Scale.X, 1.0f);
		ASSERT_EQ(transformResult.Scale.Y, 1.0f);
		ASSERT_EQ(transformResult.Rotation, 0.0f);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 5);
	}

	TEST(Transform2DComponentSerializer, Load_DefaultsOptionalFields_When_HeadersAreMissing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Transform2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Transform2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& transformResult = result.GetValue();
		ASSERT_EQ(transformResult.OwnerHandle, 1);
		ASSERT_EQ(transformResult.Position.X, 0.0f);
		ASSERT_EQ(transformResult.Position.Y, 0.0f);
		ASSERT_EQ(transformResult.Scale.X, 1.0f);
		ASSERT_EQ(transformResult.Scale.Y, 1.0f);
		ASSERT_EQ(transformResult.Rotation, 0.0f);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 3);
	}

	TEST(Transform2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Transform2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Transform2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Transform2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Transform2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
