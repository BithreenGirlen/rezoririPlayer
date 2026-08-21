#ifndef RAYLIB_CLOCK_H_
#define RAYLIB_CLOCK_H_

/// @brief 経過時間測定
class CRaylibClock
{
public:
	CRaylibClock();
	~CRaylibClock() = default;

	/// @brief 秒単位の経過時間を取得
	float getElapsedTime();
	/// @brief 再計測開始
	void restart();
private:
	double m_lastTime = 0.0;

	double getTime();
};

#endif // !RAYLIB_CLOCK_H_
