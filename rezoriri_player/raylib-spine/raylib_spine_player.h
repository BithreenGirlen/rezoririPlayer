#ifndef RAYLIB_SPINE_PLAYER_H_
#define RAYLIB_SPINE_PLAYER_H_

#include "spine_player.h"

class CRaylibSpinePlayer : public CSpinePlayer
{
public:
	CRaylibSpinePlayer() = default;
	virtual ~CRaylibSpinePlayer() = default;

	void redraw(const RenderTexture2D* pRenderTarget);

	Rectangle getCurrentBoundingOfSlot(const char* slotName, size_t nameLength) const;
	template<size_t nameSize>
	Rectangle getCurrentBoundingOfSlot(const char(&slotName)[nameSize]) const
	{
		return getCurrentBoundingOfSlot(slotName, nameSize - 1);
	}
private:
	void workOutDefaultScale() override;
	void workOutDefaultSizeAndOffset() override;
};

#endif // !RAYLIB_SPINE_PLAYER_H_

