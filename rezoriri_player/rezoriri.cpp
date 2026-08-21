

#include <map>

#include "rezoriri.h"

#include "path_utility.h"
#include "text_utility.h"

#include "../rezoriri/static_string.h"
#include "json_minimal.h"
#include "raylib_filesystem_utility.h"

/* 内部用 */
namespace rezoriri
{
	using StaticString32 = StaticString<char, 32>;

	struct PlayerSetting
	{
		StaticString32 atlasExtension = ".atlas";
		StaticString32 skeletonExtension = ".skel";
		StaticString32 voiceExtension = ".m4a";
		StaticString32 sceneTextExtension = ".xml";
#if defined(_WIN32)
		StaticString512 fontFilePath = R"(C:\Windows\Fonts\yumindb.ttf)";
#else
		StaticString512 fontFilePath;
#endif
		bool toTranslateText = true;
	};

	PlayerSetting g_playerSetting;

	/// @brief 設定ファイル読み込み
	static bool ReadSettingFile(PlayerSetting& playerSetting)
	{
		std::string settingFile = raylib_filesystem_utility::LoadFileAsString("setting.txt");
		if (settingFile.empty())return false;

		const char* p1 = &settingFile[0];
		const char* pStart = nullptr, * pEnd = nullptr;
		bool bRet = json_minimal::FindNextObject(&p1, "fileExtensions", &pStart, &pEnd);
		if (!bRet)return false;

		const char* p2 = pStart;
		bRet &= json_minimal::FindValueByName(p2, "atlas", &pStart, &pEnd);
		if (bRet)
		{
			playerSetting.atlasExtension.assign(pStart, pEnd - pStart);
		}

		bRet &= json_minimal::FindValueByName(p2, "skeleton", &pStart, &pEnd);
		if (bRet)
		{
			playerSetting.skeletonExtension.assign(pStart, pEnd - pStart);
		}

		bRet &= json_minimal::FindValueByName(p2, "voice", &pStart, &pEnd);
		if (bRet)
		{
			playerSetting.voiceExtension.assign(pStart, pEnd - pStart);
		}

		bRet &= json_minimal::FindValueByName(p2, "scenarioScript", &pStart, &pEnd);
		if (bRet)
		{
			playerSetting.sceneTextExtension.assign(pStart, pEnd - pStart);
		}

		bRet &= json_minimal::FindValueByName(p1, "fontPath", &pStart, &pEnd);
		if (bRet)
		{
			playerSetting.fontFilePath.assign(pStart, pEnd - pStart);
		}

		bRet = json_minimal::FindValueByName(p1, "translation", &pStart, &pEnd);
		if (bRet)
		{
			static constexpr std::string_view s_true = "true";
			playerSetting.toTranslateText = ::memcmp(pStart, s_true.data(), s_true.length()) == 0;
		}

		return true;
	}

