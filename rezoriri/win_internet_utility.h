#ifndef WIN_INTERNET_UTILITY_H_
#define WIN_INTERNET_UTILITY_H_

#include <string>

namespace win_internet_utility
{
	std::string LoadInternetResourceAsString(const wchar_t* url);
	uint64_t GetInternetResoureSize(const wchar_t* url);
	bool SaveInternetResourceToFile(std::wstring_view url, std::wstring_view folderPath = {}, std::wstring_view fileName = {}, bool toOverwrite = false);
	bool SaveInternetResourceToFileCreatingNestedFolder(std::wstring_view url, std::wstring_view fileName = {}, std::wstring_view basePath = {}, int depth = 0);
}
#endif // !WIN_INTERNET_UTILITY_H_
