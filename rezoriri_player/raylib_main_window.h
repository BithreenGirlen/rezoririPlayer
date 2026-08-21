#ifndef RAYLIB_MAIN_WINDOW_H_
#define RAYLIB_MAIN_WINDOW_H_

#include "raylib-spine/raylib_spine_player.h"
#include "raylib_handle.h"
#include "rezoriri_scene_crafter.h"
#include "raylib_text_drawer.h"
#include "windows/mf_media_player.h" /* Raylib does not support decoding AAC */
#include "raylib_clock.h"

#include <raylib.h>

class CRaylibMainWindow
{
public:
	CRaylibMainWindow(const char* windowTitle, const char* fontFilePath);
	~CRaylibMainWindow();

	void display();

	bool hasValidFont() const noexcept;
private:
	struct MouseState
	{
		bool wasLeftPressed = false;
		bool wasLeftDragged = false;
		/// @brief 左釦押下を要する操作を行ったか
		bool wasLeftCombined = false;
		/// @brief 右釦押下を要する操作を行ったか
		bool wasRightCombined = false;
	};

	struct WindowStyle
	{
		bool isBorderless = false;
		bool toFitToMonitorHeight = false;
		bool isZoomReversed = false;
	};

	struct WindowState
	{
		bool isUnderWindowMove = false;
		bool toShowPopupMenu = false;
		bool toShowSettingDialogue = false;
		bool toUpdateSettingFontParameters = false;

		/// @brief 直前のメニュー欄表示処理時のメニュー欄の高さ
		int lastMenuBarHeight = 0;
	};

	struct FontStyle
	{
		float sceneFontSize = CRaylibTextDrawer::kDefaultFillSize;
		int sceneFontThickness = CRaylibTextDrawer::kDefaultOutLineSize;
		float sceneFontSpacing = CRaylibTextDrawer::kDefaultSpacing;

		/* Raylibでは太字や斜体の指定ができない */
	};

	MouseState m_mouseState;
	WindowStyle m_windowStyle;
	WindowState m_windowState;
	FontStyle m_fontStyle;

	RaylibFontHandle m_font;
	CRaylibTextDrawer m_helpTextDrawer;

	std::vector<std::string> m_sceneSpineFolderPaths;
	size_t m_nSceneSpineFolderPathIndex = 0;

	CRezoririSceneCrafter m_sceneCrafter;
	CRaylibSpinePlayer m_sceneSpinePlayer;
	RaylibRenderTextureHandle m_spineRenderTexture;

	CMfMediaPlayer m_voicePlayer;
	CRaylibTextDrawer m_sceneTextDrawer;

	CRaylibClock m_spineClock;
	CRaylibClock m_textClock;

	/// @brief キーボード入力処理
	void handleKeyboardEvents();
	/// @brief マウス入力処理
	void handleMouseEvents();

	/// @brief 字体更新
	void setupFont(const char* fontFilePath);
	void updateSceneText();
	void updateHelpText();

	/// @brief フォルダ選択
	void menuOnOpenFolder();

	/// @brief 窓寸法変更
	void resizeWindow();
	/// @brief 現在のモニタの原点位置に窓を合わせる
	void alignWindowToTheTopLeftOfMonitor();

	/// @brief Spine寸法初期化
	void setSpinePlayerSize();
	/// @brief Spine拡縮初期化
	void resetSpinePlayerScale();

	void imguiMenuBar();
	void imguiPopupMenu();
	void imguiSettingDialogue();

	/// @brief ファイル読み込み
	bool loadScenario(const std::string& sceneSpineFolderPath);

	/// @brief 場面移行
	void shiftScene(bool forward);
	/// @brief 場面更新
	void updateScene();
	/// @brief 自動進行
	void autoTexting();
	/// @brief 文章表示経過時間確認
	void checkTextClock();
};
#endif // !RAYLIB_MAIN_WINDOW_H_
