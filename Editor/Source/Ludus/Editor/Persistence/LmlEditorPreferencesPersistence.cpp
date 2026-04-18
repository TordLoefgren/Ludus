#include "pch.h"

#include <stdexcept>
#include <utility>

#include <Ludus/Editor/Persistence/LmlEditorPreferencesPersistence.h>
#include <Ludus/Editor/Serialization/EditorPreferencesSchema.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Serialization/Codecs/LmlDomCodec.h>
#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>

namespace Ludus::Editor::Persistence
{
	void LmlEditorPreferencesPersistence::Save(const Ludus::Editor::Core::EditorPreferences& editorPreferences, const std::filesystem::path& path) const
	{
		Ludus::Engine::Serialization::Core::DomDocument document;
		Ludus::Engine::Serialization::Core::DomTokenStreamWriter writer { document };
		Ludus::Editor::Serialization::Schemas::EditorPreferencesSchema::Serialize(writer, editorPreferences);

		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		const auto text = codec.Encode(*document.GetRoot());
		Ludus::Engine::FileSystem::WriteAllText(path, text);
	}

	Ludus::Editor::Core::EditorPreferences LmlEditorPreferencesPersistence::Load(const std::filesystem::path& path) const
	{
		const auto text = Ludus::Engine::FileSystem::ReadAllText(path);
		Ludus::Engine::Serialization::Codecs::LmlDomCodec codec;
		auto node = codec.Decode(text);

		Ludus::Engine::Serialization::Core::DomDocument document;
		document.SetRoot(std::move(node));

		Ludus::Engine::Serialization::Core::DomTokenStreamReader reader { document };
		auto result = Ludus::Editor::Serialization::Schemas::EditorPreferencesSchema::Deserialize(reader);
		if (!result.HasValue())
		{
			throw std::runtime_error(result.GetError().what());
		}

		return std::move(result.GetValue());
	}
}
