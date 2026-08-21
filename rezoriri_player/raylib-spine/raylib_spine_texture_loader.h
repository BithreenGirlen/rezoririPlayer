#ifndef RAYLIB_SPINE_TEXTURE_LOADER_H_
#define RAYLIB_SPINE_TEXTURE_LOADER_H_

#include <spine/TextureLoader.h>

class CRaylibTextureLoader : public spine::TextureLoader
{
public:
	CRaylibTextureLoader() = default;
	virtual ~CRaylibTextureLoader() = default;

	void load(spine::AtlasPage& atlasPage, const spine::String& textureFilePath) override;
	void unload(void* texture) override;

	/// @brief Mind that blend-mode-multiply and screen are applicable only for premultiplied texture.
	void enableConversionToPma(bool toEnable);
	bool isConversionToPmaEnabled() const noexcept;
	/// @brief Register a load callback; *pOutImage should be casted to Image*
	/// @remark For case when unsupported format be used or decryption be performed.
	void setTextureLoadCallback(void (*pFunc)(void* pUserDatum, const char* textureFilePath, size_t filePathLength, void* pOutImage), void* pUserDatum);
private:
	bool m_toConvertToPma = false;

	void (*m_pTextureLoadCallback)(void* pUserDatum, const char* textureFilePath, size_t filePathLength, void* pOutImage) = nullptr;
	void* m_pCallbackUserDatum = nullptr;
};

#endif // !RAYLIB_SPINE_TEXTURE_LOADER_H_
