#ifndef MF_MEDIA_PLAYER_H_
#define MF_MEDIA_PLAYER_H_

/* In other projects, I do not make this pImpl; just to avoid naming conflict with <raylib.h> and <Windows.h> */

class CMfMediaPlayer
{
public:
	CMfMediaPlayer();
	virtual ~CMfMediaPlayer();

	bool play(const wchar_t* filePath);

	bool setLoop(bool looped);
	bool isLooped() const noexcept;

	bool setMute(bool muted);
	bool isMuted() const noexcept;

	bool setPause(bool paused);
	bool isPaused() const noexcept;

	bool isEnded() const noexcept;

	/// @brief Step one frame forward/backward
	bool frameStep(bool forward);

	bool setCurrentVolume(double dbVolume);
	double getCurrentVolume() const noexcept;

	bool setCurrentRate(double dbRate);
	double getCurrentRate() const noexcept;

	long long getCurrentTimeInMilliSeconds();

	/// @brief Set window and message code to receive event notification.
	virtual bool setPlaybackWindow(void* hWnd, unsigned int uMsg = 0);

	void* getRetHwnd() const noexcept;
	unsigned int getRetMsg() const noexcept;

	class Impl;
protected:
	Impl* m_pImpl = nullptr;
};

#endif // !MF_MEDIA_PLAYER_H_
