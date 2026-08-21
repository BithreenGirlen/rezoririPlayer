

#include "raylib_imgui_init.h"

#include <rlImGui.h>
#include <imgui.h>

SRaylibImGuiInit::SRaylibImGuiInit(const char* fontFilePath, int fontSize)
{
	/* 
	* In my opinion, the argument to specify theme is useless because it can be easily changed after setup. 
	* On the other hand, lack of an argument as to font style and forcing of default one are making the situation bothersome.
	*/
	::rlImGuiSetup(false);

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.875f;

	if (fontFilePath != nullptr)
	{
		const auto& fontAtlas = io.Fonts;
		/* ::rlImGuiSetup always uses default font, so clear it first. */
		fontAtlas->Clear();

		const ImWchar* glyph = fontAtlas->GetGlyphRangesChineseFull();
		fontAtlas->AddFontFromFileTTF(fontFilePath, fontSize, nullptr, glyph);
	}

	ImGuiStyle& style = ImGui::GetStyle();
	style._NextFrameFontSizeBase = fontSize;
}

SRaylibImGuiInit::~SRaylibImGuiInit()
{
	::rlImGuiShutdown();
}
