

#include "raylib_main_window.h"
#include "raylib_input.h"
#include "raylib-spine/spine_file_verifier.h"
#include "raylib_filesystem_utility.h"
#include "raylib_render_target_scope.h"
#include "path_utility.h"
#include "rezoriri.h"

#include "windows/win_dialogue.h" /* Raylib does not support native file-select-dialogue. */
#include "../rezoriri/win_text.h" /* Raylib does not support character encoding. */

#include <rlImGui.h>
#include <imgui.h>


CRaylibMainWindow::CRaylibMainWindow(const char* windowTitle, const char* fontFilePath)
{
	::SetConfigFlags(FLAG_VSYNC_HINT);
	::InitWindow(1080, 720, windowTitle);

	static constexpr auto SpineTextureLoaderCallback = [](void* pUserDatum, const char* textureFilePath, size_t filePathLength, void* pOutImage)
		-> void
		{
			Image* pImage = static_cast<Image*>(pOutImage);
			if (pImage == nullptr)return;

			*pImage = ::LoadImage(textureFilePath);
			if (!::IsImageValid(*pImage))return;

			/*
			* The size of usual textures is 2048x2048, and that of mask image is 16x16, 64x64, or 128x128.
			* The latter would be preferable to be transparent.
			*/
			if (pImage->width < 512 && pImage->height < 512)
			{
				::ImageClearBackground(pImage, Color(0, 0, 0, 0));
			}
		};
	m_sceneSpinePlayer.setTextureLoadCallback(SpineTextureLoaderCallback, this);
	m_sceneSpinePlayer.enableConversionToPmaOnLoading(true);

	setupFont(fontFilePath);
	if (!m_font.isHandleValid())
	{
		win_dialogue::ShowErrorMessageBox("Error", "Failed to load font file");
	}
	updateHelpText();
	m_helpTextDrawer.setTextVisibility(false);
}

CRaylibMainWindow::~CRaylibMainWindow()
{
	::CloseWindow();
}

void CRaylibMainWindow::display()
{
	m_spineClock.restart();
	m_textClock.restart();

	while (!::WindowShouldClose())
	{
		handleKeyboardEvents();
		handleMouseEvents();

		m_sceneSpinePlayer.update(m_spineClock.getElapsedTime());
		m_spineClock.restart();

		::BeginDrawing();

			::ClearBackground(RAYWHITE);

			if (m_spineRenderTexture.isHandleValid())
			{
				{
					CRaylibRenderTargetScope renderTargetScope(m_spineRenderTexture.get());
					m_sceneSpinePlayer.redraw(&m_spineRenderTexture.get());
				}

				const Texture& texture = m_spineRenderTexture.get().texture;
				const Rectangle srcRec =
				{
					.x = 0.f,
					.y = 0.f,
					.width = static_cast<float>(texture.width),
					.height = -static_cast<float>(texture.height)
				};
				const Rectangle destRec =
				{
					.x = 0.f,
					.y = static_cast<float>(m_windowState.lastMenuBarHeight),
					.width = static_cast<float>(::GetScreenWidth()),
					.height = static_cast<float>(::GetScreenHeight())
				};

				::DrawTexturePro(texture, srcRec, destRec, {}, 0.f, WHITE);
			}

			{
				int helpTextHeight = 0;
				m_helpTextDrawer.getTextTextureSize(nullptr, &helpTextHeight);

				int windowHeight = ::GetScreenHeight();

				m_helpTextDrawer.renderTextRenderTexture(m_helpTextDrawer.getFontSpacing(), windowHeight - helpTextHeight - m_helpTextDrawer.getFontSpacing());
			}

			m_sceneTextDrawer.renderTextRenderTexture(m_sceneTextDrawer.getFontSpacing(), m_sceneTextDrawer.getFontSpacing() + static_cast<float>(m_windowState.lastMenuBarHeight));

			::rlImGuiBegin();

			imguiMenuBar();
			imguiPopupMenu();
			imguiSettingDialogue();

			::rlImGuiEnd();

		::EndDrawing();

		checkTextClock();
	}
}

bool CRaylibMainWindow::hasValidFont() const noexcept
{
	return m_font.isHandleValid();
}

