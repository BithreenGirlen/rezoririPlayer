#ifndef RAYLIB_INPUT_H_
#define RAYLIB_INPUT_H_

#include <raylib.h>

/// @brief C++17 style mouse/keyboard inputs
namespace raylib_input
{
	enum class EInputType : char
	{
		Unknown = -1,
		Keyboard,
		Mouse
	};

	class CInput
	{
	public:
		constexpr CInput(EInputType inputType, unsigned int code)
			:m_inputType(inputType), m_code(code)
		{
		};

		constexpr EInputType type() const noexcept;
		constexpr unsigned code() const noexcept;

		/// @brief 押されているか
		bool down() const noexcept;
		/// @brief 離されているか
		bool up() const noexcept;
		/// @brief 押されて離れた一回目か
		bool released() const noexcept;
		/// @brief 押された一回目か
		bool pressed() const noexcept;
		/// @brief 押されてから暫く経ったか否か。キーボードのみ実装
		bool pressedRepeatedly() const noexcept;
	private:
		EInputType m_inputType = EInputType::Unknown;
		unsigned int m_code = 0;
	};

	static constexpr CInput KeyApostrophe{ EInputType::Keyboard, KEY_APOSTROPHE };
	static constexpr CInput KeyComma{ EInputType::Keyboard, KEY_COMMA };
	static constexpr CInput KeyMinus{ EInputType::Keyboard, KEY_MINUS };
	static constexpr CInput KeyPeriod{ EInputType::Keyboard, KEY_PERIOD };
	static constexpr CInput KeySlash{ EInputType::Keyboard, KEY_SLASH };
	static constexpr CInput Key0{ EInputType::Keyboard, KEY_ZERO };
	static constexpr CInput Key1{ EInputType::Keyboard, KEY_ONE };
	static constexpr CInput Key2{ EInputType::Keyboard, KEY_TWO };
	static constexpr CInput Key3{ EInputType::Keyboard, KEY_THREE };
	static constexpr CInput Key4{ EInputType::Keyboard, KEY_FOUR };
	static constexpr CInput Key5{ EInputType::Keyboard, KEY_FIVE };
	static constexpr CInput Key6{ EInputType::Keyboard, KEY_SIX };
	static constexpr CInput Key7{ EInputType::Keyboard, KEY_SEVEN };
	static constexpr CInput Key8{ EInputType::Keyboard, KEY_EIGHT };
	static constexpr CInput Key9{ EInputType::Keyboard, KEY_NINE };
	static constexpr CInput KeySemicolon{ EInputType::Keyboard, KEY_SEMICOLON };
	static constexpr CInput KeyEqual{ EInputType::Keyboard, KEY_EQUAL };

	static constexpr CInput KeyA{ EInputType::Keyboard, KEY_A };
	static constexpr CInput KeyB{ EInputType::Keyboard, KEY_B };
	static constexpr CInput KeyC{ EInputType::Keyboard, KEY_C };
	static constexpr CInput KeyD{ EInputType::Keyboard, KEY_D };
	static constexpr CInput KeyE{ EInputType::Keyboard, KEY_E };
	static constexpr CInput KeyF{ EInputType::Keyboard, KEY_F };
	static constexpr CInput KeyG{ EInputType::Keyboard, KEY_G };
	static constexpr CInput KeyH{ EInputType::Keyboard, KEY_H };
	static constexpr CInput KeyI{ EInputType::Keyboard, KEY_I };
	static constexpr CInput KeyJ{ EInputType::Keyboard, KEY_J };
	static constexpr CInput KeyK{ EInputType::Keyboard, KEY_K };
	static constexpr CInput KeyL{ EInputType::Keyboard, KEY_L };
	static constexpr CInput KeyM{ EInputType::Keyboard, KEY_M };
	static constexpr CInput KeyN{ EInputType::Keyboard, KEY_N };
	static constexpr CInput KeyO{ EInputType::Keyboard, KEY_O };
	static constexpr CInput KeyP{ EInputType::Keyboard, KEY_P };
	static constexpr CInput KeyQ{ EInputType::Keyboard, KEY_Q };
	static constexpr CInput KeyR{ EInputType::Keyboard, KEY_R };
	static constexpr CInput KeyS{ EInputType::Keyboard, KEY_S };
	static constexpr CInput KeyT{ EInputType::Keyboard, KEY_T };
	static constexpr CInput KeyU{ EInputType::Keyboard, KEY_U };
	static constexpr CInput KeyV{ EInputType::Keyboard, KEY_V };
	static constexpr CInput KeyW{ EInputType::Keyboard, KEY_W };
	static constexpr CInput KeyX{ EInputType::Keyboard, KEY_X };
	static constexpr CInput KeyY{ EInputType::Keyboard, KEY_Y };
	static constexpr CInput KeyZ{ EInputType::Keyboard, KEY_Z };

