#include "pch.h"

#include <Ludus/Editor/Core/EditorApplicationBuilder.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>

int main()
{
	auto editorConfiguration = Ludus::Editor::Core::EditorConfiguration::Default();
	auto application = Ludus::Editor::Core::EditorApplicationBuilder::Create()
		.AddDefaultEngine()
		.WithEditorConfiguration(editorConfiguration)
		.AddEditorSystem()
		.Build();

	application->Run();
}