void CRaylibMainWindow::handleKeyboardEvents()
{
	if (ImGui::GetIO().WantCaptureKeyboard)return;

	if (raylib_input::KeyC.pressed())
	{
		m_sceneTextDrawer.toggleTextColour();
		updateSceneText();
		m_helpTextDrawer.toggleTextColour();
		updateHelpText();
	}
	else if (raylib_input::KeyH.pressed())
	{
		m_helpTextDrawer.setTextVisibility(!m_helpTextDrawer.isTextVisible());
	}
	else if (raylib_input::KeyT.pressed())
	{
		m_sceneTextDrawer.setTextVisibility(!m_sceneTextDrawer.isTextVisible());
	}
	else if (raylib_input::KeyUp.pressed())
	{
		if (!m_sceneSpineFolderPaths.empty())
		{
			if (--m_nSceneSpineFolderPathIndex >= m_sceneSpineFolderPaths.size())
			{
				m_nSceneSpineFolderPathIndex = m_sceneSpineFolderPaths.size() - 1;
			}

			loadScenario(m_sceneSpineFolderPaths[m_nSceneSpineFolderPathIndex]);
		}
	}
	else if (raylib_input::KeyDown.pressed())
	{
		if (!m_sceneSpineFolderPaths.empty())
		{
			if (++m_nSceneSpineFolderPathIndex >= m_sceneSpineFolderPaths.size())
			{
				m_nSceneSpineFolderPathIndex = 0;
			}

			loadScenario(m_sceneSpineFolderPaths[m_nSceneSpineFolderPathIndex]);
		}
	}
	else if (raylib_input::KeyRight.pressed() || raylib_input::KeyRight.pressedRepeatedly())
	{
		autoTexting();
	}
	else if (raylib_input::KeyLeft.pressed() || raylib_input::KeyLeft.pressedRepeatedly())
	{
		shiftScene(false);
	}
}

void CRaylibMainWindow::handleMouseEvents()
{
	if (ImGui::GetIO().WantCaptureMouse)return;
	if (!m_sceneCrafter.hasScenarioData())return;

	/* ボタン入力 */

	if (raylib_input::MouseM.down())
	{
		resetSpinePlayerScale();
	}
	else if (raylib_input::MouseL.down())
	{
		if (!m_mouseState.wasLeftCombined)
		{
			Vector2 mouseDelta = ::GetMouseDelta();
			if (m_mouseState.wasLeftDragged && ::islessgreater(mouseDelta.x, 0.f) && ::islessgreater(mouseDelta.y, 0.f))
			{
				m_sceneSpinePlayer.addOffset(static_cast<int>(-mouseDelta.x), static_cast<int>(-mouseDelta.y));
			}

			m_mouseState.wasLeftDragged = true;
		}

		m_mouseState.wasLeftPressed = true;
	}
	else if (raylib_input::MouseL.released())
	{
		m_mouseState.wasLeftCombined = false;
		m_mouseState.wasLeftPressed = false;
		m_mouseState.wasLeftDragged = false;
	}
	else if (raylib_input::MouseM.released())
	{
		if (raylib_input::MouseR.down())
		{
			/* Hide/show the border of window. */
			m_windowStyle.isBorderless ^= true;
			if (m_windowStyle.isBorderless)
			{
				int monitorId = ::GetCurrentMonitor();
				Vector2 monitorPosition = ::GetMonitorPosition(monitorId);
				::SetWindowPosition(monitorPosition.x, monitorPosition.y);
				::SetWindowState(FLAG_WINDOW_UNDECORATED);
			}
			else
			{
				::ClearWindowState(FLAG_WINDOW_UNDECORATED);

				/*
				* I would like to move window downwards by the height of title bar though,
				* Raylib does not provide API to retrieve border size.
				*/

				Vector2 windowPosition = ::GetWindowPosition();
				::SetWindowPosition(windowPosition.x, windowPosition.y + 32);
			}

			m_mouseState.wasRightCombined = true;

			resizeWindow();
		}
	}
	else if (raylib_input::MouseR.released())
	{
		if (!m_mouseState.wasRightCombined)
		{
			m_windowState.toShowPopupMenu = true;
		}

		m_mouseState.wasRightCombined = false;
	}

	/* ホイール回転 */

	Vector2 scroll = ::GetMouseWheelMoveV();
	if (::islessgreater(scroll.y, 0.f))
	{
		const float scrollSign = (::isless(scroll.y, 0.f) ^ m_windowStyle.isZoomReversed ? 1.f : -1.f);

		if (raylib_input::MouseL.down())
		{
			static constexpr float kTimeScaleDelta = 0.05f;

			float timeScale = m_sceneSpinePlayer.getTimeScale() + kTimeScaleDelta * scrollSign;
			timeScale = (std::max)(timeScale, 0.f);
			m_sceneSpinePlayer.setTimeScale(timeScale);

			m_mouseState.wasLeftCombined = true;
		}
		else if (raylib_input::MouseR.down())
		{
			shiftScene(scroll.y < 0);

			m_mouseState.wasRightCombined = true;
		}
		else
		{
			static constexpr float kScaleDelta = 0.025f;
			static constexpr float kMinScale = 0.15f;

			float skeletonScale = m_sceneSpinePlayer.getSkeletonScale() + kScaleDelta * scrollSign;
			skeletonScale = (std::max)(kMinScale, skeletonScale);
			m_sceneSpinePlayer.setSkeletonScale(skeletonScale);

			if (!raylib_input::KeyLeftControl.down())
			{
				float canvasScale = m_sceneSpinePlayer.getCanvasScale() + kScaleDelta * scrollSign;
				canvasScale = (std::max)(kMinScale, canvasScale);
				m_sceneSpinePlayer.setCanvasScale(canvasScale);

				resizeWindow();
			}
		}
	}
}

