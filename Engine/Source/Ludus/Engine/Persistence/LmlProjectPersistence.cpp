#include "pch.h"

#include <stdexcept>
#include <utility>

#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/LmlProjectPersistence.h>
#include <Ludus/Engine/Serialization/Codecs/LmlDomCodec.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Schemas/ProjectSchema.h>

namespace Ludus::Engine::Persistence
{
	void LmlProjectPersistence::Save(const Ludus::Engine::Core::Project& project, const std::filesystem::path& path)
	{
		Ludus::Engine::Serialization::Core::DomDocument document;
		Ludus::Engine::Serialization::Core::DomTokenStreamWriter writer { document };
		Ludus::Engine::Serialization::Schemas::ProjectSchema::Serialize(writer, project);

		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		const auto text = codec.Encode(*document.GetRoot());
		Ludus::Engine::FileSystem::WriteAllText(path, text);
	}

	Ludus::Engine::Core::Project LmlProjectPersistence::Load(const std::filesystem::path& path)
	{
		const auto text = Ludus::Engine::FileSystem::ReadAllText(path);
		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		auto node = codec.Decode(text);

		Ludus::Engine::Serialization::Core::DomDocument document;
		document.SetRoot(std::move(node));

		Ludus::Engine::Serialization::Core::DomTokenStreamReader reader { document };
		auto result = Ludus::Engine::Serialization::Schemas::ProjectSchema::Deserialize(reader);
		if (!result.HasValue())
		{
			throw std::runtime_error(result.GetError().what());
		}

		return std::move(result.GetValue());
	}
}
