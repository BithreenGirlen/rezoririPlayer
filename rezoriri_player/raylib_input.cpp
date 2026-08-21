

#include "raylib_input.h"

namespace raylib_input
{
	constexpr EInputType CInput::type() const noexcept
	{
		return m_inputType;
	}

	constexpr unsigned int CInput::code() const noexcept
	{
		return m_code;
	}

	bool CInput::down() const noexcept
	{
		switch (m_inputType)
		{
		case EInputType::Keyboard:
			return ::IsKeyDown(m_code);
		case EInputType::Mouse:
			return ::IsMouseButtonDown(m_code);
		default:
			return false;
		}
	}
	bool CInput::up() const noexcept
	{
		switch (m_inputType)
		{
		case EInputType::Keyboard:
			return ::IsKeyUp(m_code);
		case EInputType::Mouse:
			return ::IsMouseButtonUp(m_code);
		default:
			return false;
		}
	}
	bool CInput::released() const noexcept
	{
		switch (m_inputType)
		{
		case EInputType::Keyboard:
			return ::IsKeyReleased(m_code);
		case EInputType::Mouse:
			return ::IsMouseButtonReleased(m_code);
		default:
			return false;
		}
	}
	bool CInput::pressed() const noexcept
	{
		switch (m_inputType)
		{
		case EInputType::Keyboard:
			return ::IsKeyPressed(m_code);
		case EInputType::Mouse:
			return ::IsMouseButtonPressed(m_code);
		default:
			return false;
		}
	}
	bool CInput::pressedRepeatedly() const noexcept
	{
		switch (m_inputType)
		{
		case EInputType::Keyboard:
			return ::IsKeyPressedRepeat(m_code);
		default:
			return false;
		}
	}
}