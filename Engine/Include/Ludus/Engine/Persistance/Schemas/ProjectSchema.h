#pragma once

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Schemas/SceneSchema.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using Project = Ludus::Engine::Core::Project;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

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

		inline static Ludus::Engine::Core::Expected<Project, PersistanceException> Deserialize(ITokenStreamReader& reader)
		{
			Project project;

			try
			{
				bool hasVersion = false;

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "Version")
						{
							bool hasMajor = false;
							bool hasMinor = false;
							bool hasPatch = false;

							Ludus::Engine::Persistance::Serialization::ReadObject(reader,
								[&](std::string_view versionKey)
								{
									if (versionKey == "Major")
									{
										project.Version.Major = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
										hasMajor = true;
										return;
									}
									if (versionKey == "Minor")
									{
										project.Version.Minor = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
										hasMinor = true;
										return;
									}
									if (versionKey == "Patch")
									{
										project.Version.Patch = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
										hasPatch = true;
										return;
									}

									Ludus::Engine::Persistance::Serialization::SkipValue(reader);
								});

							if (!hasMajor || !hasMinor || !hasPatch)
							{
								throw PersistanceException("Project version is incomplete.");
							}

							hasVersion = true;
							return;
						}
						if (key == "Scenes")
						{
							Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::StartArray>(reader);

							while (!Ludus::Engine::Persistance::Serialization::Is<Token::EndArray>(reader.Peek()))
							{
								auto sceneResult = SceneSchema::Deserialize(reader);
								if (!sceneResult.HasValue())
								{
									throw PersistanceException(sceneResult.GetError().what());
								}

								project.Scenes.emplace_back(std::move(sceneResult.GetValue()));
							}

							Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::EndArray>(reader);
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasVersion)
				{
					throw PersistanceException("Project version not found.");
				}

				return project;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Project, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
