#ifndef RAYLIB_HANDLE_H_
#define RAYLIB_HANDLE_H_

#include <type_traits> /* std::is_same_v */

#include <raylib.h> /* UnloadXXXXX and IsXXXXXValid functions. */

/// @brief 資源管理器
/// @tparam T 型
/// @tparam pUnloader 破棄関数
template<typename T, void (*pUnloader)(T)>
class RaylibHandle
{
public:
	RaylibHandle()
		:m_handle({})
	{

	};
	explicit RaylibHandle(T handle)
		:m_handle(handle)
	{

	};
	~RaylibHandle()
	{
		destroy();
	}

	T& get() noexcept { return m_handle; }
	const T& get() const noexcept { return m_handle; }
	void reset(T handle) noexcept
	{
		destroy();

		m_handle = handle;
	}
	bool isHandleValid() const noexcept
	{
		if constexpr (std::is_same_v<T, Font>)
		{
			return ::IsFontValid(m_handle);
		}
		else if constexpr (std::is_same_v<T, Shader>)
		{
			return ::IsShaderValid(m_handle);
		}
		else if constexpr (std::is_same_v<T, Image>)
		{
			return ::IsImageValid(m_handle);
		}
		else if constexpr (std::is_same_v<T, Texture>)
		{
			return ::IsTextureValid(m_handle);
		}
		else if constexpr (std::is_same_v<T, RenderTexture2D>)
		{
			return ::IsRenderTextureValid(m_handle);
		}
		else if constexpr (std::is_same_v<T, FilePathList>)
		{
			return m_handle.count != 0;
		}

		return isValidGeneric();
	}

	RaylibHandle(RaylibHandle&& other) noexcept
		: m_handle(other.m_handle)
	{
		other.m_handle = {};
	}
	RaylibHandle& operator=(RaylibHandle&& other) noexcept
	{
		if (this != &other)
		{
			destroy();

			m_handle = other.m_handle;
			other.m_handle = {};
		}

		return *this;
	}

	RaylibHandle(const RaylibHandle&) = delete;
	RaylibHandle& operator=(const RaylibHandle&) = delete;
private:
	T m_handle{};

	void destroy() noexcept
	{
		if (isHandleValid() && pUnloader != nullptr)
		{
			pUnloader(m_handle);
		}
	}

	/// @brief Just in case; Preferable not to be called.
	bool isValidGeneric() const noexcept
	{
		const uint8_t* p = reinterpret_cast<const uint8_t*>(&m_handle);
		constexpr size_t handleSize = sizeof(m_handle);
		for (size_t i = 0; i < handleSize; ++i)
		{
			if (p[i] != 0x00)return true;
		}

		return false;
	}
};

using RaylibFontHandle = RaylibHandle<Font, &::UnloadFont>;
using RaylibShaderHandle = RaylibHandle<Shader, &::UnloadShader>;
using RaylibImageHandle = RaylibHandle<Image, &::UnloadImage>;
using RaylibTextureHandle = RaylibHandle<Texture, &::UnloadTexture>;
using RaylibRenderTextureHandle = RaylibHandle<RenderTexture2D, &::UnloadRenderTexture>;
using RaylibFilePathListHandle = RaylibHandle<FilePathList, &::UnloadDirectoryFiles>;

#endif // !RAYLIB_HANDLE_H_
