#pragma once

#include <filesystem>

#include <Ludus/Editor/Persistence/IEditorPreferencesPersistence.h>

namespace Ludus::Editor::Persistence
{
	class LmlEditorPreferencesPersistence final : public IEditorPreferencesPersistence
	{
	public:
		LmlEditorPreferencesPersistence() = default;

		void Save(const Ludus::Editor::Core::EditorPreferences& editorPreferences, const std::filesystem::path& path) const override;
		Ludus::Editor::Core::EditorPreferences Load(const std::filesystem::path& path) const override;
	};
}
