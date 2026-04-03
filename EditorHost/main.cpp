#include <Ludus/Editor/Core/EditorHostBuilder.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>

int main(int argc, char* argv[])
{
	auto host = Ludus::Editor::Core::EditorHostBuilder::Create()
		.WithStartupOptions(Ludus::Editor::Core::EditorStartupOptions::FromCommandLineArgs(argc, argv))
		.UseEditorHostDefaults()
		.UseEditor()
		.Build();

	host->Run();
}