void CRaylibMainWindow::setupFont(const char* fontFilePath)
{
	static constexpr unsigned short s_glyphRanges[] =
	{
		0x0020, 0x00FF, /* Basic Latin + Latin Supplement */
		0x2000, 0x206F, /* General Punctuation */
		0x2190, 0x21FF, /* Arrows */
		0x3000, 0x30FF, /* CJK Symbols and Punctuations, Hiragana, Katakana */
		0x31F0, 0x31FF, /* Katakana Phonetic Extensions */
		0xFF00, 0xFFEF, /* Half-width characters */
		0x4e00, 0x9FAF, /* CJK Ideograms */
	};
	static constexpr size_t s_glyphCount = sizeof(s_glyphRanges) / sizeof(s_glyphRanges[0]);

	static constexpr auto WorkOutGlyphCountSum = []()
		-> size_t
		{
			size_t sum = 0;
			for (size_t i = 0; i < s_glyphCount - 1; i += 2)
			{
				sum += static_cast<size_t>(s_glyphRanges[i + 1] - s_glyphRanges[i]) + 1;
			}

			return sum;
		};
	static constexpr size_t s_codePointCount = WorkOutGlyphCountSum();

	struct CodePoints
	{
		int data[s_codePointCount];
	};
	static constexpr auto SetupCodePoints = []()
		-> CodePoints
		{
			CodePoints codePoints{};

			size_t index = 0;
			for (size_t i = 0; i < s_glyphCount - 1; i += 2)
			{
				const int nStart = s_glyphRanges[i];
				const int nEnd = s_glyphRanges[i + 1];
				for (int code = nStart; code <= nEnd; ++code)
				{
					codePoints.data[index++] = code;
				}
			}

			return codePoints;
		};
	static constexpr CodePoints s_codePoints = SetupCodePoints();

	m_font.reset(::LoadFontEx(fontFilePath, m_fontStyle.sceneFontSize, s_codePoints.data, s_codePointCount));
	::SetTextureFilter(m_font.get().texture, TEXTURE_FILTER_BILINEAR);
	
	m_sceneTextDrawer.setFont(m_font.get());
	m_helpTextDrawer.setFont(m_font.get(), m_sceneTextDrawer.getFontSize() / 2.f, m_sceneTextDrawer.getFontThickness() / 2, m_sceneTextDrawer.getFontSpacing() / 2.f);
	m_sceneCrafter.setFontForFormatting(&m_font.get(), m_sceneTextDrawer.getFontSize(), m_sceneTextDrawer.getFontSpacing());
}

void CRaylibMainWindow::updateSceneText()
{
	m_sceneTextDrawer.recreateTextRenderTexture(m_sceneCrafter.getFormattedSceneText().data());
}

