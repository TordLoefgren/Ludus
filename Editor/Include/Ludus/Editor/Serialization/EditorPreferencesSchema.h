#pragma once

#include <Ludus/Editor/Core/EditorPreferences.h>
#include <Ludus/Editor/Core/RecentlyOpenedProject.h>
#include <Ludus/Engine/Core/Enums/EnumTraits.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Editor::Serialization::Schemas
{
	using EditorPreferences = Ludus::Editor::Core::EditorPreferences;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct EditorPreferencesSchema
	{
		static void Serialize(ITokenStreamWriter& writer, const EditorPreferences& editorPreferences)
		{
			writer.Emit(Token::StartObject { });

			const std::string activeThemeId = Ludus::Engine::Core::Enums::GetDisplayName(editorPreferences.ActiveThemeId);
			writer.Emit(Token::Key { "ActiveThemeId" });
			writer.Emit(Token::String { activeThemeId });

			writer.Emit(Token::Key { "RecentlyOpenedProjects" });
			writer.Emit(Token::StartArray { });

			for (const auto& recentlyOpenedProject : editorPreferences.RecentlyOpenedProjects)
			{
				writer.Emit(Token::StartObject { });

				writer.Emit(Token::Key { "DisplayName" });
				writer.Emit(Token::String { recentlyOpenedProject.DisplayName });

				writer.Emit(Token::Key { "Path" });
				const auto recentlyOpenedProjectPath = Ludus::Engine::FileSystem::ToPortablePathString(recentlyOpenedProject.Path);
				writer.Emit(Token::String { recentlyOpenedProjectPath });

				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::EndArray { });

			writer.Emit(Token::EndObject { });
		}

		static Ludus::Engine::Core::Expected<EditorPreferences, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			EditorPreferences editorPreferences;

			try
			{
				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "ActiveThemeId")
					{
						std::string themeIdValue = std::string(
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data
						);
						Ludus::UI::Theme::ThemeId parsed;
						if (Ludus::UI::Theme::TryParse(themeIdValue, parsed))
						{
							editorPreferences.ActiveThemeId = parsed;
						}
						return;
					}
					if (key == "RecentlyOpenedProjects")
					{
						Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

						while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
						{
							Ludus::Editor::Core::RecentlyOpenedProject recentlyOpenedProject;
							bool hasDisplayName = false;
							bool hasPath = false;

							Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view recentlyOpenedProjectKey)
							{
								if (recentlyOpenedProjectKey == "DisplayName")
								{
									recentlyOpenedProject.DisplayName = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
									hasDisplayName = true;
									return;
								}
								if (recentlyOpenedProjectKey == "Path")
								{
									recentlyOpenedProject.Path = std::filesystem::path(
										std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
									);
									hasPath = true;
									return;
								}

								Ludus::Engine::Serialization::Core::SkipValue(reader);
							});

							if (!hasDisplayName || !hasPath)
							{
								throw SerializationException("RecentlyOpenedProject entry is incomplete.");
							}

							editorPreferences.RecentlyOpenedProjects.emplace_back(std::move(recentlyOpenedProject));
						}

						Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
						return;
					}
				});

				return editorPreferences;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "ProjectManifestSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