	static constexpr CInput KeySpace{ EInputType::Keyboard, KEY_SPACE };
	static constexpr CInput KeyEscape{ EInputType::Keyboard, KEY_ESCAPE };
	static constexpr CInput KeyEnter{ EInputType::Keyboard, KEY_ENTER };
	static constexpr CInput KeyTab{ EInputType::Keyboard, KEY_TAB };
	static constexpr CInput KeyBackSpace{ EInputType::Keyboard, KEY_BACKSPACE };
	static constexpr CInput KeyInsert{ EInputType::Keyboard, KEY_INSERT };
	static constexpr CInput KeyDelete{ EInputType::Keyboard, KEY_DELETE };
	static constexpr CInput KeyRight{ EInputType::Keyboard, KEY_RIGHT };
	static constexpr CInput KeyLeft{ EInputType::Keyboard, KEY_LEFT };
	static constexpr CInput KeyDown{ EInputType::Keyboard, KEY_DOWN };
	static constexpr CInput KeyUp{ EInputType::Keyboard, KEY_UP };
	static constexpr CInput KeyPageUp{ EInputType::Keyboard, KEY_PAGE_UP };
	static constexpr CInput KeyPageDown{ EInputType::Keyboard, KEY_PAGE_DOWN };
	static constexpr CInput KeyHome{ EInputType::Keyboard, KEY_HOME };
	static constexpr CInput KeyEnd{ EInputType::Keyboard, KEY_END };
	static constexpr CInput KeyCapsLock{ EInputType::Keyboard, KEY_CAPS_LOCK };
	static constexpr CInput KeyScrollLock{ EInputType::Keyboard, KEY_SCROLL_LOCK };
	static constexpr CInput KeyNumLock{ EInputType::Keyboard, KEY_NUM_LOCK };
	static constexpr CInput KeyPrintScreen{ EInputType::Keyboard, KEY_PRINT_SCREEN };
	static constexpr CInput KeyPause{ EInputType::Keyboard, KEY_PAUSE };
	static constexpr CInput KeyF1{ EInputType::Keyboard, KEY_F1 };
	static constexpr CInput KeyF2{ EInputType::Keyboard, KEY_F2 };
	static constexpr CInput KeyF3{ EInputType::Keyboard, KEY_F3 };
	static constexpr CInput KeyF4{ EInputType::Keyboard, KEY_F4 };
	static constexpr CInput KeyF5{ EInputType::Keyboard, KEY_F5 };
	static constexpr CInput KeyF6{ EInputType::Keyboard, KEY_F6 };
	static constexpr CInput KeyF7{ EInputType::Keyboard, KEY_F7 };
	static constexpr CInput KeyF8{ EInputType::Keyboard, KEY_F8 };
	static constexpr CInput KeyF9{ EInputType::Keyboard, KEY_F9 };
	static constexpr CInput KeyF10{ EInputType::Keyboard, KEY_F10 };
	static constexpr CInput KeyF11{ EInputType::Keyboard, KEY_F11 };
	static constexpr CInput KeyF12{ EInputType::Keyboard, KEY_F12 };
	static constexpr CInput KeyLeftShift{ EInputType::Keyboard, KEY_LEFT_SHIFT };
	static constexpr CInput KeyLeftControl{ EInputType::Keyboard, KEY_LEFT_CONTROL };
	static constexpr CInput KeyLeftAlt{ EInputType::Keyboard, KEY_LEFT_ALT };
	static constexpr CInput KeyLeftSuper{ EInputType::Keyboard, KEY_LEFT_SUPER };
	static constexpr CInput KeyRightShift{ EInputType::Keyboard, KEY_RIGHT_SHIFT };
	static constexpr CInput KeyRightControl{ EInputType::Keyboard, KEY_RIGHT_CONTROL };
	static constexpr CInput KeyRightAlt{ EInputType::Keyboard, KEY_RIGHT_ALT };
	static constexpr CInput KeyRightSuper{ EInputType::Keyboard, KEY_RIGHT_SUPER };
	static constexpr CInput KeyKbMenu{ EInputType::Keyboard, KEY_KB_MENU };

