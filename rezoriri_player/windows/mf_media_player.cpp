
#include <Windows.h>
#include <mfmediaengine.h>
#include <atlbase.h>
#include <mfapi.h>

#include "mf_media_player.h"

#pragma comment (lib,"Mfplat.lib")


class CMfMediaPlayerNotify : public IMFMediaEngineNotify
{
public:
	CMfMediaPlayerNotify(void* pMediaPlayer);
	~CMfMediaPlayerNotify() = default;

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (riid == __uuidof(IMFMediaEngineNotify))
		{
			*ppv = static_cast<IMFMediaEngineNotify*>(this);
		}
		else
		{
			*ppv = nullptr;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}
	STDMETHODIMP EventNotify(DWORD Event, DWORD_PTR param1, DWORD param2)
	{
		if (Event == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
		{
			::SetEvent(reinterpret_cast<HANDLE>(param1));
		}
		else
		{
			onMediaEngineEvent(Event, param1, param2);
		}
		return S_OK;
	}
	STDMETHODIMP_(ULONG) AddRef() { return ::InterlockedIncrement(&m_lRef); }
	STDMETHODIMP_(ULONG) Release()
	{
		LONG lRef = ::InterlockedDecrement(&m_lRef);
		if (!lRef) delete this;
		return lRef;
	}

private:
	LONG m_lRef = 0;
	void* m_pMediaPlayer = nullptr;

	void onMediaEngineEvent(DWORD Event, DWORD_PTR param1, DWORD param2);
};


class CMfMediaPlayer::Impl
{
public:
	Impl();
	virtual ~Impl();

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
	virtual bool setPlaybackWindow(HWND hWnd, UINT uMsg = 0);

	HWND getRetHwnd()const { return m_hRetWnd; }
	UINT getRetMsg() const { return m_uRetMsg; }
protected:
	HWND m_hRetWnd = nullptr;
	UINT m_uRetMsg = 0;

	HRESULT m_hrComInit = E_FAIL;
	HRESULT m_hrMfStart = E_FAIL;

	CMfMediaPlayerNotify* m_pMfNotify = nullptr;
	IMFMediaEngineEx* m_pMfEngineEx = nullptr;
	IMFAttributes* m_pMfAttributes = nullptr;
};


CMfMediaPlayerNotify::CMfMediaPlayerNotify(void* pMediaPlayer)
	:m_pMediaPlayer(pMediaPlayer)
{

}

void CMfMediaPlayerNotify::onMediaEngineEvent(DWORD Event, DWORD_PTR param1, DWORD param2)
{
	CMfMediaPlayer::Impl* pMediaPlayer = static_cast<CMfMediaPlayer::Impl*>(m_pMediaPlayer);
	if (pMediaPlayer != nullptr)
	{
		const HWND hWnd = static_cast<HWND>(pMediaPlayer->getRetHwnd());
		const UINT uMsg = pMediaPlayer->getRetMsg();
		if (hWnd != nullptr && uMsg != 0)
		{
			::PostMessage(hWnd, uMsg, param1, Event);
		}
	}
}

CMfMediaPlayer::Impl::Impl()
{
	m_hrComInit = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(m_hrComInit))return;

	m_hrMfStart = ::MFStartup(MF_VERSION);
	if (FAILED(m_hrMfStart))return;

	CComPtr<IMFMediaEngineClassFactory> pMfFactory;
	CComPtr<IMFMediaEngine> pMfMediaEngine;

