#include "pch.h"

#include <cstdint>
#include <string>

#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Persistance/Archives/DomArchiveDiagnostics.h>
#include <Ludus/Engine/Persistance/Archives/DomLoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/DomSaveArchive.h>
#include <Ludus/Engine/Persistance/Serializers/Text2DComponentSerializer.h>

namespace Ludus::Tests::Persistance::Serializers
{
	using DomArchiveDiagnostics = Ludus::Engine::Persistance::Archives::DomArchiveDiagnostics;
	using DomSaveArchive = Ludus::Engine::Persistance::Archives::DomSaveArchive;
	using DomLoadArchive = Ludus::Engine::Persistance::Archives::DomLoadArchive;
	using Text2DComponentSerializer = Ludus::Engine::Persistance::Serializers::Text2DComponentSerializer;

	using Ludus::Engine::Persistance::Archives::AsObject;
	using Ludus::Engine::Persistance::Archives::AsValue;

	TEST(Text2DComponentSerializer, Save_WritesExpectedComponentValues)
	{
		// Arrange.
		Ludus::Engine::Components::Text2DComponent text(
			1, "Text", Ludus::Engine::Graphics::Colors::White, Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		);
		DomSaveArchive archive;

		// Act.
		Text2DComponentSerializer::Save(archive, text);

		// Assert.
		const auto& rootObject = AsObject(archive.GetRoot());
		const auto& textObject = AsObject(rootObject.at("Text2D"));

		const auto ownerHandle = std::get<uint32_t>(AsValue(textObject.at("OwnerHandle")));
		const auto textValue = std::get<std::string>(AsValue(textObject.at("Text")));

		const auto& colorObject = AsObject(textObject.at("Color"));
		const auto r = std::get<float>(AsValue(colorObject.at("R")));
		const auto g = std::get<float>(AsValue(colorObject.at("G")));
		const auto b = std::get<float>(AsValue(colorObject.at("B")));
		const auto a = std::get<float>(AsValue(colorObject.at("A")));

		const auto horizontalAlignment = std::get<std::string>(AsValue(textObject.at("HorizontalAlignment")));

		ASSERT_EQ(ownerHandle, 1u);
		ASSERT_EQ(textValue, "Text");
		ASSERT_EQ(r, 1.0f);
		ASSERT_EQ(g, 1.0f);
		ASSERT_EQ(b, 1.0f);
		ASSERT_EQ(a, 1.0f);
		ASSERT_EQ(horizontalAlignment, "Left");
	}

	TEST(Text2DComponentSerializer, Load_ReadsFields_When_ComponentHasValues)
	{
		// Arrange.
		const auto ownerHandle = 1;
		const auto textValue = "Text";
		const auto color = Ludus::Engine::Graphics::Colors::White;
		const auto horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left;

		auto text = Ludus::Engine::Components::Text2DComponent(
			ownerHandle, textValue, color, horizontalAlignment
		);

		DomSaveArchive saveArchive;
		Text2DComponentSerializer::Save(saveArchive, text);

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto& result = Text2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& textResult = result.GetValue();
		ASSERT_EQ(textResult.OwnerHandle, ownerHandle);
		ASSERT_EQ(textResult.Text, textValue);
		ASSERT_EQ(textResult.Color.R, color.R);
		ASSERT_EQ(textResult.Color.G, color.G);
		ASSERT_EQ(textResult.Color.B, color.B);
		ASSERT_EQ(textResult.Color.A, color.A);
		ASSERT_EQ(textResult.HorizontalAlignment, horizontalAlignment);
	}

	TEST(Text2DComponentSerializer, Load_DefaultsOptionalFields_When_Missing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Text2D");

		saveArchive.WriteUint32("OwnerHandle", 1);

		saveArchive.BeginObject("Color");
		saveArchive.EndObject();

		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Text2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& textResult = result.GetValue();
		ASSERT_EQ(textResult.OwnerHandle, 1);
		ASSERT_EQ(textResult.Text, "");
		ASSERT_EQ(textResult.Color.R, 1.0);
		ASSERT_EQ(textResult.Color.G, 1.0);
		ASSERT_EQ(textResult.Color.B, 1.0);
		ASSERT_EQ(textResult.Color.A, 1.0);
		ASSERT_EQ(textResult.HorizontalAlignment, Ludus::Engine::Graphics::HorizontalTextAlignment::Left);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 6);
	}

	TEST(Text2DComponentSerializer, Load_DefaultsOptionalFields_When_HeadersAreMissing_And_Warns)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Text2D");
		saveArchive.WriteUint32("OwnerHandle", 1);
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomArchiveDiagnostics diagnostics;
		DomLoadArchive loadArchive(root, &diagnostics);

		// Act.
		const auto result = Text2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& textResult = result.GetValue();
		ASSERT_EQ(textResult.OwnerHandle, 1);
		ASSERT_EQ(textResult.Text, "");
		ASSERT_EQ(textResult.Color.R, 1.0);
		ASSERT_EQ(textResult.Color.G, 1.0);
		ASSERT_EQ(textResult.Color.B, 1.0);
		ASSERT_EQ(textResult.Color.A, 1.0);
		ASSERT_EQ(textResult.HorizontalAlignment, Ludus::Engine::Graphics::HorizontalTextAlignment::Left);

		ASSERT_TRUE(diagnostics.HasWarnings());
		ASSERT_EQ(diagnostics.GetIssues().size(), 3);
	}

	TEST(Text2DComponentSerializer, Load_Fails_When_RequiredFieldsAreMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		saveArchive.BeginObject("Text2D");
		saveArchive.EndObject();

		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Text2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}

	TEST(Text2DComponentSerializer, Load_Fails_When_ComponentHeaderIsMissing)
	{
		// Arrange.
		DomSaveArchive saveArchive;
		const auto& root = saveArchive.GetRoot();
		DomLoadArchive loadArchive(root);

		// Act.
		const auto result = Text2DComponentSerializer::Load(loadArchive);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
