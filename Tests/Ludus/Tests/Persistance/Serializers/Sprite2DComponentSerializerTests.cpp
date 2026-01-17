#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/Sprite2DComponentSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Sprite2DComponentSerializer = Ludus::Engine::Persistance::Serializers::Sprite2DComponentSerializer;

	TEST(Sprite2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Sprite2DComponent sprite(
			1, Ludus::Engine::Graphics::Shape::Quad, Ludus::Engine::Graphics::Colors::White, nullptr, true
		);
		DomSaveArchive archive;

		// Act.
		Sprite2DComponentSerializer::Save(archive, sprite);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& spriteObject = AsObject(rootObject.at("Sprite2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(spriteObject.at("OwnerHandle")));
		const auto shape = std::get<std::string>(AsValue(spriteObject.at("Shape")));

		const auto& colorObject = AsObject(spriteObject.at("Color"));
		const auto r = std::get<float>(AsValue(colorObject.at("R")));
		const auto g = std::get<float>(AsValue(colorObject.at("G")));
		const auto b = std::get<float>(AsValue(colorObject.at("B")));
		const auto a = std::get<float>(AsValue(colorObject.at("A")));

		const auto fill = std::get<bool>(AsValue(spriteObject.at("Fill")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(shape, "Quad");
		ASSERT_EQ(r, 1.0f);
		ASSERT_EQ(g, 1.0f);
		ASSERT_EQ(b, 1.0f);
		ASSERT_EQ(a, 1.0f);
		ASSERT_EQ(fill, true);
	}

	TEST(Sprite2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto shape = Ludus::Engine::Graphics::Shape::Quad;
		const auto color = Ludus::Engine::Graphics::Colors::White;
		const auto texture = nullptr;
		const auto fill = true;

		auto sprite = Ludus::Engine::Components::Sprite2DComponent(
			ownerHandle, shape, color, texture, fill
		);

		DomSaveArchive saveArchive;
		Sprite2DComponentSerializer::Save(saveArchive, sprite);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = Sprite2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& spriteResult = result.GetValue();
		ASSERT_EQ(spriteResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(spriteResult.Shape, shape);
		ASSERT_EQ(spriteResult.Color.R, color.R);
		ASSERT_EQ(spriteResult.Color.G, color.G);
		ASSERT_EQ(spriteResult.Color.B, color.B);
		ASSERT_EQ(spriteResult.Color.A, color.A);
		ASSERT_EQ(spriteResult.Texture, texture);
		ASSERT_EQ(spriteResult.Fill, fill);
	}

	TEST(Sprite2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Sprite2D");

		saveArchive.WriteUint32("OwnerHandle", 1);

		saveArchive.BeginObject("Shape");
		saveArchive.EndObject();

		saveArchive.BeginObject("Color");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Sprite2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& spriteResult = result.GetValue();
		ASSERT_EQ(spriteResult.OwnerHandle, 1);
		ASSERT_EQ(spriteResult.Shape, Ludus::Engine::Graphics::Shape::Quad);
		ASSERT_EQ(spriteResult.Color.R, 1.0);
		ASSERT_EQ(spriteResult.Color.G, 1.0);
		ASSERT_EQ(spriteResult.Color.B, 1.0);
		ASSERT_EQ(spriteResult.Color.A, 1.0);
		ASSERT_EQ(spriteResult.Texture, nullptr);
		ASSERT_EQ(spriteResult.Fill, true);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 6);
	}

	TEST(Sprite2DComponentSerializer, Load_DefaultsOptionalFields_When_HeadersAreMissing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Sprite2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Sprite2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& spriteResult = result.GetValue();
		ASSERT_EQ(spriteResult.OwnerHandle, 1);
		ASSERT_EQ(spriteResult.Shape, Ludus::Engine::Graphics::Shape::Quad);
		ASSERT_EQ(spriteResult.Color.R, 1.0);
		ASSERT_EQ(spriteResult.Color.G, 1.0);
		ASSERT_EQ(spriteResult.Color.B, 1.0);
		ASSERT_EQ(spriteResult.Color.A, 1.0);
		ASSERT_EQ(spriteResult.Texture, nullptr);
		ASSERT_EQ(spriteResult.Fill, true);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 3);
	}

	TEST(Sprite2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Sprite2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Sprite2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Sprite2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Sprite2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