	/// @brief 台本ファイル経路 => 音声フォルダ階層
	template<typename T>
	static std::basic_string<T> DeriveVoiceFolderPathFromScenarioFilePath(const std::basic_string<T>& filePath)
	{
		static constexpr const auto s_scriptFolderName = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "dialogxml";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"dialogxml";
			}();
		static constexpr const auto s_sceneVoiceFolderPath = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "dialogvoice/";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"dialogvoice/";
			}();

		size_t nPos = filePath.find(s_scriptFolderName);
		if (nPos == std::basic_string<T>::npos)return {};

		return filePath.substr(0, nPos).append(s_sceneVoiceFolderPath);
	}

	/// @brief dialog_showファイル経路からdialog_textファイル経路導出
	template<typename T>
	static std::basic_string<T> ShowXmlPathToTextXmlPath(const std::basic_string<T>& dialogueShowXmlFilePath)
	{
		static constexpr const auto s_dialogueTextPath = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "/dialog_text";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"/dialog_text";
			}();

		std::basic_string_view<T> parentFolderPath = path_utility::ExtractParentPath(dialogueShowXmlFilePath);
		std::string textXmlPath = std::basic_string<T>(parentFolderPath).append(s_dialogueTextPath);

		return textXmlPath;
	}

	/// @brief XML復文処理
	template<typename T>
	static void UnescapeXML(std::basic_string<T>& text)
	{
		static constexpr const auto s_escapedLineBreak = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "&#xA;";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"&#xA;";
			}();
		static constexpr const auto s_escapedLessThan = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "&lt;";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"&lt;";
			}();
		static constexpr const auto s_escapedGreaterThan = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "&gt;";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"&gt;";
			}();
		static constexpr const auto s_lineBreak = []()
			-> std::basic_string_view<T>
			{
				if constexpr (std::is_same_v<T, char>) return "\n";
				else if constexpr (std::is_same_v<T, wchar_t>) return L"\n";
			}();

		text_utility::ReplaceAll(text, s_escapedLineBreak, s_lineBreak);

		for (size_t nRead = 0;;)
		{
			size_t nStart = text.find(s_escapedLessThan, nRead);
			if (nStart == std::basic_string<T>::npos)return;

			size_t nEnd = text.find(s_escapedGreaterThan, nStart);
			if (nEnd == std::basic_string<T>::npos)return;

			nEnd += s_escapedGreaterThan.length();
			text.erase(nStart, nEnd - nStart);

			nRead = nStart;
		}
	};

	/* Todo: Reduce memory allocation in parsing script files. */

	/// @brief 要素探索
	static long long FindAttribute(const std::vector<std::pair<std::string, std::string>>& attributes, std::string_view str)
	{
		for (size_t i = 0; i < attributes.size(); ++i)
		{
			if (attributes[i].first == str)
			{
				return i;
			}
		}
		return -1;
	}

	/// @brief 指令文一単位
	struct CommandDatum
	{
		std::string commandType;
		std::map<std::string, std::string> parameters;
	};

	/// @brief dialog_show解析
	static void ParseDialogueShowXml(const std::string& dialogueShowXml, std::vector<CommandDatum>& commandData)
	{
		std::vector<std::string> tags;
		text_utility::ToXmlTags(dialogueShowXml, "dialog_show", tags);
		for (const auto& tag : tags)
		{
			std::vector<std::pair<std::string, std::string>> attributes;
			text_utility::GetXmlAttributes(tag, attributes);

			long long llIndex = FindAttribute(attributes, "show_type");
			if (llIndex == -1)continue;

			CommandDatum commandDatum;
			commandDatum.commandType = attributes[llIndex].second;

			for (const auto& attiribute : attributes)
			{
				if (attiribute != attributes[llIndex])
				{
					commandDatum.parameters.insert({ attiribute.first, attiribute.second });
				}
			}

			commandData.push_back(std::move(commandDatum));
		}
	}

	/// @brief dialg_text解析
	static void ParseDialogueTextXml(const std::string& dialogueTextXml, std::map<std::string, std::string>& textMap)
	{
		std::vector<std::string> tags;
		text_utility::ToXmlTags(dialogueTextXml, "dialog_text", tags);
		for (const auto& tag : tags)
		{
			std::vector<std::pair<std::string, std::string>> attributes;
			text_utility::GetXmlAttributes(tag, attributes);

			long long llIndex = FindAttribute(attributes, "id");
			if (llIndex == -1)continue;
			const std::string& id = attributes[llIndex].second;

			llIndex = FindAttribute(attributes, "text");
			if (llIndex == -1)continue;
			const std::string& text = attributes[llIndex].second;

			textMap.insert({ id, text });
		}
	}
}

bool rezoriri::InitialiseSetting()
{
	PlayerSetting playerSetting;
	bool bRet = ReadSettingFile(playerSetting);
	if (bRet)
	{
		/* Actually, this results in copy because they are in stack. */
		g_playerSetting = std::move(playerSetting);
	}

	return (g_playerSetting.atlasExtension != g_playerSetting.skeletonExtension) && !g_playerSetting.fontFilePath.empty();
}

std::string_view rezoriri::GetFontFilePath()
{
	return g_playerSetting.fontFilePath.string_view();
}

