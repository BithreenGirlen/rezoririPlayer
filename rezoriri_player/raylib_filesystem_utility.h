#ifndef RAYLIB_FILESYSTEM_UTILITY_H_
#define RAYLIB_FILESYSTEM_UTILITY_H_

#include <string>
#include <vector>

/// @brief Raylib filesystem utility depending on C++17 STL
namespace raylib_filesystem_utility
{
	/// @brief 最大経路長
	static constexpr size_t kMaxPathLength = 1024;

	/// @brief Path type to enumerate
	enum class EPathType : uint8_t
	{
		File,
		Directory,
	};

	/// @brief 指定階層と同階層のファイルもしくはフォルダ経路一覧を作成
	/// @param directoryPath 探索先階層
	/// @param fileSpec 名称候補。空の場合絞り込み無し。例："char_*2.json;card_*2.json"
	/// @param pathType ファイル一覧を生成するかフォルダ一覧を作成するか
	/// @param toScanSubDirectory 下位階層を探索するか否か
	/// @return 経路一覧
	[[nodiscard]] std::vector<std::string> CreateFilePathList(std::string_view directoryPath, std::string_view fileSpec, EPathType pathType, bool toScanSubDirectory = false);

	/// @brief 文字列としてファイル読み込み。バイナリ形式であったとしてもstd::stringに格納
	[[nodiscard]] std::string LoadFileAsString(const char* filePath);
}

#endif // !RAYLIB_FILESYSTEM_UTILITY_H_
