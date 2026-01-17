#include "pch.h"

#include <cstdint>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/RigidBody2DComponentSerializer.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using RigidBody2DComponentSerializer = Ludus::Engine::Persistance::Serializers::RigidBody2DComponentSerializer;

	TEST(RigidBody2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::RigidBody2DComponent rigidBody(
			1, { 2.0f, 4.0f }, Ludus::Engine::Physics::Core::BodyType::Dynamic, 1.0f, 1.0f
		);
		DomSaveArchive archive;

		// Act.
		RigidBody2DComponentSerializer::Save(archive, rigidBody);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& rigidBodyObject = AsObject(rootObject.at("RigidBody2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(rigidBodyObject.at("OwnerHandle")));

		const auto& velocityObject = AsObject(rigidBodyObject.at("Velocity"));
		const auto velocityX = std::get<float>(AsValue(velocityObject.at("X")));
		const auto velocityY = std::get<float>(AsValue(velocityObject.at("Y")));

		const auto type = std::get<std::string>(AsValue(rigidBodyObject.at("Type")));
		const auto gravityScale = std::get<float>(AsValue(rigidBodyObject.at("GravityScale")));
		const auto mass = std::get<float>(AsValue(rigidBodyObject.at("Mass")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(type, "Dynamic");
		ASSERT_EQ(velocityX, 2.0f);
		ASSERT_EQ(velocityY, 4.0f);
		ASSERT_EQ(gravityScale, 1.0f);
		ASSERT_EQ(mass, 1.0f);
	}

	TEST(RigidBody2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto velocityX = 5.0f;
		const auto velocityY = 10.0f;
		const auto type = Ludus::Engine::Physics::Core::BodyType::Dynamic;
		const auto gravityScale = 1.0f;
		const auto mass = 1.0f;

		auto rigidBody = Ludus::Engine::Components::RigidBody2DComponent(
			ownerHandle, { velocityX, velocityY }, type, gravityScale, mass
		);

		DomSaveArchive saveArchive;
		RigidBody2DComponentSerializer::Save(saveArchive, rigidBody);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = RigidBody2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& rigidBodyResult = result.GetValue();
		ASSERT_EQ(rigidBodyResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(rigidBodyResult.Type, type);
		ASSERT_EQ(rigidBodyResult.Velocity.X, velocityX);
		ASSERT_EQ(rigidBodyResult.Velocity.Y, velocityY);
		ASSERT_EQ(rigidBodyResult.GravityScale, gravityScale);
		ASSERT_EQ(rigidBodyResult.Mass, mass);
	}

	TEST(RigidBody2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("RigidBody2D");

		saveArchive.WriteUint32("OwnerHandle", 1);

		saveArchive.BeginObject("Velocity");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = RigidBody2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& rigidBodyResult = result.GetValue();
		ASSERT_EQ(rigidBodyResult.OwnerHandle, 1);
		ASSERT_EQ(rigidBodyResult.Type, Ludus::Engine::Physics::Core::BodyType::Dynamic);
		ASSERT_EQ(rigidBodyResult.Velocity.X, 0.0f);
		ASSERT_EQ(rigidBodyResult.Velocity.Y, 0.0f);
		ASSERT_EQ(rigidBodyResult.GravityScale, 1.0f);
		ASSERT_EQ(rigidBodyResult.Mass, 1.0f);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 5);
	}

	TEST(RigidBody2DComponentSerializer, Load_DefaultsOptionalFields_When_HeadersAreMissing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("RigidBody2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = RigidBody2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& rigidBodyResult = result.GetValue();
		ASSERT_EQ(rigidBodyResult.OwnerHandle, 1);
		ASSERT_EQ(rigidBodyResult.Type, Ludus::Engine::Physics::Core::BodyType::Dynamic);
		ASSERT_EQ(rigidBodyResult.Velocity.X, 0.0f);
		ASSERT_EQ(rigidBodyResult.Velocity.Y, 0.0f);
		ASSERT_EQ(rigidBodyResult.GravityScale, 1.0f);
		ASSERT_EQ(rigidBodyResult.Mass, 1.0f);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 4);
	}

	TEST(RigidBody2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("RigidBody2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = RigidBody2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(RigidBody2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = RigidBody2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
