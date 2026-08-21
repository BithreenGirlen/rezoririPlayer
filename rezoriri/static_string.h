#ifndef STATIC_STRING_H_
#define STATIC_STRING_H_

#include <string_view>

/// @brief 動的割り当てを行わない文字列操作
template<typename T, size_t N>
class StaticString
{
public:
	constexpr StaticString() noexcept = default;
	template <size_t M>
	constexpr StaticString(const T(&s)[M]) noexcept
	{
		constexpr size_t length = M - 1;
		static_assert(length <= MaxSize, "String literal is longer than the capacity of StaticString.");

		for (size_t i = 0; i < length; ++i)
		{
			m_data[i] = s[i];
		}

		m_nWritten = length;
		m_data[m_nWritten] = s_termination;
	}

	constexpr const T* data() const noexcept { return m_data; }
	constexpr size_t size() const noexcept { return m_nWritten; }
	constexpr size_t available() const noexcept { return MaxSize - m_nWritten; }
	constexpr size_t capacity() const noexcept { return MaxSize; }
	constexpr bool empty() const noexcept { return m_nWritten == 0; }
	constexpr const T& front() const noexcept { return m_data[0]; }
	constexpr const T& back() const noexcept { return m_nWritten == 0 ? m_data[0] : m_data[m_nWritten - 1]; }

	constexpr std::basic_string_view<T> string_view() const noexcept
	{
		return std::basic_string_view<T>(m_data, m_nWritten);
	}

	/// @brief 文字列割り当て
	StaticString& assign(std::basic_string_view<T> s) noexcept
	{
		if (s.size() > MaxSize)return *this;

		::memcpy(m_data, s.data(), s.size() * sizeof(T));
		m_nWritten = s.size();
		m_data[m_nWritten] = s_termination;

		return *this;
	}
	StaticString& assign(const StaticString& s) noexcept
	{
		if (s.size() > MaxSize)return *this;

		::memcpy(m_data, s.data(), s.size() * sizeof(T));
		m_nWritten = s.size();
		m_data[m_nWritten] = s_termination;

		return *this;
	}
	StaticString& assign(const T* s, size_t length) noexcept
	{
		if (length > MaxSize)return *this;

		::memcpy(m_data, s, length * sizeof(T));
		m_nWritten = length;
		m_data[m_nWritten] = s_termination;

		return *this;
	}

	/// @brief 文字列連結
	StaticString& append(std::basic_string_view<T> s) noexcept
	{
		if (m_nWritten + s.size() > MaxSize)return *this;

		::memcpy(m_data + m_nWritten, s.data(), s.size() * sizeof(T));
		m_nWritten += s.size();
		m_data[m_nWritten] = s_termination;

		return *this;
	}
	StaticString& append(const T* s, size_t length) noexcept
	{
		if (m_nWritten + length > MaxSize)return *this;

		::memcpy(m_data + m_nWritten, s, length * sizeof(T));
		m_nWritten += length;
		m_data[m_nWritten] = s_termination;

		return *this;
	}

	/// @brief 文字連結
	void push_back(const T c) noexcept
	{
		if (m_nWritten + 1 > MaxSize)return;

		m_data[m_nWritten] = c;
		++m_nWritten;
		m_data[m_nWritten] = s_termination;
	}

	/// @brief 文字挿入
	void insert(const T c, size_t nPos = 0) noexcept
	{
		if (m_nWritten + 1 > MaxSize)return;

		::memmove(&m_data[nPos + 1], &m_data[nPos], (m_nWritten - nPos) * sizeof(T));
		m_data[nPos] = c;
		++m_nWritten;
	}
	/// @brief 文字列挿入
	void insert(std::basic_string_view<T> s, size_t nPos = 0) noexcept
	{
		if (s.size() + nPos > MaxSize)return;

		::memmove(&m_data[nPos + s.size()], &m_data[nPos], (m_nWritten - nPos) * sizeof(T));
		::memcpy(&m_data[nPos], s.data(), s.size() * sizeof(T));
		m_nWritten += s.size();
	}
	/// @brief 文字置換
	void replace(const T cOld, const T cNew) noexcept
	{
		for (size_t i = 0; i < m_nWritten; ++i)
		{
			T& cRef = m_data[i];
			if (cRef == cOld)
			{
				cRef = cNew;
			}
		}
	}
	/// @brief 文字列置換
	void replace(std::basic_string_view<T> strOld, std::basic_string_view<T> strNew) noexcept
	{
		if (strOld.empty())return;

		for (size_t nLast = 0; nLast < m_nWritten;)
		{
			std::basic_string_view<T> s = string_view();
			size_t nPos = s.find(strOld, nLast);
			if (nPos == std::basic_string_view<T>::npos)break;

			ptrdiff_t nDiff = static_cast<ptrdiff_t>(strNew.size() - strOld.size());
			if (m_nWritten + nDiff > MaxSize) break;

			T* pPos = m_data + nPos;
			::memmove(pPos + strNew.size(), pPos + strOld.size(), (m_nWritten - nPos - strOld.size() + 1) * sizeof(T));
			::memcpy(pPos, strNew.data(), strNew.size() * sizeof(T));

			m_nWritten += nDiff;
			nLast = nPos + strNew.size();
		}
	}
	/// @brief 消去
	void clear() noexcept
	{
		::memset(m_data, s_termination, MaxSize * sizeof(T));
		m_nWritten = 0;
	}
	/// @brief 縮め
	void shrink(size_t nLength) noexcept
	{
		if (nLength >= m_nWritten)return;

		::memset(m_data + nLength, s_termination, (MaxSize - nLength) * sizeof(T));
		m_nWritten = nLength;
	}
	void resize(size_t nLength) noexcept
	{
		if (nLength >= MaxSize)return;

		m_nWritten = nLength;
		m_data[m_nWritten] = s_termination;
	}

	T& operator[](const size_t nPos) noexcept
	{
		return m_data[nPos];
	}

	constexpr bool operator==(const StaticString& rhs) const noexcept
	{
		return string_view() == rhs.string_view();
	}

private:
	T m_data[N]{};
	size_t m_nWritten = 0;
	static constexpr size_t MaxSize = sizeof(m_data) / sizeof(T) - 1;

	static constexpr T s_termination = []()
		-> T
		{
			if constexpr (std::is_same_v<T, char>) return '\0';
			else if constexpr (std::is_same_v<T, wchar_t>) return L'\0';
			else if constexpr (std::is_same_v<T, char16_t>) return u'\0';
			else if constexpr (std::is_same_v<T, char32_t>) return U'\0';
		}();
};


using StaticString512 = StaticString<char, 512>;
using StaticWString512 = StaticString<wchar_t, 512>;

#endif // !STATIC_STRING_H_
