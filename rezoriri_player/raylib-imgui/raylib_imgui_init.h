#ifndef RAYLIB_IMGUI_INIT_H_
#define RAYLIB_IMGUI_INIT_H_

struct SRaylibImGuiInit
{
	SRaylibImGuiInit(const char* fontFilePath, int fontSize);
	~SRaylibImGuiInit();

	bool hasBeenInitialised = false;
};
#endif // !RAYLIB_IMGUI_INIT_H_
