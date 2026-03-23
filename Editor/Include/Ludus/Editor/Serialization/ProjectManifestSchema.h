#pragma once

#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Editor::Serialization::Schemas
{
	using ProjectManifest = Ludus::Editor::Core::ProjectManifest;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;

	struct ProjectManifestSchema
	{
		static void Serialize(ITokenStreamWriter& writer, const ProjectManifest& manifest);
		static Ludus::Engine::Core::Expected<ProjectManifest, SerializationException> Deserialize(ITokenStreamReader& reader);
	};
}
