#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorHostBuilder.h>

int main()
{
	auto host = Ludus::Editor::Core::EditorHostBuilder::Create()
		.AddDefaultEngine()
		.AddEditorSystem()
		.WithEditorConfiguration(Ludus::Editor::Core::EditorConfiguration::Default())
		.Build();

	host->Run();
}
