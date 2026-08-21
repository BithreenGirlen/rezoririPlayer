

#include "win_internet_utility.h"
#include "win_text.h"
#include "static_string.h"
#include "unity_bundle.h"

/* Avoid string literal for the benefit of IP. */
static constexpr const wchar_t g_rawHostDir[] =
{
	0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F,
	0x72, 0x65, 0x7A, 0x6F, 0x72, 0x69, 0x72, 0x69,
	0x2E, 0x66, 0x75, 0x6E, 0x79, 0x6F, 0x75, 0x72,
	0x73, 0x6A, 0x61, 0x70, 0x61, 0x6E, 0x2E, 0x70,
	0x69, 0x6E, 0x6B, 0x2F, 0x70, 0x72, 0x6F, 0x64,
	0x2F, 0x72, 0x65, 0x73, 0x2F, 0x77, 0x65, 0x62,
	0x67, 0x6C, 0x2F, 0x00
};
static constexpr std::wstring_view g_hostDir(g_rawHostDir, std::size(g_rawHostDir) - 1);

static uint32_t ToUInt32(const char* src)
{
	const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static std::string LoadManifestFile()
{
	static constexpr std::wstring_view compressedManifestName = L"webgl.bin";

	StaticWString512 catalogueDir;
	catalogueDir.append(g_hostDir).append(compressedManifestName);

	std::string bundleFile = win_internet_utility::LoadInternetResourceAsString(catalogueDir.data());
	if (bundleFile.empty())return {};

	std::vector<std::string> blocks = unity_bundle::DecompressUnityFs(bundleFile);
	if (blocks.empty())return {};

	return blocks[0];
}

static void ReadManifest(const std::string& manifestFile, std::vector<std::string>& rawFileNames)
{
	static constexpr std::string_view mapName = "AssetBundleManifest";

	/* File name */
	size_t nPos = manifestFile.find(mapName);
	if (nPos == std::string_view::npos)return;
	nPos += mapName.length() + 1;
	/* Map name */
	nPos = manifestFile.find(mapName, nPos);
	if (nPos == std::string_view::npos)return;
	nPos += mapName.length() + 1;
	/* Map size */
	uint32_t ulMapSize = ToUInt32(&manifestFile[nPos]);
	nPos += 4;

	rawFileNames.reserve(ulMapSize);
	for (uint32_t i = 0; i < ulMapSize; ++i)
	{
		/* Index */
		uint32_t ul = ToUInt32(&manifestFile[nPos]);
		nPos += 4;
		/* The length of filename */
		ul = ToUInt32(&manifestFile[nPos]);
		nPos += 4;
		/* Filename */
		rawFileNames.emplace_back(&manifestFile[nPos], ul);
		/* Filled with 0 to be multiple of 4 */
		size_t nPadding = ul % 4 ? 4ULL - ul % 4 : 0;
		nPos += ul + nPadding;
		if (nPos > manifestFile.size())break;
	}
}

static void DownloadBundle()
{
	std::vector<std::string> fileNames;
	{
		std::string manifestFile = LoadManifestFile();
		if (manifestFile.empty())return;

		ReadManifest(manifestFile, fileNames);
	}

	StaticWString512 urlBuffer;
	urlBuffer.append(g_hostDir);

	wchar_t fileNameBuffer[256]{};
	static constexpr size_t fileNameBufferSize = std::size(fileNameBuffer);

	for (const auto& fileName : fileNames)
	{
		int fileNameLength = win_text::WidenUtf8InBuffer(fileName.data(), static_cast<int>(fileName.size()), fileNameBuffer, fileNameBufferSize);

		urlBuffer.resize(g_hostDir.size());
		urlBuffer.append(fileNameBuffer, fileNameLength).append(L".bin");

		win_internet_utility::SaveInternetResourceToFileCreatingNestedFolder(urlBuffer.string_view());
	}
}

int main()
{
	DownloadBundle();
}