void CRaylibMainWindow::updateHelpText()
{
	static constexpr const char8_t s_help[] =
	{
		u8"[C] Toggle text colour\n"
		u8"[H] Hide help\n"
		u8"[T] Hide scene text\n"
		u8"[Wheel] Scale up/down; with Ctrl to zoom in/out\n"
		u8"[Left-pressed + wheel] Speed up/down the animation\n"
		u8"[Left-drag] Move view-point\n"
		u8"[R-click] Show context menu to jump scene\n"
		u8"[R-pressed + M-click] Hide/show the border of window\n"
		u8"[Middle-click] Reset scale, speed, and view-point\n"
		u8"[←|→; Right-pressed + wheel] Fast forward/rewind the text\n"
		u8"[↑|↓] Open the next/prev. folder\n"
	};
	m_helpTextDrawer.recreateTextRenderTexture(reinterpret_cast<const char*>(s_help));
}

void CRaylibMainWindow::menuOnOpenFolder()
{
	std::wstring utf16SelectedFolderPath = win_dialogue::SelectFolder(L"Select bundles/hcg/XXXXXX folder", ::GetWindowHandle());

	char pathBuffer[raylib_filesystem_utility::kMaxPathLength]{};
	constexpr size_t pathBufferCapacity = std::size(pathBuffer);
	size_t pathBufferLength = win_text::NarrowUtf8InBuffer(utf16SelectedFolderPath.data(), static_cast<int>(utf16SelectedFolderPath.size()), pathBuffer, pathBufferCapacity);

	std::string_view utf8SelectedFolderPath(pathBuffer, pathBufferLength);
	std::string_view parentFolderPath = path_utility::ExtractParentPath(utf8SelectedFolderPath);

	m_nSceneSpineFolderPathIndex = 0;
	m_sceneSpineFolderPaths = raylib_filesystem_utility::CreateFilePathList(parentFolderPath, {}, raylib_filesystem_utility::EPathType::Directory);
	if (m_sceneSpineFolderPaths.empty())return;
	
	const auto& iter = std::find_if(m_sceneSpineFolderPaths.begin(), m_sceneSpineFolderPaths.end(),
		[&utf8SelectedFolderPath](const std::string& folderPath)
		{
			return folderPath == utf8SelectedFolderPath;
		}
	);
	if (iter != m_sceneSpineFolderPaths.cend())
	{
		m_nSceneSpineFolderPathIndex = std::distance(m_sceneSpineFolderPaths.begin(), iter);
	}

	loadScenario(m_sceneSpineFolderPaths[m_nSceneSpineFolderPathIndex]);
}

void CRaylibMainWindow::resizeWindow()
{
	static constexpr float kScaleDelta = 0.025f;

	float canvasScale = m_sceneSpinePlayer.getCanvasScale();
	Vector2 baseSize = m_sceneSpinePlayer.getBaseSize();

	const int monitor = ::GetCurrentMonitor();
	const int monitorWidth = ::GetMonitorWidth(monitor);
	const int monitorHeight = ::GetMonitorHeight(monitor);

	/*
	* It seems that Windows forces fullscreen mode once the windows has the same size as the monitor.
	* This happens as well in other libraries like SFML which create window on OpenGL backend,
	* but does not happen in libraries which support DirectX backend like DxLib, Siv3D, and SDL3.
	*/

	const int windowWidth = (std::min)(static_cast<int>(baseSize.x * canvasScale), monitorWidth + 4);
	/* rlImgui seems to cause scissors problem when window has the same, or greater height as/than monitor. */
	const int windowHeight = (std::min)(static_cast<int>(baseSize.y * canvasScale), monitorHeight + (m_windowStyle.isBorderless ? 0 : -32) + 4);

	::SetWindowSize(windowWidth, windowHeight);

	const int rendertextureWidth = windowWidth;
	const int rendertextureHeight = windowHeight - m_windowState.lastMenuBarHeight;

	m_spineRenderTexture.reset(::LoadRenderTexture(rendertextureWidth, rendertextureHeight));
	updateSceneText();
	m_sceneCrafter.onResize();
}

void CRaylibMainWindow::alignWindowToTheTopLeftOfMonitor()
{
	const int monitor = ::GetCurrentMonitor();
	Vector2 moitorPposition = ::GetMonitorPosition(monitor);
	/* Raylib does not provide API to retrieve border size. */
	const int titleBarHeight = m_windowStyle.isBorderless ? 0 : 32;
	::SetWindowPosition(moitorPposition.x, moitorPposition.y + titleBarHeight);
}