void rezoriri::GetSpineList(std::string_view sceneSpineFolderPath, std::vector<std::string>& atlasFilePaths, std::vector<std::string>& skeletonFilePaths)
{
	const bool isAtlasLonger = g_playerSetting.atlasExtension.size() > g_playerSetting.skeletonExtension.size();

	std::string_view longerExtesion = isAtlasLonger ? g_playerSetting.atlasExtension.string_view() : g_playerSetting.skeletonExtension.string_view();
	std::string_view shorterExtension = isAtlasLonger ? g_playerSetting.skeletonExtension.string_view() : g_playerSetting.atlasExtension.string_view();

	std::vector<std::string>& longerFilePaths = isAtlasLonger ? atlasFilePaths : skeletonFilePaths;
	std::vector<std::string>& shorterFilePaths = isAtlasLonger ? skeletonFilePaths : atlasFilePaths;

	std::vector<std::string> filePaths = raylib_filesystem_utility::CreateFilePathList(sceneSpineFolderPath, {}, raylib_filesystem_utility::EPathType::File);

	for (auto& filePath : filePaths)
	{
		if (filePath.ends_with(longerExtesion))
		{
			longerFilePaths.push_back(std::move(filePath));
		}
		else if (filePath.ends_with(shorterExtension))
		{
			shorterFilePaths.push_back(std::move(filePath));
		}
	}
}

std::string rezoriri::DeriveScriptFilePathFromSceneSpineFolderPath(std::string_view sceneSpineFolderPath)
{
	char scenarioIdBuffer[32]{};
	static constexpr size_t scenarioIdBufferCapacity = std::size(scenarioIdBuffer);
	std::string_view spineFolderName = path_utility::ExtractParentFolderName(sceneSpineFolderPath);
	if (spineFolderName.size() < 2)return {};

	if (spineFolderName.size() >= scenarioIdBufferCapacity)return {};
	::memcpy(scenarioIdBuffer, spineFolderName.data(), spineFolderName.size());
	scenarioIdBuffer[spineFolderName.size() - 2] = '6';
	scenarioIdBuffer[spineFolderName.size()] = '\0';
	size_t scenarioIdBufferLength = spineFolderName.size();

	size_t nPos = sceneSpineFolderPath.rfind("bundles");
	if (nPos == std::wstring::npos)return {};

	std::string scriptFilePath = std::string(sceneSpineFolderPath.substr(0, nPos))
		.append("dialogxml/jp/")
		.append(scenarioIdBuffer, scenarioIdBufferLength)
		.append("/dialog_show")
		.append(g_playerSetting.sceneTextExtension.string_view());

	return scriptFilePath;
}

