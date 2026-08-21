
#include <Windows.h>
#include <urlmon.h>
#include <atlbase.h>

#include "win_internet_utility.h"

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "shlwapi.lib")

namespace win_internet_utility
{
	/* 最大経路長 */
	static constexpr size_t kMaxPathLength = 1024;

	static void WriteMessage(const wchar_t* format, ...)
	{
		wchar_t swBuffer[4096]{};
		constexpr size_t bufferSize = sizeof(swBuffer)/sizeof(wchar_t) - 1;

		SYSTEMTIME tm;
		::GetLocalTime(&tm);

		int timeLen = swprintf_s(swBuffer, L"%02d:%02d:%02d:%03d ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
		if (timeLen == -1)return;

		va_list args;
		va_start(args, format);
		int formatLen = vswprintf_s(swBuffer + timeLen, bufferSize - timeLen, format, args);
		va_end(args);

		/* The length not including null termination. */
		size_t nWritten = static_cast<size_t>(timeLen + formatLen);
		if (nWritten < bufferSize - 1ULL)
		{
			swBuffer[nWritten] = L'\n';
			++nWritten;
			swBuffer[nWritten] = L'\0';
		}

		wprintf(swBuffer);
	}

	static std::wstring_view GetCurrentProcessPath()
	{
		static wchar_t s_basePath[kMaxPathLength]{};
		static size_t s_basePathLength = 0;
		if (s_basePath[0] == L'\0')
		{
			static constexpr size_t basePathSize = sizeof(s_basePath) / sizeof(wchar_t);
			DWORD length = ::GetModuleFileNameW(nullptr, s_basePath, basePathSize);
			wchar_t* pEnd = s_basePath + length;
			for (; pEnd != s_basePath; --pEnd)
			{
				if (*pEnd == L'\\' || *pEnd == L'/')break;
			}

			wchar_t* pFileName = pEnd + 1;
			size_t fileNameLength = s_basePath + length - pFileName;
			wmemset(pFileName, L'\0', fileNameLength);

			s_basePathLength = pFileName - s_basePath;
		}

		return std::wstring_view(s_basePath, s_basePathLength);
	}

	bool CreateDirectoryInBuffer(std::wstring_view directoryName, wchar_t* dst, size_t dstSize, size_t& nWritten, std::wstring_view basePath)
	{
		if (basePath.empty())
		{
			basePath = GetCurrentProcessPath();
		}

		if (dstSize < basePath.size())return false;

		wmemcpy(dst, basePath.data(), basePath.size());
		nWritten = basePath.size();

		size_t nRead = 0;
		if (dst[nWritten - 1ULL] != L'\\' && dst[nWritten - 1ULL] != L'/')
		{
			dst[nWritten++] = L'\\';
			dst[nWritten] = L'\0';

		}
		if (directoryName[0] == L'\\' || directoryName[0] == L'/')++nRead;

		for (;;)
		{
			size_t nPos = directoryName.find_first_of(L"\\/", nRead);
			if (nPos == std::wstring_view::npos)
			{
				const wchar_t* pRead = directoryName.data() + nRead;
				size_t nLength = directoryName.size() - nRead;
				if (dstSize < nWritten + nLength + 1)return false;

				wmemcpy(dst + nWritten, pRead, nLength);
				nWritten += nLength;
				dst[nWritten++] = L'\\';
				dst[nWritten] = L'\0';

				::CreateDirectoryW(dst, nullptr);

				break;
			}

			const wchar_t* pRead = &directoryName[nRead];
			size_t nLength = nPos - nRead;
			if (dstSize < nWritten + nLength + 1)return false;

			wmemcpy(dst + nWritten, pRead, nLength);
			nWritten += nLength;
			dst[nWritten++] = L'\\';
			dst[nWritten] = L'\0';

			::CreateDirectoryW(dst, nullptr);

			nRead = nPos + 1;
		}

		return true;
	}

	static bool DoesFilePathExist(const wchar_t* path)
	{
		WIN32_FILE_ATTRIBUTE_DATA win32FileAttributeData{};
		BOOL iRet = ::GetFileAttributesExW(path, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &win32FileAttributeData);

		return iRet != 0;
	}

	/// @brief 拡張子を含むファイル名を抜粋
	static std::wstring_view ExtractFileName(std::wstring_view path)
	{
		size_t nPos1 = path.find_last_of(L"\\/");
		if (nPos1 != std::wstring_view::npos)
		{
			++nPos1;
			size_t nPos2 = path.find('?', nPos1);

			return nPos2 == std::wstring_view::npos ?
				path.substr(nPos1) :
				path.substr(nPos1, nPos2 - nPos1);
		}

		return path;
	}
	/// @brief 親階層を抜粋
	static std::wstring_view ExtractParentDirectory(std::wstring_view path)
	{
		size_t nPos = path.find_last_of(L"\\/");
		if (nPos == std::wstring_view::npos)nPos = 0;

		return path.substr(0, nPos);
	}
}

std::string win_internet_utility::LoadInternetResourceAsString(const wchar_t* url)
{
	if (url == nullptr)return{};

	CComPtr<IStream> pStream;
	HRESULT hr = ::URLOpenBlockingStreamW(nullptr, url, &pStream, 0, nullptr);
	if (SUCCEEDED(hr))
	{
		STATSTG stat;
		hr = pStream->Stat(&stat, STATFLAG_DEFAULT);
		if (SUCCEEDED(hr))
		{
			std::string str(stat.cbSize.QuadPart, '\0');
			ULONGLONG nTotalRead = 0;
			for (;;)
			{
				DWORD ulRead = 0;
				ULONG ulToBeRead = static_cast<ULONG>(stat.cbSize.QuadPart - nTotalRead);
				hr = pStream->Read(&str[nTotalRead], ulToBeRead, &ulRead);
				if (FAILED(hr))break;

				nTotalRead += ulRead;
				if (nTotalRead >= stat.cbSize.QuadPart)break;
			}

			return str;
		}
	}

	return {};
}

uint64_t win_internet_utility::GetInternetResoureSize(const wchar_t* url)
{
	CComPtr<IStream> pStream;
	HRESULT hr = ::URLOpenBlockingStreamW(nullptr, url, &pStream, 0, nullptr);
	if (SUCCEEDED(hr))
	{
		STATSTG stat;
		hr = pStream->Stat(&stat, STATFLAG_DEFAULT);
		if (SUCCEEDED(hr))
		{
			return static_cast<uint64_t>(stat.cbSize.QuadPart);
		}
	}

	return 0;
}

bool win_internet_utility::SaveInternetResourceToFile(std::wstring_view url, std::wstring_view folderPath, std::wstring_view fileName, bool toOverwrite)
{
	/* Todo: escape more characters which cannot be used in filename. */
	std::wstring_view truncatedFileName =
		fileName.empty() ?
		ExtractFileName(url) :
		ExtractFileName(fileName);

	if (folderPath.empty())
	{
		folderPath = GetCurrentProcessPath();
	}

	static constexpr wchar_t longPathPrefix[] = LR"(\\?\)";
	static constexpr size_t longPathPrefixLength = sizeof(longPathPrefix) / sizeof(wchar_t) - 1;
	wchar_t filePath[kMaxPathLength]{};
	wmemcpy(filePath, longPathPrefix, longPathPrefixLength);
	constexpr size_t filePathSize = sizeof(filePath)/sizeof(wchar_t) - 1;
	size_t filePathLength = longPathPrefixLength;
	if (filePathSize < longPathPrefixLength + folderPath.size() + truncatedFileName.size() + 1)return false;

	wmemcpy(&filePath[filePathLength], folderPath.data(), folderPath.size());
	filePathLength += folderPath.size();

	wmemcpy(&filePath[filePathLength], truncatedFileName.data(), truncatedFileName.size());
	filePathLength += truncatedFileName.size();
	filePath[filePathLength++] = L'\0';

	if (!toOverwrite && DoesFilePathExist(filePath))
	{
		WriteMessage(L"%.*s %s", static_cast<int>(truncatedFileName.size()), truncatedFileName.data(), L"already exists.");

		return true;
	}

	const auto EscapeUrl = [](const std::wstring_view& url, wchar_t* dst, size_t dstSize)
		-> size_t
		{
			/* Null-terminated url */
			wchar_t urlBuffer[kMaxPathLength]{};
			constexpr size_t urlBufferSize = sizeof(urlBuffer) / sizeof(wchar_t) - 1;
			size_t urlBufferLength = 0;

			if (urlBufferSize < url.size())return 0;
			wmemcpy(urlBuffer, url.data(), url.size());
			urlBufferLength += url.size();
			urlBuffer[urlBufferLength++] = L'\0';

			/*
			* Before calling ::UrlEscapeW, should be the size of the buffer,
			* After successful, the number of character written to the buffer
			*/
			DWORD ulBufferLength = static_cast<DWORD>(dstSize);
			HRESULT hr = ::UrlEscapeW(urlBuffer, dst, &ulBufferLength, URL_ESCAPE_AS_UTF8);
			if (FAILED(hr))return 0;

			dst[ulBufferLength] = L'\0';

			return static_cast<size_t>(ulBufferLength);
		};


	wchar_t escapedUrlBuffer[kMaxPathLength]{};
	constexpr size_t escapedUrlBufferSize = sizeof(escapedUrlBuffer) / sizeof(wchar_t) - 1;
	size_t escapedUrlBufferLength = EscapeUrl(url, escapedUrlBuffer, escapedUrlBufferSize);
	if (escapedUrlBufferLength == 0)return false;

	HRESULT hr = ::URLDownloadToFileW(nullptr, escapedUrlBuffer, filePath, 0, nullptr);
	if (FAILED(hr))
	{
		WriteMessage(L"%.*s %s", static_cast<int>(url.size()), url.data(), L"failed");

		return false;
	}

	WriteMessage(L"%.*s %s", static_cast<int>(url.size()), url.data(), L"success");

	return true;
}

bool win_internet_utility::SaveInternetResourceToFileCreatingNestedFolder(std::wstring_view url, std::wstring_view fileName, std::wstring_view basePath, int depth)
{
	if (basePath.empty())
	{
		basePath = GetCurrentProcessPath();
	}

	const auto ExtractHostDir = [&url, &depth]()
		-> std::wstring_view
		{
			static constexpr std::wstring_view key = L"//";
			size_t nPos = url.find(key);
			if (nPos == std::wstring_view::npos)nPos = 0;
			nPos += sizeof(key);

			size_t nPos2 = nPos;
			do
			{
				nPos = url.find(L'/', nPos2);
				if (nPos2 == std::wstring_view::npos)break;
				else nPos2 = nPos + 1;

				--depth;
			} while (depth >= 0);

			size_t nPos3 = url.find_last_of(L'/');
			if (nPos3 == std::wstring_view::npos)nPos3 = url.size();

			return url.substr(nPos2, nPos3 - nPos2);
		};

	wchar_t folderPath[kMaxPathLength]{};
	size_t folderPathLength = 0;
	if (fileName.empty())
	{
		std::wstring_view hostDir = ExtractHostDir();
		CreateDirectoryInBuffer(hostDir, folderPath, sizeof(folderPath), folderPathLength, basePath);
	}
	else
	{
		std::wstring_view parentDir = ExtractParentDirectory(fileName);
		CreateDirectoryInBuffer(parentDir, folderPath, sizeof(folderPath), folderPathLength, basePath);
	}

	return SaveInternetResourceToFile(url, folderPath, fileName);
}