void CRaylibMainWindow::setSpinePlayerSize()
{
	static constexpr float kDefaultWidth = 1530.f;
	static constexpr float kDefaultHeight = 970.f;

	m_sceneSpinePlayer.setOffset(0.f, 0.f);
	m_sceneSpinePlayer.setBaseSize(kDefaultWidth, kDefaultHeight);
}

void CRaylibMainWindow::resetSpinePlayerScale()
{
	static constexpr float kDefaultZoomDiff = 0.15f;
	m_sceneSpinePlayer.resetScale();

	const Vector2 fBaseSize = m_sceneSpinePlayer.getBaseSize();
	const float skeletonScale = m_sceneSpinePlayer.getSkeletonScale();

#if 0 /* Raylib can toggle borderless style after window creation, so this would not be preferred. */
	const int monitor = ::GetCurrentMonitor();
	const int monitorWidth = ::GetMonitorWidth(monitor);
	const int monitorHeight = ::GetMonitorHeight(monitor);

	const bool isLandscape = monitorWidth > monitorHeight;
	const bool toFitToWidth = isLandscape ^ m_windowStyle.toFitToMonitorHeight;
	const float scale = (toFitToWidth ? monitorWidth : monitorHeight) / (toFitToWidth ? fBaseSize.x : fBaseSize.y);

	m_sceneSpinePlayer.setSkeletonScale(scale + kDefaultZoomDiff);
	m_sceneSpinePlayer.setCanvasScale(scale);
#else
	m_sceneSpinePlayer.setSkeletonScale(skeletonScale + kDefaultZoomDiff);
	m_sceneSpinePlayer.setCanvasScale(skeletonScale);
#endif

	resizeWindow();
}

