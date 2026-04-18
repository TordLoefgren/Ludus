#pragma once

#include <memory>

#include <Ludus/Editor/Persistence/IEditorPreferencesPersistence.h>
#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/LmlEditorPreferencesPersistence.h>
#include <Ludus/Editor/Persistence/LmlProjectManifestPersistence.h>

namespace Ludus::Editor::Persistence
{
	class EditorPersistence
	{
	private:
		std::unique_ptr<IEditorPreferencesPersistence> m_EditorPreferences;
		std::unique_ptr<IProjectManifestPersistence> m_ProjectManifest;

		EditorPersistence(
			std::unique_ptr<IEditorPreferencesPersistence> editorPreferences,
			std::unique_ptr<IProjectManifestPersistence> projectManifest
		) :
			m_EditorPreferences(std::move(editorPreferences)),
			m_ProjectManifest(std::move(projectManifest))
		{}

	public:
		static EditorPersistence DefaultText()
		{
			return {
				std::make_unique<LmlEditorPreferencesPersistence>(),
				std::make_unique<LmlProjectManifestPersistence>()
			};
		}

		const IEditorPreferencesPersistence& EditorPreferences() const { return *m_EditorPreferences; }
		const IProjectManifestPersistence& ProjectManifest() const { return *m_ProjectManifest; }
	};
}
