
#include "raylib_clock.h"

#include <raylib.h>

CRaylibClock::CRaylibClock()
{
	restart();
}

float CRaylibClock::getElapsedTime()
{
	double nowTime = getTime();

	return static_cast<float>(nowTime - m_lastTime);
}

void CRaylibClock::restart()
{
	m_lastTime = getTime();
}

double CRaylibClock::getTime()
{
	return ::GetTime();
}
