#include "pch.h"

#include <string_view>

#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomNode.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>
#include <Ludus/Engine/Serialization/Schemas/RuntimeLaunchSettingsSchema.h>

namespace Ludus::EngineTests::Serialization::Schemas
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using RuntimeLaunchSettings = Ludus::Engine::Runtime::RuntimeLaunchSettings;
	using RuntimeLaunchSettingsSchema = Ludus::Engine::Serialization::Schemas::RuntimeLaunchSettingsSchema;
	using Token = Ludus::Engine::Serialization::Core::Token;

	using Ludus::Engine::Serialization::Core::AsObject;
	using Ludus::Engine::Serialization::Core::AsValue;
	using Ludus::Engine::Serialization::Core::DomNode;
	using Ludus::Engine::Serialization::Core::DomObject;

	static const DomNode* FindMember(const DomObject& object, std::string_view key)
	{
		for (const auto& [memberKey, memberValue] : object)
		{
			if (memberKey == key)
			{
				return memberValue.get();
			}
		}

		return nullptr;
	}

	TEST(RuntimeLaunchSettingsSchema, Serialize_WritesExpectedRuntimeSettingValues)
	{
		// Arrange.
		RuntimeLaunchSettings runtimeLaunchSettings;
		runtimeLaunchSettings.WindowSettings.StartupWidth = 1280;
		runtimeLaunchSettings.WindowSettings.StartupHeight = 720;
		runtimeLaunchSettings.WindowSettings.IsResizeable = false;
		runtimeLaunchSettings.WindowSettings.IsMaximized = true;
		runtimeLaunchSettings.PresentationSettings.InternalResolution = { 640, 360 };
		runtimeLaunchSettings.PresentationSettings.UseFixedRenderResolution = true;

		DomDocument document;
		DomTokenStreamWriter writer(document);

		// Act.
		RuntimeLaunchSettingsSchema::Serialize(writer, runtimeLaunchSettings);

		// Assert.
		const auto* root = document.GetRoot();
		ASSERT_NE(root, nullptr);

		const auto& settingsObject = AsObject(*root);
		const auto& windowObject = AsObject(*FindMember(settingsObject, "WindowSettings"));
		const auto& presentationObject = AsObject(*FindMember(settingsObject, "PresentationSettings"));
		const auto& resolutionObject = AsObject(*FindMember(presentationObject, "InternalResolution"));

		ASSERT_EQ(std::get<int64_t>(AsValue(*FindMember(windowObject, "StartupWidth"))), 1280);
		ASSERT_EQ(std::get<int64_t>(AsValue(*FindMember(windowObject, "StartupHeight"))), 720);
		ASSERT_EQ(std::get<bool>(AsValue(*FindMember(windowObject, "IsResizeable"))), false);
		ASSERT_EQ(std::get<bool>(AsValue(*FindMember(windowObject, "IsMaximized"))), true);
		ASSERT_EQ(std::get<int64_t>(AsValue(*FindMember(resolutionObject, "Width"))), 640);
		ASSERT_EQ(std::get<int64_t>(AsValue(*FindMember(resolutionObject, "Height"))), 360);
		ASSERT_EQ(std::get<bool>(AsValue(*FindMember(presentationObject, "UseFixedRenderResolution"))), true);
	}

	TEST(RuntimeLaunchSettingsSchema, Deserialize_ReadsFields_When_RuntimeLaunchSettingsHasValues)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "WindowSettings" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "StartupWidth" });
		writer.Emit(Token::Int { 1280 });
		writer.Emit(Token::Key { "StartupHeight" });
		writer.Emit(Token::Int { 720 });
		writer.Emit(Token::Key { "IsResizeable" });
		writer.Emit(Token::Bool { false });
		writer.Emit(Token::Key { "IsMaximized" });
		writer.Emit(Token::Bool { true });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "PresentationSettings" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "InternalResolution" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Width" });
		writer.Emit(Token::Int { 640 });
		writer.Emit(Token::Key { "Height" });
		writer.Emit(Token::Int { 360 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::Key { "UseFixedRenderResolution" });
		writer.Emit(Token::Bool { true });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeLaunchSettingsSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());

		const auto& runtimeLaunchSettings = result.GetValue();
		ASSERT_EQ(runtimeLaunchSettings.WindowSettings.StartupWidth, 1280);
		ASSERT_EQ(runtimeLaunchSettings.WindowSettings.StartupHeight, 720);
		ASSERT_EQ(runtimeLaunchSettings.WindowSettings.IsResizeable, false);
		ASSERT_EQ(runtimeLaunchSettings.WindowSettings.IsMaximized, true);
		ASSERT_EQ(runtimeLaunchSettings.PresentationSettings.InternalResolution.Width, 640);
		ASSERT_EQ(runtimeLaunchSettings.PresentationSettings.InternalResolution.Height, 360);
		ASSERT_EQ(runtimeLaunchSettings.PresentationSettings.UseFixedRenderResolution, true);
	}

	TEST(RuntimeLaunchSettingsSchema, Deserialize_DefaultsFields_When_AllPayloadFieldsAreMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeLaunchSettingsSchema::Deserialize(reader);

		// Assert.
		ASSERT_TRUE(result.HasValue());
		ASSERT_EQ(result.GetValue().WindowSettings.StartupWidth, 1920);
		ASSERT_EQ(result.GetValue().WindowSettings.StartupHeight, 1080);
		ASSERT_EQ(result.GetValue().WindowSettings.IsResizeable, true);
		ASSERT_EQ(result.GetValue().WindowSettings.IsMaximized, false);
		ASSERT_EQ(result.GetValue().PresentationSettings.InternalResolution.Width, 1920);
		ASSERT_EQ(result.GetValue().PresentationSettings.InternalResolution.Height, 1080);
		ASSERT_EQ(result.GetValue().PresentationSettings.UseFixedRenderResolution, false);
	}

	TEST(RuntimeLaunchSettingsSchema, Deserialize_Fails_When_RuntimeLaunchSettingsHeaderIsMissing)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::Int { 1 });
		DomTokenStreamReader reader(document);

		// Act.
		const auto result = RuntimeLaunchSettingsSchema::Deserialize(reader);

		// Assert.
		ASSERT_FALSE(result.HasValue());
	}
}
