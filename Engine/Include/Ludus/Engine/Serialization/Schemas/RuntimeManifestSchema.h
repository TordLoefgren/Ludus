#pragma once

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;

	struct RuntimeManifestSchema
	{
		static void Serialize(ITokenStreamWriter& writer, const RuntimeManifest& data);
		static Ludus::Engine::Core::Expected<RuntimeManifest, SerializationException> Deserialize(ITokenStreamReader& reader);
	};
}
