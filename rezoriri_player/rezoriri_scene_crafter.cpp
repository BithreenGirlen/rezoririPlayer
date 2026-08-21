
#include "rezoriri_scene_crafter.h"
#include "rezoriri.h"
#include "raylib-spine/spine_file_verifier.h"
#include "raylib_filesystem_utility.h"
#include "path_utility.h"
#include "text_utility.h"

void CRezoririSceneCrafter::setFontForFormatting(Font* pFont, float fontSize, float spacing)
{
	m_fontInfo.pFont = pFont;
	m_fontInfo.fontSize = fontSize;
	m_fontInfo.spacing = spacing;
}

bool CRezoririSceneCrafter::loadScenario(const std::string& sceneSpineFolderPath, CRaylibSpinePlayer& spinePlayer)
{
	std::string scriptFilePath = rezoriri::DeriveScriptFilePathFromSceneSpineFolderPath(sceneSpineFolderPath);
	if (scriptFilePath.empty())return false;

	clearScenerioData();

	bool bRet = rezoriri::ReadScenario(scriptFilePath, m_textData, m_animationNames, m_lebelData);
	if (!bRet)return false;

	std::vector<std::string> atlasFilePaths;
	std::vector<std::string> skelFilePaths;
	rezoriri::GetSpineList(sceneSpineFolderPath, atlasFilePaths, skelFilePaths);
	if (atlasFilePaths.empty() || skelFilePaths.empty()) return false;

	std::string atlasFile = raylib_filesystem_utility::LoadFileAsString(atlasFilePaths[0].data());
	std::string skeletonFile = raylib_filesystem_utility::LoadFileAsString(skelFilePaths[0].data());
	if (atlasFile.empty() || skeletonFile.empty()) return false;

	spine_file_verifier::SkeletonMetadata skeletonMetaData = spine_file_verifier::VerifySkeletonFileData(reinterpret_cast<unsigned char*>(skeletonFile.data()), skeletonFile.size());
	if (skeletonMetaData.skeletonFormat == spine_file_verifier::SkeletonFormat::Neither)return false;
	if (!skeletonMetaData.version.starts_with("4.2"))return false;

	std::vector<std::string> textureFilePath;
	std::vector<std::string> atlasFileData;
	std::vector<std::string> skelFileData;

	textureFilePath.emplace_back(path_utility::ExtractParentPath(atlasFilePaths[0]));
	atlasFileData.push_back(std::move(atlasFile));
	skelFileData.push_back(std::move(skeletonFile));

	const bool isBinaryFormat = skeletonMetaData.skeletonFormat == spine_file_verifier::SkeletonFormat::Binary;
	const bool hasLoaded = spinePlayer.loadSpineFromMemory(atlasFileData, textureFilePath, skelFileData, isBinaryFormat);
	if (!hasLoaded) return false;

	spinePlayer.setPhysicsAll(CRaylibSpineDrawable::Physics::None);

	std::string_view atlasFileName = path_utility::ExtractFileNameWithoutExtension(atlasFilePaths[0]);
	char windowNameBuffer[128]{};
	static constexpr size_t windowNameBufferCapacity = std::size(windowNameBuffer);
	if (atlasFileName.length() < windowNameBufferCapacity)
	{
		::memcpy(windowNameBuffer, atlasFileName.data(), atlasFileName.length());
		windowNameBuffer[atlasFileName.length()] = '\0';
		::SetWindowTitle(windowNameBuffer);
	}

	prepareScene();

	return true;
}

bool CRezoririSceneCrafter::hasScenarioData() const noexcept
{
	return !m_textData.empty();
}

void CRezoririSceneCrafter::shiftScene(bool forward)
{
	if (m_textData.empty())return;

	if (forward)
	{
		if (++m_nTextIndex >= m_textData.size())
		{
			m_nTextIndex = 0;
		}
	}
	else
	{
		if (--m_nTextIndex >= m_textData.size())
		{
			m_nTextIndex = m_textData.size() - 1;
		}
	}

	prepareScene();
}

bool CRezoririSceneCrafter::hasReachedLastScene() const noexcept
{
	return m_nTextIndex == m_textData.size() - 1;
}