bool rezoriri::ReadScenario(const std::string& scenarioFilePath, std::vector<adv::TextDatum>& textData, std::vector<std::string>& animationNames, std::vector<adv::LabelDatum>& labelData)
{
	std::string dialogueShowXml = raylib_filesystem_utility::LoadFileAsString(scenarioFilePath.data());
	if (dialogueShowXml.empty())return false;

	std::string dialogueTextXml = raylib_filesystem_utility::LoadFileAsString(ShowXmlPathToTextXmlPath(scenarioFilePath).append(g_playerSetting.sceneTextExtension.string_view()).data());
	if (dialogueTextXml.empty())return false;

	std::vector<CommandDatum> commandData;
	ParseDialogueShowXml(dialogueShowXml, commandData);
	if (commandData.empty())return false;

	std::map<std::string, std::string> textMap;
	if (g_playerSetting.toTranslateText)
	{
		/* 翻訳文章と対応ID取得 */
		ParseDialogueTextXml(dialogueTextXml, textMap);
		if (textMap.empty())return false;
	}

	std::string voiceFolderPath = DeriveVoiceFolderPathFromScenarioFilePath(scenarioFilePath);
	if (voiceFolderPath.empty())return false;

	size_t nCurrentAnimationIndex = 0;
	std::string* pLastLabelName = nullptr;

	for (const auto& commandDatum : commandData)
	{
		const auto& type = commandDatum.commandType;
		const auto& params = commandDatum.parameters;

		/*
		* ---------- The command-types in "dialog_show.xml" ----------
		* - InsertDialog
		*   - parameter1 : character name ID?
		*   - parameter4 : text ID to be referred in "dialog_text.xml"
		*   - parameter5 : text
		*   - parameter6 : voice file name
		*
		* - InsertDynamicCG
		*   - parameter1 : Spine folder name?
		*   - parameter2 : Initial animation name
		*
		* - DynamicCGChangeAction
		*   - parameter1 : Animation name to be set
		*
		* - InsertBackgroundVoice
		*   - parameter1 : background voice file name
		*   - parameter2 : BGV ID
		*
		* - InsertSound
		*   - parameter1 : sound effect file name
		*
		* - InsertHeroTemplate; appears only in the latest scripts
		*   - parameter18 : text ID to be referred in "dialog_text.xml"
		*   - parameter19 : text
		*   - parameter20 : voice folder name and its file name
		*/

		const auto PushBackLabelDatum = [&]()
			-> void
			{
				const auto& iter = std::find_if(labelData.begin(), labelData.end(),
					[&pLastLabelName](const adv::LabelDatum& labelDatum)
					{
						return labelDatum.caption == *pLastLabelName;
					});
				if (iter == labelData.cend())
				{
					adv::LabelDatum labelDatum
					{
						.caption = *pLastLabelName,
						.nTextIndex = textData.size() - 1
					};
					labelData.push_back(std::move(labelDatum));
				}
			};

		enum class EInsertCommand : uint8_t
		{
			InsertDialog,
			InsertHeroTemplate
		};
		const auto PushBackTextDatum = [&](EInsertCommand inertCommand)
			-> void
			{
				bool isInSertDialog = inertCommand == EInsertCommand::InsertDialog;

				adv::TextDatum t;
				if (g_playerSetting.toTranslateText)
				{
					const auto& iterId = params.find(isInSertDialog ? "parameter4" : "parameter18");
					if (iterId == params.cend())return;

					const auto& iterText = textMap.find(iterId->second);
					if (iterText == textMap.cend())return;

					t.sceneText = iterText->second;
				}
				else
				{
					const auto& iterRawText = commandDatum.parameters.find(isInSertDialog ? "parameter5" : "parameter19");
					if (iterRawText == params.cend())return;

					t.sceneText = iterRawText->second;
				}

				const auto& iterVoice = params.find(isInSertDialog ? "parameter6" : "parameter20");
				if (iterVoice != params.cend())
				{
					t.voiceFilePath.assign(voiceFolderPath).append(iterVoice->second).append(g_playerSetting.voiceExtension.string_view());
				}

				UnescapeXML(t.sceneText);
				t.nAnimationIndex = nCurrentAnimationIndex;
				textData.push_back(std::move(t));
				
				if (pLastLabelName != nullptr)
				{
					PushBackLabelDatum();
					pLastLabelName = nullptr;
				}
			};

		if (type == "InsertDialog")
		{
			PushBackTextDatum(EInsertCommand::InsertDialog);
		}
		else if (type == "DynamicCGChangeAction")
		{
			const auto& iter = params.find("parameter1");
			if (iter == params.cend())continue;

			/* 対応文章が挟まることなく動作が切り替わるので1つ前のデータを上書きする */
			if (pLastLabelName != nullptr)
			{
				PushBackLabelDatum();
				if (!textData.empty())
				{
					textData.back().nAnimationIndex = nCurrentAnimationIndex;
				}
			}

			animationNames.push_back(iter->second);
			nCurrentAnimationIndex = animationNames.size() - 1;
			pLastLabelName = &animationNames.back();
		}
		else if (type == "InsertDynamicCG")
		{
			const auto& iter = params.find("parameter2");
			if (iter == params.cend())continue;

			if (pLastLabelName != nullptr)
			{
				PushBackLabelDatum();
				if (!textData.empty())
				{
					textData.back().nAnimationIndex = nCurrentAnimationIndex;
				}
			}

			animationNames.push_back(iter->second);
			nCurrentAnimationIndex = animationNames.size() - 1;
			pLastLabelName = &animationNames.back();
		}
		else if (type == "InsertHeroTemplate")
		{
			PushBackTextDatum(EInsertCommand::InsertHeroTemplate);
		}
	}

	for (auto& textDatum : textData)
	{
		text_utility::ReplaceAll(textDatum.sceneText, u8"[USER]", u8"俺");
		text_utility::ReplaceAll(textDatum.sceneText, u8"[user]", u8"俺");
	}

	return true;
}
