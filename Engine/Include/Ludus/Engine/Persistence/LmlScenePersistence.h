#pragma once

#include <filesystem>
#include <stdexcept>
#include <utility>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Serialization/Codecs/LmlDomCodec.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Schemas/SceneSchema.h>

namespace Ludus::Engine::Persistence
{
	class LmlScenePersistence final : public IScenePersistence
	{
	public:
		LmlScenePersistence() = default;

		void Save(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path) override
		{
			Ludus::Engine::Serialization::Core::DomDocument document;
			Ludus::Engine::Serialization::Core::DomTokenStreamWriter writer { document };
			Ludus::Engine::Serialization::Schemas::SceneSchema::Serialize(writer, scene);

			Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
			const auto text = codec.Encode(*document.GetRoot());
			Ludus::Engine::FileSystem::WriteAllText(path, text);
		}

		Ludus::Engine::Core::Scene Load(const std::filesystem::path& path) override
		{
			const auto text = Ludus::Engine::FileSystem::ReadAllText(path);
			Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
			auto node = codec.Decode(text);

			Ludus::Engine::Serialization::Core::DomDocument document;
			document.SetRoot(std::move(node));
			Ludus::Engine::Serialization::Core::DomTokenStreamReader reader { document };

			auto result = Ludus::Engine::Serialization::Schemas::SceneSchema::Deserialize(reader);
			if (!result.HasValue())
			{
				throw std::runtime_error(result.GetError().what());
			}

			return std::move(result.GetValue());
		}
	};
}