bool CRezoririSceneCrafter::changeAnimationIfToBeSwitched(CRaylibSpinePlayer& spinePlayer) noexcept
{
	if (m_nTextIndex >= m_textData.size())return false;
	const adv::TextDatum& textDatum = m_textData[m_nTextIndex];

	if (m_nTextIndex == 0 || (m_nLastAnimationIndex != textDatum.nAnimationIndex))
	{
		if (textDatum.nAnimationIndex < m_animationNames.size())
		{
			m_nLastAnimationIndex = textDatum.nAnimationIndex;
			spinePlayer.setAnimationByName(m_animationNames[m_nLastAnimationIndex].c_str());
		}

		return true;
	}

	return false;
}

const std::string* CRezoririSceneCrafter::getCurrentAnimationName() const noexcept
{
	return nullptr;
}

const std::string& CRezoririSceneCrafter::getFormattedSceneText() const noexcept
{
	return m_formatedSceneText;
}

const std::string* CRezoririSceneCrafter::getCurrentVoiceFilePath() const noexcept
{
	if (m_nTextIndex < m_textData.size())
	{
		return &m_textData[m_nTextIndex].voiceFilePath;
	}

	return nullptr;
}

const std::vector<adv::LabelDatum>& CRezoririSceneCrafter::getLabelData() const noexcept
{
	return m_lebelData;
}

bool CRezoririSceneCrafter::jumpScene(size_t nSceneIndex)
{
	if (nSceneIndex >= m_textData.size())return false;

	m_nTextIndex = nSceneIndex;
	prepareScene();

	return true;
}

void CRezoririSceneCrafter::onResize()
{
	prepareText();
}

void CRezoririSceneCrafter::clearScenerioData()
{
	m_textData.clear();
	m_nTextIndex = 0;

	m_animationNames.clear();
	m_nLastAnimationIndex = 0;

	m_lebelData.clear();

	m_formatedSceneText.clear();
}

void CRezoririSceneCrafter::prepareScene()
{
	prepareText();
}

void CRezoririSceneCrafter::prepareText()
{
	if (m_nTextIndex >= m_textData.size())return;
	const std::string& sceneText = m_textData[m_nTextIndex].sceneText;

	Font* pFont = m_fontInfo.pFont;
	if (pFont == nullptr)
	{
		m_formatedSceneText.assign(sceneText);
		if (m_formatedSceneText.back() != '\n')
		{
			m_formatedSceneText.push_back('\n');
		}
	}
	else
	{
		m_formatedSceneText.clear();

		float accumulatedWidth = 0.f;
		size_t nLast = 0;
		size_t nRead = 0;

		const auto& wrapText = [&]()
			-> void
			{
				if (nRead != nLast)
				{
					m_formatedSceneText.append(&sceneText[nLast], nRead - nLast);
					if (m_formatedSceneText.back() != '\n')
					{
						m_formatedSceneText.push_back('\n');
					}
				}

				accumulatedWidth = 0;
				nLast = nRead;
			};

		static const auto PointSizeToDip = [](float fPointSize, unsigned int dpi)
			-> float
			{
				return (fPointSize / 72.f) * dpi;
			};
		static constexpr unsigned int kDpi = 96;

		const float scale = PointSizeToDip(m_fontInfo.fontSize, kDpi) / static_cast<float>(pFont->baseSize);
		const float screenWidth = static_cast<float>(::GetScreenWidth()) / scale;
		for (; nRead < sceneText.size();)
		{
			int nAdvanced = 0;
			const int codePoint = ::GetCodepointNext(&sceneText[nRead], &nAdvanced);
			const int glyphIndex = ::GetGlyphIndex(*pFont, codePoint);

			nRead += static_cast<size_t>(nAdvanced);

			const float glyphWidth =
				pFont->glyphs[glyphIndex].advanceX > 0
				? pFont->glyphs[glyphIndex].advanceX
				: pFont->recs[glyphIndex].width + pFont->glyphs[glyphIndex].offsetX;

			if (codePoint == static_cast<int>('\n'))
			{
				wrapText();
				continue;
			}
			else if (codePoint == static_cast<int>('\r'))
			{
				continue;
			}
			else
			{
				accumulatedWidth += glyphWidth;
			}

			if (::isgreater(accumulatedWidth, screenWidth))
			{
				wrapText();
				accumulatedWidth = glyphWidth;
			}
		}

		if (::isgreater(accumulatedWidth, 0.f))
		{
			wrapText();
		}
	}

	text_utility::ReplaceAll(m_formatedSceneText, "\n\n", "\n");

	/* size_t is 20 digits at most. */
	char indexBuffer[64]{};
	sprintf_s(indexBuffer, "%zu/%zu", m_nTextIndex + 1, m_textData.size());
	m_formatedSceneText += indexBuffer;
}