	HRESULT hr = ::CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pMfFactory));
	if (FAILED(hr))return;

	hr = ::MFCreateAttributes(&m_pMfAttributes, 1);
	if (FAILED(hr))return;

	m_pMfNotify = new CMfMediaPlayerNotify(this);
	hr = m_pMfAttributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(m_pMfNotify));
	if (FAILED(hr))goto failed;

	hr = pMfFactory->CreateInstance(MF_MEDIA_ENGINE_REAL_TIME_MODE, m_pMfAttributes, &pMfMediaEngine);
	if (FAILED(hr))goto failed;

	hr = pMfMediaEngine->QueryInterface(__uuidof(IMFMediaEngineEx), (void**)&m_pMfEngineEx);
	if (FAILED(hr))goto failed;

	m_pMfEngineEx->SetVolume(0.5);
	m_pMfEngineEx->SetPreload(MF_MEDIA_ENGINE_PRELOAD_METADATA);

	return;
failed:
	if (m_pMfNotify != nullptr)
	{
		m_pMfNotify->Release();
		m_pMfNotify = nullptr;
	}
	if (m_pMfAttributes != nullptr)
	{
		m_pMfAttributes->Release();
		m_pMfAttributes = nullptr;
	}
}

CMfMediaPlayer::Impl::~Impl()
{
	if (m_pMfAttributes != nullptr)
	{
		m_pMfAttributes->Release();
		m_pMfAttributes = nullptr;
	}
	if (m_pMfEngineEx != nullptr)
	{
		m_pMfEngineEx->Shutdown();
		m_pMfEngineEx = nullptr;
	}

	if (SUCCEEDED(m_hrMfStart))
	{
		::MFShutdown();
		m_hrMfStart = E_FAIL;
	}

	if (SUCCEEDED(m_hrComInit))
	{
		::CoUninitialize();
		m_hrComInit = E_FAIL;
	}
}


bool CMfMediaPlayer::Impl::play(const wchar_t* filePath)
{
	if (m_pMfEngineEx != nullptr)
	{
		if (filePath != nullptr)
		{
			HRESULT hr = m_pMfEngineEx->SetSource(const_cast<BSTR>(filePath));
		}
		return SUCCEEDED(m_pMfEngineEx->Play());
	}
	return false;
}

bool CMfMediaPlayer::Impl::setLoop(bool looped)
{
	if (m_pMfEngineEx != nullptr)
	{
		HRESULT hr = m_pMfEngineEx->SetLoop(looped ? TRUE : FALSE);

		return SUCCEEDED(hr);
	}

	return false;
}

bool CMfMediaPlayer::Impl::isLooped() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->GetLoop() == TRUE;
	}

	return false;
}

bool CMfMediaPlayer::Impl::setMute(bool muted)
{
	if (m_pMfEngineEx != nullptr)
	{
		HRESULT hr = m_pMfEngineEx->SetMuted(muted ? TRUE : FALSE);

		return SUCCEEDED(hr);
	}

	return false;
}

bool CMfMediaPlayer::Impl::isMuted() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->GetMuted() == TRUE;
	}

	return false;
}

bool CMfMediaPlayer::Impl::setPause(bool paused)
{
	if (m_pMfEngineEx != nullptr)
	{
		HRESULT hr = paused ? m_pMfEngineEx->Pause() : m_pMfEngineEx->Play();
		if (SUCCEEDED(hr))
		{
			if (paused)
			{
				frameStep(true);
			}

			return true;
		}
	}

	return false;
}

bool CMfMediaPlayer::Impl::isPaused() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->IsPaused() == TRUE;
	}

	return false;
}

bool CMfMediaPlayer::Impl::isEnded() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		BOOL iRet = m_pMfEngineEx->HasAudio();
		iRet |= m_pMfEngineEx->HasVideo();
		if (!iRet)
		{
			return true;
		}
		else
		{
			return m_pMfEngineEx->IsEnded() == TRUE;
		}
	}

	return true;
}

bool CMfMediaPlayer::Impl::frameStep(bool forward)
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->FrameStep(forward ? TRUE : FALSE) == TRUE;
	}

	return false;
}

bool CMfMediaPlayer::Impl::setCurrentVolume(double dbVolume)
{
	if (m_pMfEngineEx != nullptr)
	{
		return SUCCEEDED(m_pMfEngineEx->SetVolume(dbVolume));
	}

	return false;
}

