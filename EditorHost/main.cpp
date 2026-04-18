#include <Ludus/Editor/Core/EditorLauncher.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>

int main(int argc, char* argv[])
{
	return Ludus::Editor::Core::EditorLauncher::Run(
		Ludus::Editor::Core::EditorStartupOptions::FromCommandLineArgs(argc, argv)
	);
}
