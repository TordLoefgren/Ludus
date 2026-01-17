#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/Collider2DComponentSerializer.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Collider2DComponentSerializer = Ludus::Engine::Persistance::Serializers::Collider2DComponentSerializer;

	TEST(Collider2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Collider2DComponent collider(
			1, 0, Ludus::Engine::Physics::Core::LayerMask::GetEmpty(), false
		);
		DomSaveArchive archive;

		// Act.
		Collider2DComponentSerializer::Save(archive, collider);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& colliderObject = AsObject(rootObject.at("Collider2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(colliderObject.at("OwnerHandle")));
		const auto layerIndex = std::get<uint8_t>(AsValue(colliderObject.at("LayerIndex")));
		const auto collidesWith = std::get<uint32_t>(AsValue(colliderObject.at("CollidesWith")));
		const auto isTrigger = std::get<bool>(AsValue(colliderObject.at("IsTrigger")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(layerIndex, 0);
		ASSERT_EQ(collidesWith, Ludus::Engine::Physics::Core::LayerMask::GetEmpty());
		ASSERT_EQ(isTrigger, false);
	}

	TEST(Collider2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto layerIndex = 0;
		const auto collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty();
		const auto isTrigger = false;

		auto collider = Ludus::Engine::Components::Collider2DComponent(
			ownerHandle, layerIndex, collidesWith, isTrigger
		);

		DomSaveArchive saveArchive;
		Collider2DComponentSerializer::Save(saveArchive, collider);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = Collider2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& colliderResult = result.GetValue();
		ASSERT_EQ(colliderResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(colliderResult.LayerIndex, layerIndex);
		ASSERT_EQ(colliderResult.CollidesWith, collidesWith);
		ASSERT_EQ(colliderResult.IsTrigger, isTrigger);
	}

	TEST(Collider2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Collider2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Collider2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& colliderResult = result.GetValue();
		ASSERT_EQ(colliderResult.OwnerHandle, 1);
		ASSERT_EQ(colliderResult.LayerIndex, 0);
		ASSERT_EQ(colliderResult.CollidesWith, Ludus::Engine::Physics::Core::LayerMask::GetEmpty());
		ASSERT_EQ(colliderResult.IsTrigger, false);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 3);
	}

	TEST(Collider2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Collider2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Collider2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Collider2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Collider2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
