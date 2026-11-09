

#include "raylib_spine_player.h"


void CRaylibSpinePlayer::redraw(const float scale, const float offsetX, const float offsetY)
{
	if (!m_drawables.empty())
	{
		if (!m_isDrawOrderReversed)
		{
			for (size_t i = 0; i < m_drawables.size(); ++i)
			{
				m_drawables[i]->draw(scale, offsetX, offsetY);
			}
		}
		else
		{
			for (long long i = m_drawables.size() - 1; i >= 0; --i)
			{
				m_drawables[i]->draw(scale, offsetX, offsetY);
			}
		}
	}
}

Vector3 CRaylibSpinePlayer::calculateTransformScaleAndOffsets(int targetWidth, int targetHeight) const noexcept
{
	return Vector3(
		m_fSkeletonScale,
		(targetWidth - m_fBaseSize.x * m_fSkeletonScale) / 2.f,
		(targetHeight - m_fBaseSize.y * m_fSkeletonScale) / 2.f);
}

Rectangle CRaylibSpinePlayer::getCurrentBoundingOfSlot(const char* slotName, size_t nameLength) const
{
	bool found = false;
	for (const auto& drawable : m_drawables)
	{
		const auto& rect = drawable->getBoundingBoxOfSlot(slotName, nameLength, &found);
		if (found)
		{
			return rect;
		}
	}

	return {};
}

void CRaylibSpinePlayer::workOutDefaultScale() noexcept
{
	m_fDefaultScale = 1.f;

	const int skeletonWidth = static_cast<int>(m_fBaseSize.x);
	const int skeletonHeight = static_cast<int>(m_fBaseSize.y);

	const int monitor = ::GetCurrentMonitor();

	const int desktopWidth = ::GetMonitorWidth(monitor);
	const int desktopHeight = ::GetMonitorHeight(monitor);

	if (skeletonWidth > desktopWidth || skeletonHeight > desktopHeight)
	{
		const float scaleX = static_cast<float>(desktopWidth) / skeletonWidth;
		const float scaleY = static_cast<float>(desktopHeight) / skeletonHeight;

		m_fDefaultScale = (scaleX < scaleY) ? scaleX : scaleY;
	}
}

bool CRaylibSpinePlayer::workOutDefaultSizeAndOffset()
{
	static constexpr float kMaxDimension = 16384.f;

	float fMinX = FLT_MAX;
	float fMinY = FLT_MAX;
	float fWidth = -FLT_MAX;
	float fHeight = -FLT_MAX;

	for (const auto& pDrawable : m_drawables)
	{
		const auto& rect = pDrawable->getBoundingBox();
		fMinX = (std::min)(fMinX, rect.x);
		fMinY = (std::min)(fMinY, rect.y);

		if (::isless(rect.width, kMaxDimension) && ::isless(rect.height, kMaxDimension))
		{
			fWidth = (std::max)(fWidth, rect.width);
			fHeight = (std::max)(fHeight, rect.height);
		}
	}

	if (::isless(fMinX, FLT_MAX) && ::isless(fMinY, FLT_MAX))
	{
		m_fDefaultOffset.x = fMinX;
		m_fDefaultOffset.y = fMinY;
	}

	if (::isgreater(fWidth, -FLT_MAX) && ::isgreater(fHeight, -FLT_MAX))
	{
		m_fBaseSize.x = fWidth;
		m_fBaseSize.y = fHeight;

		return true;
	}

	return false;
}
