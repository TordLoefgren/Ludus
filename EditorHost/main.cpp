#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorHostBuilder.h>

int main()
{
	auto editorConfiguration = Ludus::Editor::Core::EditorConfiguration::Default();
	auto host = Ludus::Editor::Core::EditorHostBuilder::Create()
		.AddDefaultEngine()
		.WithEditorConfiguration(editorConfiguration)
		.AddEditorSystem()
		.Build();

	host->Run();
}