void CRaylibMainWindow::imguiMenuBar()
{
	int menuBarHeight = 0;
	if (!m_windowStyle.isBorderless)
	{
		if (ImGui::BeginMainMenuBar())
		{
			menuBarHeight = static_cast<int>(ImGui::GetWindowSize().y);
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open folder"))
				{
					menuOnOpenFolder();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Option"))
			{
				bool isSceneTextShown = m_sceneTextDrawer.isTextVisible();
				if (ImGui::MenuItem("Show scene text", nullptr, &isSceneTextShown))
				{
					m_sceneTextDrawer.setTextVisibility(isSceneTextShown);
				}

				bool isHelpShown = m_helpTextDrawer.isTextVisible();
				if (ImGui::MenuItem("Show help", nullptr, &isHelpShown))
				{
					m_helpTextDrawer.setTextVisibility(isHelpShown);
				}

				if (ImGui::MenuItem("Setting", nullptr, &m_windowState.toShowSettingDialogue))
				{
					m_windowState.toUpdateSettingFontParameters = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	m_windowState.lastMenuBarHeight = menuBarHeight;
}

void CRaylibMainWindow::imguiPopupMenu()
{
	if (const auto& labelData = m_sceneCrafter.getLabelData(); !labelData.empty())
	{
		if (ImGui::BeginPopup("Jump##ContextMenu"))
		{
			for (const auto& labelDatum : labelData)
			{
				if (ImGui::MenuItem(labelDatum.caption.data()))
				{
					m_sceneCrafter.jumpScene(labelDatum.nTextIndex);
					updateScene();
				}
			}
			ImGui::EndPopup();
		}
	}

	if (m_windowState.toShowPopupMenu)
	{
		ImGui::OpenPopup("Jump##ContextMenu");
		m_windowState.toShowPopupMenu = false;
	}
}

void CRaylibMainWindow::imguiSettingDialogue()
{
	/* ホイール回転で増減可能な浮動小数点数スライダ */
	static constexpr auto ScrollableSliderFloat = [](const char* label, float* v, float v_min, float v_max, float v_step, const char* format = "%.0f", ImGuiSliderFlags flags = 0)
		-> bool
		{
			bool result = ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
			if (result)return result;
			else
			{
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
				if (ImGui::IsItemHovered())
				{
					float wheel = ImGui::GetIO().MouseWheel;
					if (wheel > 0.f && (*v + v_step) < v_max)
					{
						*v += v_step;
					}
					else if (wheel < 0.f && (*v - v_step) > v_min)
					{
						*v -= v_step;
					}

					return wheel != 0.f;
				}
			}

			return result;
		};

	/* ホイール回転で増減可能な整数スライダ */
	static constexpr auto ScrollableSliderInt = [](const char* label, int* v, int v_min, int v_max)
		-> bool
		{
			bool result = ImGui::SliderInt(label, v, v_min, v_max);
			if (result)return result;
			else
			{
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
				if (ImGui::IsItemHovered())
				{
					float wheel = ImGui::GetIO().MouseWheel;
					if (wheel > 0 && *v < v_max)
					{
						++(*v);
					}
					else if (wheel < 0 && *v > v_min)
					{
						--(*v);
					}

					return wheel != 0.f;
				}
			}

			return result;
		};

	if (m_windowState.toShowSettingDialogue)
	{
		ImGui::Begin("Setting", &m_windowState.toShowSettingDialogue, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::SeparatorText("Voice");

		float voiceVolume = static_cast<float>(m_voicePlayer.getCurrentVolume());
		if (ScrollableSliderFloat("Volume", &voiceVolume, 0.f, 1.0f, 0.01f, "%.2f"))
		{
			m_voicePlayer.setCurrentVolume(voiceVolume);
		}

		ImGui::SeparatorText("Font");

		if (m_windowState.toUpdateSettingFontParameters)
		{
			m_fontStyle.sceneFontSize = m_sceneTextDrawer.getFontSize();
			m_fontStyle.sceneFontThickness = m_sceneTextDrawer.getFontThickness();
			m_fontStyle.sceneFontSpacing = m_sceneTextDrawer.getFontSpacing();

			m_windowState.toUpdateSettingFontParameters = false;
		}

		ScrollableSliderFloat("Size", &m_fontStyle.sceneFontSize, 8.f, 64.f, 1.f);
		ScrollableSliderInt("Thickness", &m_fontStyle.sceneFontThickness, 0, 8);
		ScrollableSliderFloat("Spacing", &m_fontStyle.sceneFontSpacing, 0.f, 8.f, 1.f);

		if (ImGui::Button("Apply##ApplyFontSetting"))
		{
			m_sceneTextDrawer.setFont(m_font.get(), m_fontStyle.sceneFontSize, m_fontStyle.sceneFontThickness, m_fontStyle.sceneFontSpacing);
			m_sceneCrafter.setFontForFormatting(&m_font.get(), m_sceneTextDrawer.getFontSize(), m_sceneTextDrawer.getFontSpacing());
			updateSceneText();
		}

		ImGui::SeparatorText("Mouse wheel");
		ImGui::Checkbox("Reverse zoom direction", &m_windowStyle.isZoomReversed);

		ImGui::End();
	}
}

bool CRaylibMainWindow::loadScenario(const std::string& sceneSpineFolderPath)
{
	bool bRet = m_sceneCrafter.loadScenario(sceneSpineFolderPath, m_sceneSpinePlayer);
	if (bRet)
	{
		setSpinePlayerSize();
		resetSpinePlayerScale();

		m_spineClock.restart();
		m_textClock.restart();
	}

	return bRet;
}

void CRaylibMainWindow::shiftScene(bool forward)
{
	m_sceneCrafter.shiftScene(forward);
	updateScene();
}

void CRaylibMainWindow::updateScene()
{
	const std::string* pVoiceFilePath = m_sceneCrafter.getCurrentVoiceFilePath();
	if (pVoiceFilePath != nullptr)
	{
		wchar_t utf16PathBuffer[raylib_filesystem_utility::kMaxPathLength]{};
		static constexpr size_t pathBufferCapacity = std::size(utf16PathBuffer);
		int nWritten = win_text::WidenUtf8InBuffer(pVoiceFilePath->data(), static_cast<int>(pVoiceFilePath->size()), utf16PathBuffer, pathBufferCapacity);
		if (nWritten != 0)
		{
			utf16PathBuffer[nWritten] = L'\0';
			m_voicePlayer.play(utf16PathBuffer);
		}
	}

	updateSceneText();
	m_sceneCrafter.changeAnimationIfToBeSwitched(m_sceneSpinePlayer);
}

void CRaylibMainWindow::autoTexting()
{
	if (!m_sceneCrafter.hasReachedLastScene())
	{
		shiftScene(true);
	}
}

void CRaylibMainWindow::checkTextClock()
{
	constexpr float kAutoPlayInterval = 3.f;
	const float fSecond = m_textClock.getElapsedTime();
	if (m_voicePlayer.isEnded() && fSecond > kAutoPlayInterval)
	{
		autoTexting();
		m_textClock.restart();
	}
}
