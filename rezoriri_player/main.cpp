
#include <locale.h>

#include "raylib_main_window.h"
#include "raylib-imgui/raylib_imgui_init.h"
#include "rezoriri.h"


#if defined(_WIN32) && defined(_MSVC_LANG) && !defined(_DEBUG)
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

int main()
{
	::setlocale(LC_ALL, ".utf8");

	if (!rezoriri::InitialiseSetting()) return -1;

	CRaylibMainWindow mainWindow("rezoriri player", rezoriri::GetFontFilePath().data());
	if (!mainWindow.hasValidFont()) return -1;

	SRaylibImGuiInit rayLibInit(rezoriri::GetFontFilePath().data(), 24);

	mainWindow.display();

	return 0;
}