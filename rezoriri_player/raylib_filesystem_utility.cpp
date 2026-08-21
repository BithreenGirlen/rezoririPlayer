

#include "raylib_filesystem_utility.h"
#include "raylib_handle.h"

#include <raylib.h>

namespace raylib_filesystem_utility
{
	struct SplitFilter
	{
		static constexpr size_t kFilterCapacity = 8;

		size_t filterCount = 0;
		std::string_view filters[kFilterCapacity];
	};

	static bool HasFilterPattern(std::string_view path, std::string_view filter)
	{
		size_t pathPos = 0;
		size_t filerPos = 0;
		size_t pathLastPos = 0;
		size_t filterLastPos = 0;
		bool hasAsterisk = false;

		for (; pathPos < path.length(); )
		{
			if (filerPos < filter.length())
			{
				const char pathChar = static_cast<const char>(::tolower(static_cast<unsigned char>(path[pathPos])));
				const char filterChar = static_cast<const char>(::tolower(static_cast<unsigned char>(filter[filerPos])));

				if (filterChar == '?' || filterChar == pathChar)
				{
					pathPos++;
					filerPos++;

					continue;
				}
			}

			if (filerPos < filter.length() && filter[filerPos] == '*')
			{
				hasAsterisk = true;

				pathLastPos = pathPos;
				filterLastPos = ++filerPos;
			}
			else if (hasAsterisk)
			{
				pathPos = ++pathLastPos;
				filerPos = filterLastPos;
			}
			else
			{
				return false;
			}
		}

		for (; filerPos < filter.length() && filter[filerPos] == '*'; )
		{
			filerPos++;
		}

		return filerPos == filter.length();
	}

	static SplitFilter SplitFileSpecs(std::string_view fileSpecs)
	{
		SplitFilter splitFilter;
		if (fileSpecs.empty())return splitFilter;

		for (size_t nRead = 0;;)
		{
			size_t nPos = fileSpecs.find(';', nRead);
			if (nPos == std::string_view::npos)
			{
				std::string_view s = fileSpecs.substr(nRead);

				splitFilter.filters[splitFilter.filterCount] = s;
				++splitFilter.filterCount;

				break;
			}

			std::string_view s = fileSpecs.substr(nRead, nPos);

			splitFilter.filters[splitFilter.filterCount] = s;
			++splitFilter.filterCount;
			if (splitFilter.filterCount >= splitFilter.kFilterCapacity)break;

			nRead = nPos + 1;
		}

		return splitFilter;
	}
}

std::vector<std::string> raylib_filesystem_utility::CreateFilePathList(std::string_view directoryPath, std::string_view fileSpec, EPathType pathType, bool toScanSubDirectory)
{
	char pathBuffer[kMaxPathLength]{};

	if (directoryPath.length() >= kMaxPathLength)return {};

	::memcpy(pathBuffer, directoryPath.data(), directoryPath.length());
	pathBuffer[directoryPath.length()] = '\0';

	static constexpr std::string_view s_fileSpec = "FILES*";
	static constexpr std::string_view s_dirSpec = "DIRS*";

	const auto& generalSpec = pathType == EPathType::File ? s_fileSpec : s_dirSpec;
	RaylibFilePathListHandle filePathHandle(::LoadDirectoryFilesEx(pathBuffer, generalSpec.data(), toScanSubDirectory));
	if (!filePathHandle.isHandleValid())return {};

	std::vector<std::string> filePaths;
	filePaths.reserve(filePathHandle.get().count);

	SplitFilter splitFilter = SplitFileSpecs(fileSpec);

	for (unsigned int i = 0; i < filePathHandle.get().count; ++i)
	{
		auto& filePath = filePathHandle.get().paths[i];
		size_t filePathLength = ::strlen(filePath);
		std::string_view filePathView(filePath, filePathLength);

		size_t nPos = filePathView.find_last_of("\\/");
		if (nPos == std::string_view::npos)nPos = 0;
		else ++nPos;
		std::string_view fileNameWithExtension = filePathView.substr(nPos);

		const auto IsMatched = [&splitFilter, &fileNameWithExtension]()
			-> bool
			{
				if (splitFilter.filterCount == 0)return true;

				for (size_t i = 0; i < splitFilter.filterCount; ++i)
				{
					auto& filter = splitFilter.filters[i];
					if (HasFilterPattern(fileNameWithExtension, filter))
					{
						return true;
					}
				}

				return false;
			};

		if (IsMatched())
		{
			filePaths.emplace_back(filePathView);
		}
	}

	return filePaths;
}

std::string raylib_filesystem_utility::LoadFileAsString(const char* filePath)
{
	int dataLength = 0;
	unsigned char* pData = ::LoadFileData(filePath, &dataLength);
	if (pData == nullptr)return {};
	
	std::string file(reinterpret_cast<char*>(pData), dataLength);
	::UnloadFileData(pData);

	return file;
}