	static constexpr CInput KeyKp0{ EInputType::Keyboard, KEY_KP_0 };
	static constexpr CInput KeyKp1{ EInputType::Keyboard, KEY_KP_1 };
	static constexpr CInput KeyKp2{ EInputType::Keyboard, KEY_KP_2 };
	static constexpr CInput KeyKp3{ EInputType::Keyboard, KEY_KP_3 };
	static constexpr CInput KeyKp4{ EInputType::Keyboard, KEY_KP_4 };
	static constexpr CInput KeyKp5{ EInputType::Keyboard, KEY_KP_5 };
	static constexpr CInput KeyKp6{ EInputType::Keyboard, KEY_KP_6 };
	static constexpr CInput KeyKp7{ EInputType::Keyboard, KEY_KP_7 };
	static constexpr CInput KeyKp8{ EInputType::Keyboard, KEY_KP_8 };
	static constexpr CInput KeyKp9{ EInputType::Keyboard, KEY_KP_9 };
	static constexpr CInput KeyKpDecimal{ EInputType::Keyboard, KEY_KP_DECIMAL };
	static constexpr CInput KeyKpDevide{ EInputType::Keyboard, KEY_KP_DIVIDE };
	static constexpr CInput KeyKpMultiply{ EInputType::Keyboard, KEY_KP_MULTIPLY };
	static constexpr CInput KeyKpSubstract{ EInputType::Keyboard, KEY_KP_SUBTRACT };
	static constexpr CInput KeyKpadd{ EInputType::Keyboard, KEY_KP_ADD };
	static constexpr CInput KeyKpEnter{ EInputType::Keyboard, KEY_KP_ENTER };
	static constexpr CInput KeyKpEqual{ EInputType::Keyboard, KEY_KP_EQUAL };

	static constexpr CInput KeyKpBack{ EInputType::Keyboard, KEY_BACK };
	static constexpr CInput KeyKpMenu{ EInputType::Keyboard, KEY_MENU };
	static constexpr CInput KeyKpVolumeUp{ EInputType::Keyboard, KEY_VOLUME_UP };
	static constexpr CInput KeyKpVolumeDown{ EInputType::Keyboard, KEY_VOLUME_DOWN };

	static constexpr CInput MouseL{ EInputType::Mouse, MOUSE_BUTTON_LEFT };
	static constexpr CInput MouseR{ EInputType::Mouse, MOUSE_BUTTON_RIGHT };
	static constexpr CInput MouseM{ EInputType::Mouse, MOUSE_BUTTON_MIDDLE };
	static constexpr CInput MouseSide{ EInputType::Mouse, MOUSE_BUTTON_SIDE };
	static constexpr CInput MouseExtra{ EInputType::Mouse, MOUSE_BUTTON_EXTRA };
	static constexpr CInput MouseForward{ EInputType::Mouse, MOUSE_BUTTON_FORWARD };
	static constexpr CInput MouseBack{ EInputType::Mouse, MOUSE_BUTTON_BACK };
} /* namespace raylib_input */

#endif // !RAYLIB_INPUT_H_
