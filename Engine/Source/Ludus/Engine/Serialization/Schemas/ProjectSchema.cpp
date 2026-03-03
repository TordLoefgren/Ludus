#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/ProjectSchema.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Token = Ludus::Engine::Serialization::Core::Token;
	using ProjectSceneReference = Ludus::Engine::Core::ProjectSceneReference;
	using ProjectScriptReference = Ludus::Engine::Core::ProjectScriptReference;

	void ProjectSchema::Serialize(ITokenStreamWriter& writer, const Project& project)
	{
		writer.Emit(Token::StartObject { });

		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Uint { project.Version.Major });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Uint { project.Version.Minor });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Uint { project.Version.Patch });
		writer.Emit(Token::EndObject { });

		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });

		for (const auto& scene : project.Scenes)
		{
			writer.Emit(Token::StartObject { });
			writer.Emit(Token::Key { "Handle" });
			writer.Emit(Token::Uint { scene.Handle });
			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { scene.Name });
			writer.Emit(Token::Key { "Path" });
			writer.Emit(Token::String { scene.Path.generic_string().c_str() });
			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });

		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });

		for (const auto& script : project.Scripts)
		{
			writer.Emit(Token::StartObject { });
			writer.Emit(Token::Key { "Handle" });
			writer.Emit(Token::Uint { script.Handle });
			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { script.Name });
			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });

		writer.Emit(Token::Key { "ActiveSceneHandle" });
		writer.Emit(Token::Uint { project.ActiveSceneHandle });
		writer.Emit(Token::EndObject { });
	}

	Ludus::Engine::Core::Expected<Project, SerializationException> ProjectSchema::Deserialize(ITokenStreamReader& reader)
	{
		Project project;

		try
		{
			bool hasVersion = false;

			Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
			{
				if (key == "Version")
				{
					bool hasMajor = false;
					bool hasMinor = false;
					bool hasPatch = false;

					Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view versionKey)
					{
						if (versionKey == "Major")
						{
							project.Version.Major = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
							hasMajor = true;
							return;
						}
						if (versionKey == "Minor")
						{
							project.Version.Minor = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
							hasMinor = true;
							return;
						}
						if (versionKey == "Patch")
						{
							project.Version.Patch = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
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
						ProjectSceneReference scene;
						bool hasHandle = false;
						bool hasName = false;
						bool hasPath = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view sceneKey)
						{
							if (sceneKey == "Handle")
							{
								scene.Handle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
								hasHandle = true;
								return;
							}
							if (sceneKey == "Name")
							{
								scene.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
								hasName = true;
								return;
							}
							if (sceneKey == "Path")
							{
								scene.Path = std::filesystem::path(
									std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
								);
								hasPath = true;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasHandle || !hasName || !hasPath)
						{
							throw SerializationException("Project scene entry is incomplete.");
						}

						project.Scenes.emplace_back(std::move(scene));
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}
				if (key == "ActiveSceneHandle")
				{
					project.ActiveSceneHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
					return;
				}
				if (key == "Scripts")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						ProjectScriptReference script;
						bool hasHandle = false;
						bool hasName = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view scriptKey)
						{
							if (scriptKey == "Handle")
							{
								script.Handle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
								hasHandle = true;
								return;
							}
							if (scriptKey == "Name")
							{
								script.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
								hasName = true;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasHandle || !hasName)
						{
							throw SerializationException("Project script entry is incomplete.");
						}

						project.Scripts.emplace_back(std::move(script));
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
			const auto error = Ludus::Engine::Serialization::Core::WithContext(
				ex, "ProjectSchema::Deserialize"
			);

			return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
		}
	}
}
