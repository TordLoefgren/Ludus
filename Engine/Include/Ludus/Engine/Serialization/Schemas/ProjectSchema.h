#pragma once

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/SceneSchema.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Project = Ludus::Engine::Core::Project;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct ProjectSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Project& project)
		{
			writer.Emit(Token::StartObject { });

			{
				writer.Emit(Token::Key { "Version" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "Major" });
				writer.Emit(Token::Uint32 { project.Version.Major });
				writer.Emit(Token::Key { "Minor" });
				writer.Emit(Token::Uint32 { project.Version.Minor });
				writer.Emit(Token::Key { "Patch" });
				writer.Emit(Token::Uint32 { project.Version.Patch });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "Scenes" });
			writer.Emit(Token::StartArray { });

			for (const auto& scene : project.Scenes)
			{
				SceneSchema::Serialize(writer, scene);
			}

			writer.Emit(Token::EndArray { });
			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Project, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			Project project;

			try
			{
				bool hasVersion = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "Version")
						{
							bool hasMajor = false;
							bool hasMinor = false;
							bool hasPatch = false;

							Ludus::Engine::Serialization::Core::ReadObject(reader,
								[&](std::string_view versionKey)
								{
									if (versionKey == "Major")
									{
										project.Version.Major = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
										hasMajor = true;
										return;
									}
									if (versionKey == "Minor")
									{
										project.Version.Minor = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
										hasMinor = true;
										return;
									}
									if (versionKey == "Patch")
									{
										project.Version.Patch = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Uint32>(reader).Data;
										hasPatch = true;
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});

							if (!hasMajor || !hasMinor || !hasPatch)
							{
								throw SerializationException("Project version is incomplete.");
							}

							hasVersion = true;
							return;
						}
						if (key == "Scenes")
						{
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

							while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
							{
								auto sceneResult = SceneSchema::Deserialize(reader);
								if (!sceneResult.HasValue())
								{
									throw SerializationException(sceneResult.GetError().what());
								}

								project.Scenes.emplace_back(std::move(sceneResult.GetValue()));
							}

							Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

				if (!hasVersion)
				{
					throw SerializationException("Project version not found.");
				}

				return project;
			}
			catch (const SerializationException& ex)
			{
				return Ludus::Engine::Core::Expected<Project, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(ex)
				);
			}
		}
	};
}
