#include "pch.h"

#include <stdexcept>
#include <utility>

#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Persistence/LmlRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Serialization/Codecs/LmlDomCodec.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Schemas/RuntimeLaunchSettingsSchema.h>

namespace Ludus::Engine::Persistence
{
	void LmlRuntimeLaunchSettingsPersistence::Save(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings, const std::filesystem::path& path)
	{
		Ludus::Engine::Serialization::Core::DomDocument document;
		Ludus::Engine::Serialization::Core::DomTokenStreamWriter writer { document };
		Ludus::Engine::Serialization::Schemas::RuntimeLaunchSettingsSchema::Serialize(writer, runtimeLaunchSettings);

		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		const auto text = codec.Encode(*document.GetRoot());
		Ludus::Engine::FileSystem::WriteAllText(path, text);
	}

	Ludus::Engine::Runtime::RuntimeLaunchSettings LmlRuntimeLaunchSettingsPersistence::Load(const std::filesystem::path& path)
	{
		const auto text = Ludus::Engine::FileSystem::ReadAllText(path);
		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		auto node = codec.Decode(text);

		Ludus::Engine::Serialization::Core::DomDocument document;
		document.SetRoot(std::move(node));

		Ludus::Engine::Serialization::Core::DomTokenStreamReader reader { document };
		auto result = Ludus::Engine::Serialization::Schemas::RuntimeLaunchSettingsSchema::Deserialize(reader);
		if (!result.HasValue())
		{
			throw std::runtime_error(result.GetError().what());
		}

		return std::move(result.GetValue());
	}
}
