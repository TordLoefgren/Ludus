#pragma once

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Project = Ludus::Engine::Core::Project;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;

	struct ProjectSchema
	{
		static void Serialize(ITokenStreamWriter& writer, const Project& project);
		static Ludus::Engine::Core::Expected<Project, SerializationException> Deserialize(ITokenStreamReader& reader);
	};
}
