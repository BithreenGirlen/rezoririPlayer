#ifndef RAYLIB_TEXT_DRAWER_H_
#define RAYLIB_TEXT_DRAWER_H_

#include "raylib_handle.h"

/// @brief 文字列描画器
class CRaylibTextDrawer
{
public:
	CRaylibTextDrawer() = default;
	~CRaylibTextDrawer() = default;

	/// @brief 字体設定
	void setFont(Font& font, float fontSize = kDefaultFillSize, int thickness = kDefaultOutLineSize, float spacing = kDefaultSpacing) noexcept;
	
	/// @brief 紋理再作成
	void recreateTextRenderTexture(const char* text) noexcept;
	/// @brief 紋理描画
	void renderTextRenderTexture(float posX, float posY) const noexcept;

	/// @brief 文字色白・黒切り替え
	void toggleTextColour() noexcept;

	/// @brief 文字列描画を行うか否か
	void setTextVisibility(bool visible) noexcept;
	bool isTextVisible() const noexcept;

	/// @brief 紋理寸法取得
	void getTextTextureSize(int* width, int* height) const noexcept;

	/// @brief 字体の大きさ取得
	float getFontSize() const noexcept;
	/// @brief 字体の縁の太さ取得
	int getFontThickness() const noexcept;
	/// @brief 文字間隔取得
	float getFontSpacing() const noexcept;

	static constexpr float kDefaultFillSize = 48.f;
	static constexpr int kDefaultOutLineSize = 4;
	static constexpr float kDefaultSpacing = 4.f;
private:
	Font* m_storedFont = nullptr;
	RaylibRenderTextureHandle m_textRenderTexture;
	RaylibShaderHandle m_outlineShader;

	float m_fontSize = kDefaultFillSize;
	int m_thickness = kDefaultOutLineSize;
	float m_spacing = kDefaultSpacing;

	bool m_isTextColourReversed = false;
	bool m_isTextVisible = true;
};

#endif // !RAYLIB_TEXT_DRAWER_H_