double CMfMediaPlayer::Impl::getCurrentVolume() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->GetVolume();
	}

	return 100.0;
}

bool CMfMediaPlayer::Impl::setCurrentRate(double dbRate) 
{
	if (m_pMfEngineEx != nullptr)
	{
		if (dbRate != m_pMfEngineEx->GetDefaultPlaybackRate())
		{
			m_pMfEngineEx->SetPlaybackRate(dbRate);
		}
		return SUCCEEDED(m_pMfEngineEx->SetDefaultPlaybackRate(dbRate));
	}
	return false;
}

double CMfMediaPlayer::Impl::getCurrentRate() const noexcept
{
	if (m_pMfEngineEx != nullptr)
	{
		return m_pMfEngineEx->GetPlaybackRate();
	}

	return 1.0;
}

long long CMfMediaPlayer::Impl::getCurrentTimeInMilliSeconds()
{
	if (m_pMfEngineEx != nullptr)
	{
		double dbTime = m_pMfEngineEx->GetCurrentTime();
		return static_cast<long long>(::round(dbTime * 1000));
	}

	return 0;
}

bool CMfMediaPlayer::Impl::setPlaybackWindow(HWND hWnd, UINT uMsg)
{
	m_hRetWnd = hWnd;
	m_uRetMsg = uMsg;

	HRESULT hr = m_pMfAttributes->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND, reinterpret_cast<UINT64>(m_hRetWnd));
	return SUCCEEDED(hr);
}



CMfMediaPlayer::CMfMediaPlayer()
{
	m_pImpl = new Impl();
}

CMfMediaPlayer::~CMfMediaPlayer()
{
	delete m_pImpl;
}

bool CMfMediaPlayer::play(const wchar_t* filePath)
{
	return m_pImpl->play(filePath);
}

bool CMfMediaPlayer::setLoop(bool looped)
{
	return m_pImpl->setLoop(looped);
}

bool CMfMediaPlayer::isLooped() const noexcept
{
	return m_pImpl->isLooped();
}

bool CMfMediaPlayer::setMute(bool muted)
{
	return m_pImpl->setMute(muted);
}

bool CMfMediaPlayer::isMuted() const noexcept
{
	return m_pImpl->isMuted();
}

bool CMfMediaPlayer::setPause(bool paused)
{
	return m_pImpl->setMute(paused);
}

bool CMfMediaPlayer::isPaused() const noexcept
{
	return m_pImpl->isPaused();
}

bool CMfMediaPlayer::isEnded() const noexcept
{
	return m_pImpl->isEnded();
}

bool CMfMediaPlayer::frameStep(bool forward)
{
	return m_pImpl->frameStep(forward);
}

bool CMfMediaPlayer::setCurrentVolume(double dbVolume)
{
	return m_pImpl->setCurrentVolume(dbVolume);
}

double CMfMediaPlayer::getCurrentVolume() const noexcept
{
	return m_pImpl->getCurrentVolume();
}

bool CMfMediaPlayer::setCurrentRate(double dbRate)
{
	return m_pImpl->setCurrentRate(dbRate);
}

double CMfMediaPlayer::getCurrentRate() const noexcept
{
	return m_pImpl->getCurrentRate();
}

long long CMfMediaPlayer::getCurrentTimeInMilliSeconds()
{
	return m_pImpl->getCurrentTimeInMilliSeconds();
}

bool CMfMediaPlayer::setPlaybackWindow(void* hWnd, unsigned int uMsg)
{
	return m_pImpl->setPlaybackWindow(static_cast<HWND>(hWnd), uMsg);
}

void* CMfMediaPlayer::getRetHwnd() const noexcept
{
	return m_pImpl->getRetHwnd();
}

unsigned int CMfMediaPlayer::getRetMsg() const noexcept
{
	return m_pImpl->getRetMsg();
}
