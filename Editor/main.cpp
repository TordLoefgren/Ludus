#include <Ludus/Editor/Core/EditorApplicationBuilder.h>

int main()
{
	auto application = Ludus::Editor::Core::EditorApplicationBuilder::Create()
		.AddDefaultEngine()
		.AddEditorPanels()
		.AddEditorViewport("Viewport")
		.Build();

	application->Run();
}
