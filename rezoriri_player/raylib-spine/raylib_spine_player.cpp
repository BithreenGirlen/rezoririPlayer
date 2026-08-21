

#include "raylib_spine_player.h"


void CRaylibSpinePlayer::redraw(const RenderTexture2D* pRenderTarget)
{
	if (m_drawables.empty())return;

	int targetWidth = 0, targetHeight = 0;
	if (pRenderTarget == nullptr)
	{
		targetWidth = ::GetScreenWidth();
		targetHeight = ::GetScreenHeight();
	}
	else
	{
		targetWidth = pRenderTarget->texture.width;
		targetHeight = pRenderTarget->texture.height;
	}

	float fX = (m_fBaseSize.x * m_fSkeletonScale - targetWidth) / 2.f;
	float fY = (m_fBaseSize.y * m_fSkeletonScale - targetHeight) / 2.f;

	if (!m_isDrawOrderReversed)
	{
		for (size_t i = 0; i < m_drawables.size(); ++i)
		{
			m_drawables[i]->draw(m_fSkeletonScale, -fX, -fY);
		}
	}
	else
	{
		for (long long i = m_drawables.size() - 1; i >= 0; --i)
		{
			m_drawables[i]->draw(m_fSkeletonScale, -fX, -fY);
		}
	}
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

void CRaylibSpinePlayer::workOutDefaultScale()
{
	m_fDefaultScale = 1.f;

	int skeletonWidth = static_cast<int>(m_fBaseSize.x);
	int skeletonHeight = static_cast<int>(m_fBaseSize.y);

	int monitor = ::GetCurrentMonitor();

	int desktopWidth = ::GetMonitorWidth(monitor);
	int desktopHeight = ::GetMonitorHeight(monitor);

	if (skeletonWidth > desktopWidth || skeletonHeight > desktopHeight)
	{
		float scaleX = static_cast<float>(desktopWidth) / skeletonWidth;
		float scaleY = static_cast<float>(desktopHeight) / skeletonHeight;

		m_fDefaultScale = (scaleX < scaleY) ? scaleX : scaleY;
	}
}

void CRaylibSpinePlayer::workOutDefaultSizeAndOffset()
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
	}
}
