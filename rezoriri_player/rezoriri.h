#ifndef REZORIRI_H_
#define REZORIRI_H_

#include <string>
#include <vector>

#include "adv.h"

namespace rezoriri
{
	/// @brief 設定ファイル読み込み
	bool InitialiseSetting();

	/// @brief 設定字体ファイル経路取得
	std::string_view GetFontFilePath();

	/// @brief 指定階層に存在するSpineファイル経路を作成
	void GetSpineList(std::string_view sceneSpineFolderPath, std::vector<std::string>& atlasFilePaths, std::vector<std::string>& skeletonFilePaths);

	/// @brief 寸劇用Spineフォルダ経路 => 台本ファイル経路
	std::string DeriveScriptFilePathFromSceneSpineFolderPath(std::string_view sceneSpineFolderPath);

	/// @brief 台本読み込み
	bool ReadScenario(
		const std::string& scenarioFilePath,
		std::vector<adv::TextDatum>& textData,
		std::vector<std::string>& animationNames,
		std::vector<adv::LabelDatum>& labelData
	);
}

#endif // !REZORIRI_H_
