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

		writer.Emit(Token::Key { "SchemaRevision" });
		writer.Emit(Token::Uint { projectManifest.SchemaRevision });

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
			bool hasSchemaRevision = false;
			bool hasProjectRoot = false;
			bool hasRuntimeManifestPath = false;

			Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
			{
				if (key == "SchemaRevision")
				{
					projectManifest.SchemaRevision = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
					if (projectManifest.SchemaRevision != ProjectManifest::CurrentSchemaRevision)
					{
						throw SerializationException("ProjectManifest schema revision is not supported.");
					}

					hasSchemaRevision = true;
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

			if (!hasSchemaRevision)
			{
				throw SerializationException("ProjectManifest schema revision not found.");
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
