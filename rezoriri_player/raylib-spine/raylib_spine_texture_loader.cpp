

#include "raylib_spine_texture_loader.h"

#include <spine/Atlas.h> /* spine::AtlasPage */
#include <raylib.h>

void CRaylibTextureLoader::load(spine::AtlasPage& atlasPage, const spine::String& textureFilePath)
{
	Image image{};
	if (m_pTextureLoadCallback != nullptr)
	{
		m_pTextureLoadCallback(m_pCallbackUserDatum, textureFilePath.buffer(), textureFilePath.length(), &image);
	}
	if (image.data == nullptr)
	{
		image = ::LoadImage(textureFilePath.buffer());
	}

#if defined(SPINE_40) || defined(SPINE_41) || defined (SPINE_42)
	bool toConvertToPma = !atlasPage.pma && m_toConvertToPma;
	if (toConvertToPma)
	{
		atlasPage.pma = true;
	}
#else
	bool toConvertToPma = m_toConvertToPma;
#endif
	if (toConvertToPma)
	{
		::ImageAlphaPremultiply(&image);
	}

	Texture2D texture2D = ::LoadTextureFromImage(image);
	::UnloadImage(image);

	switch (atlasPage.magFilter)
	{
	case spine::TextureFilter::TextureFilter_Nearest:
		::SetTextureFilter(texture2D, TextureFilter::TEXTURE_FILTER_POINT);
		break;
	case spine::TextureFilter::TextureFilter_Linear:
		::SetTextureFilter(texture2D, TextureFilter::TEXTURE_FILTER_BILINEAR);
		break;
	default:
		break;
	}

	switch (atlasPage.uWrap)
	{
	case spine::TextureWrap::TextureWrap_MirroredRepeat:
		::SetTextureWrap(texture2D, TextureWrap::TEXTURE_WRAP_MIRROR_REPEAT);
		break;
	case spine::TextureWrap::TextureWrap_ClampToEdge:
		::SetTextureWrap(texture2D, TextureWrap::TEXTURE_WRAP_CLAMP);
		break;
	case spine::TextureWrap::TextureWrap_Repeat:
		::SetTextureWrap(texture2D, TextureWrap::TEXTURE_WRAP_REPEAT);
		break;
	}

	::GenTextureMipmaps(&texture2D);

	Texture2D* pTexture2D = new (std::nothrow)(Texture2D)(texture2D);
	void* p = static_cast<void*>(pTexture2D);
#if defined (SPINE_41) || defined (SPINE_42)
	atlasPage.texture = p;
#else
	atlasPage.setRendererObject(p);
#endif
}

void CRaylibTextureLoader::unload(void* texture)
{
	Texture2D* texture2D = static_cast<Texture2D*>(texture);
	UnloadTexture(*texture2D);
	delete texture2D;
}

void CRaylibTextureLoader::enableConversionToPma(bool toEnable)
{
	m_toConvertToPma = toEnable;
}

bool CRaylibTextureLoader::isConversionToPmaEnabled() const noexcept
{
	return m_toConvertToPma;
}

void CRaylibTextureLoader::setTextureLoadCallback(void(*pFunc)(void* pUserDatum, const char* textureFilePath, size_t filePathLength, void* pOutImage), void* pUserDatum)
{
	m_pTextureLoadCallback = pFunc;
	m_pCallbackUserDatum = pUserDatum;
}
