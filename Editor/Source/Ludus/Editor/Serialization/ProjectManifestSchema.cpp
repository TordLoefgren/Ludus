#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Serialization/ProjectManifestSchema.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Editor::Serialization::Schemas
{
	using Token = Ludus::Engine::Serialization::Core::Token;

	void ProjectManifestSchema::Serialize(ITokenStreamWriter& writer, const ProjectManifest& projectManifest)
	{
		writer.Emit(Token::StartObject { });

		writer.Emit(Token::Key { "Version" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "Major" });
		writer.Emit(Token::Uint { projectManifest.Version.Major });
		writer.Emit(Token::Key { "Minor" });
		writer.Emit(Token::Uint { projectManifest.Version.Minor });
		writer.Emit(Token::Key { "Patch" });
		writer.Emit(Token::Uint { projectManifest.Version.Patch });
		writer.Emit(Token::EndObject { });

		writer.Emit(Token::Key { "ProjectRoot" });
		const auto projectRoot = Ludus::Engine::FileSystem::ToPortablePathString(projectManifest.ProjectRoot);
		writer.Emit(Token::String { projectRoot });

		writer.Emit(Token::Key { "RuntimeManifestPath" });
		const auto runtimeManifestPath = Ludus::Engine::FileSystem::ToPortablePathString(projectManifest.RuntimeManifestPath);
		writer.Emit(Token::String { runtimeManifestPath });

		writer.Emit(Token::EndObject { });
	}

	Ludus::Engine::Core::Expected<ProjectManifest, SerializationException> ProjectManifestSchema::Deserialize(ITokenStreamReader& reader)
	{
		ProjectManifest projectManifest;

		try
		{
			bool hasVersion = false;
			bool hasProjectRoot = false;
			bool hasRuntimeManifestPath = false;

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
							projectManifest.Version.Major = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
							hasMajor = true;
							return;
						}
						if (versionKey == "Minor")
						{
							projectManifest.Version.Minor = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
							hasMinor = true;
							return;
						}
						if (versionKey == "Patch")
						{
							projectManifest.Version.Patch = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
							hasPatch = true;
							return;
						}

						Ludus::Engine::Serialization::Core::SkipValue(reader);
					});

					if (!hasMajor || !hasMinor || !hasPatch)
					{
						throw SerializationException("ProjectManifest version is incomplete.");
					}

					hasVersion = true;
					return;
				}
				if (key == "RuntimeManifestPath")
				{
					projectManifest.RuntimeManifestPath = std::filesystem::path(
						std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
					);
					hasRuntimeManifestPath = true;
					return;
				}
				if (key == "ProjectRoot")
				{
					projectManifest.ProjectRoot = std::filesystem::path(
						std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
					);
					hasProjectRoot = true;
					return;
				}

				Ludus::Engine::Serialization::Core::SkipValue(reader);
			});

			if (!hasVersion)
			{
				throw SerializationException("ProjectManifest version not found.");
			}

			if (!hasRuntimeManifestPath)
			{
				throw SerializationException("ProjectManifest runtime manifest path not found.");
			}

			if (!hasProjectRoot)
			{
				throw SerializationException("ProjectManifest project root not found.");
			}

			return projectManifest;
		}
		catch (const SerializationException& ex)
		{
			const auto error = Ludus::Engine::Serialization::Core::WithContext(
				ex, "ProjectManifestSchema::Deserialize"
			);

			return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
		}
	}
}
