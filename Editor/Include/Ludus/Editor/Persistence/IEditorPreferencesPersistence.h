#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/EditorPreferences.h>

namespace Ludus::Editor::Persistence
{
	class IEditorPreferencesPersistence
	{
	public:
		virtual ~IEditorPreferencesPersistence() = default;

		virtual void Save(const Ludus::Editor::Core::EditorPreferences& editorPreferences, const std::filesystem::path& path) const = 0;
		virtual Ludus::Editor::Core::EditorPreferences Load(const std::filesystem::path& path) const = 0;
	};
}
