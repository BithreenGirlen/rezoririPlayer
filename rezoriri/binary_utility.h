#ifndef BINARY_UTILITY_H_
#define BINARY_UTILITY_H_

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L)
	#define ALLOW_USING_STRING_VIEW
	#include <string_view>
#else
	#include <stdint.h>
#endif

namespace binary_utility
{
	static inline uint16_t ToUInt16Le(const uint8_t* p)
	{
		return p[0] | (p[1] << 8);
	}

	static inline uint32_t ToUInt32Le(const uint8_t* p)
	{
		return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
	}

	static inline uint64_t ToUInt64Le(const uint8_t* p)
	{
		return static_cast<uint64_t>(p[0])
			| (static_cast<uint64_t>(p[1]) << 8)
			| (static_cast<uint64_t>(p[2]) << 16)
			| (static_cast<uint64_t>(p[3]) << 24)
			| (static_cast<uint64_t>(p[4]) << 32)
			| (static_cast<uint64_t>(p[5]) << 40)
			| (static_cast<uint64_t>(p[6]) << 48)
			| (static_cast<uint64_t>(p[7]) << 56);
	}

	static inline uint16_t ToUInt16Be(const uint8_t* p)
	{
		return p[1] | (p[0] << 8);
	}

	static inline uint32_t ToUInt32Be(const uint8_t* p)
	{
		return p[3] | (p[2] << 8) | (p[1] << 16) | (p[0] << 24);
	}

	static inline uint64_t ToUInt64Be(const uint8_t* p)
	{
		return static_cast<uint64_t>(p[7])
			| (static_cast<uint64_t>(p[6]) << 8)
			| (static_cast<uint64_t>(p[5]) << 16)
			| (static_cast<uint64_t>(p[4]) << 24)
			| (static_cast<uint64_t>(p[3]) << 32)
			| (static_cast<uint64_t>(p[2]) << 40)
			| (static_cast<uint64_t>(p[1]) << 48)
			| (static_cast<uint64_t>(p[0]) << 56);
	}

	class CBinaryReader
	{
	public:
		CBinaryReader(const char* pData, size_t dataLength)
			: m_pData(reinterpret_cast<const uint8_t*>(pData)), m_nSize(dataLength)
		{
		}
		CBinaryReader(const unsigned char* pData, size_t dataLength)
			: m_pData(pData), m_nSize(dataLength)
		{
		}

		~CBinaryReader() = default;

		uint8_t readByte()
		{
			uint8_t v = *(m_pData + m_nRead);
			m_nRead += 1;

			return v;
		}

		uint16_t readUInt16Le()
		{
			uint16_t v = ToUInt16Le(m_pData + m_nRead);
			m_nRead += 2;

			return v;
		}

		uint32_t readUInt32Le()
		{
			uint32_t v = ToUInt32Le(m_pData + m_nRead);
			m_nRead += 4;

			return v;
		}

		uint64_t readUInt64Le()
		{
			uint64_t v = ToUInt64Le(m_pData + m_nRead);
			m_nRead += 8;

			return v;
		}

		uint16_t readUInt16Be()
		{
			uint16_t v = ToUInt16Be(m_pData + m_nRead);
			m_nRead += 2;

			return v;
		}

		uint32_t readUInt32Be()
		{
			uint32_t v = ToUInt32Be(m_pData + m_nRead);
			m_nRead += 4;

			return v;
		}

		uint64_t readUInt64Be()
		{
			uint64_t v = ToUInt64Be(m_pData + m_nRead);
			m_nRead += 8;

			return v;
		}
#if defined(ALLOW_USING_STRING_VIEW)
		/// @brief 1バイト文字列長と後続する文字列読み取り
		std::string_view readString1()
		{
			uint32_t length = readByte();
			std::string_view s(reinterpret_cast<const char*>(m_pData + m_nRead), length);
			m_nRead += length;

			return s;
		}
		/// @brief 4バイト文字列長と後続する文字列読み取り
		std::string_view readString4()
		{
			uint32_t length = readUInt32Le();
			std::string_view s(reinterpret_cast<const char*>(m_pData + m_nRead), length);
			m_nRead += length;

			return s;
		}
		/// @brief 終端有り文字列読み取り
		std::string_view readNullterminatedString()
		{
			const uint8_t* pEnd = std::find(m_pData + m_nRead, m_pData + m_nSize, '\0');
			if (pEnd == nullptr)return {};

			size_t length = pEnd - (m_pData + m_nRead);
			std::string_view s(reinterpret_cast<const char*>(m_pData + m_nRead), length);
			m_nRead += length + 1;

			return s;
		}
#endif /* C++17 std::string__view */

		void skip(size_t length)
		{
			m_nRead += length;
		}

		bool setPos(size_t pos)
		{
			if (pos >= m_nSize)return false;
			m_nRead = pos;

			return true;
		}

		size_t pos() const noexcept { return m_nRead; }
		size_t size() const noexcept { return m_nSize; }
	private:
		const uint8_t* m_pData = nullptr;
		size_t m_nRead = 0;
		size_t m_nSize = 0;
	};
}

#endif // !BINARY_UTILITY_H_
