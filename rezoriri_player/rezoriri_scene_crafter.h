#ifndef REZORIRI_SCENE_CRAFTER_H_
#define REZORIRI_SCENE_CRAFTER_H_

#include "raylib-spine/raylib_spine_player.h"
#include "adv.h"

/*
* Raylib does not allow ::BeginTextureMode and EndXXXXX to be nested, 
* which makes it so hard to other classes than main one to have draw function.
* On the other hand, it is tedious to rely on rlgl anymore for such a simple player.
* So this class is designed to manage and pass scene data, and not to update/draw.
*/

/// @brief 場面管理器
class CRezoririSceneCrafter
{
public:
	CRezoririSceneCrafter() = default;
	~CRezoririSceneCrafter() = default;

	/// @brief 文章整形用情報受け渡し
	void setFontForFormatting(Font* pFont, float fontSize, float spacing);

	/// @brief 台本の読み取りと記載データ取り込み
	bool loadScenario(const std::string& sceneSpineFolderPath, CRaylibSpinePlayer& spinePlayer);
	bool hasScenarioData() const noexcept;

	/// @brief 文章送り・戻し
	void shiftScene(bool forward);
	/// @brief 最終場面是否
	bool hasReachedLastScene() const noexcept;

	/// @brief 動作切り替えが必要か確認
	bool changeAnimationIfToBeSwitched(CRaylibSpinePlayer& spinePlayer) noexcept;

	/// @brief 現在の動作名称取得
	const std::string* getCurrentAnimationName() const noexcept;
	/// @brief 現在の場面文章取得
	const std::string& getFormattedSceneText() const noexcept;
	/// @brief 現在の文章対応音声ファイル経路取得
	const std::string* getCurrentVoiceFilePath() const noexcept;

	/// @brief 静止画切り替わり場面の名称・番号取得
	const std::vector<adv::LabelDatum>& getLabelData() const noexcept;
	/// @brief 特定場面に遷移
	bool jumpScene(size_t nSceneIndex);

	/// @brief 寸法変更通知
	void onResize();
private:
	std::vector<adv::TextDatum> m_textData;
	size_t m_nTextIndex = 0;

	std::vector<std::string> m_animationNames;
	size_t m_nLastAnimationIndex = 0;

	std::vector<adv::LabelDatum> m_lebelData;

	std::string m_formatedSceneText;
	/// @brief Font data necessary to fromat scene text
	struct FontInfo
	{
		Font* pFont = nullptr;
		float fontSize = 48.f;
		float spacing = 10.f;
		/* Usually DPI is required, but Raylib does not provide the way to handle it. */
	};
	FontInfo m_fontInfo;

	/// @brief 台本データ消去
	void clearScenerioData();

	/// @brief 場面前準備処理
	void prepareScene();
	/// @brief 表示文章整形
	void prepareText();
};

#endif // !REZORIRI_SCENE_CRAFTER_H_
