#ifndef RAYLIB_SPINE_PLAYER_H_
#define RAYLIB_SPINE_PLAYER_H_

#include "spine_player.h"

class CRaylibSpinePlayer : public CSpinePlayer
{
public:
	CRaylibSpinePlayer() = default;
	virtual ~CRaylibSpinePlayer() = default;

	void redraw(const float scale = 1.f, const float offsetX = 0.f, const float offsetY = 0.f);

	/// @brief Work out scale and offsets to be applied to vertices so as to be within the target rectangle 
	/// @return Vector3 in the order of scale, offset X, offset Y
	Vector3 calculateTransformScaleAndOffsets(int targetWidth, int targetHeight) const noexcept;
	Rectangle getCurrentBoundingOfSlot(const char* slotName, size_t nameLength) const;
	template<size_t nameSize>
	Rectangle getCurrentBoundingOfSlot(const char(&slotName)[nameSize]) const
	{
		return getCurrentBoundingOfSlot(slotName, nameSize - 1);
	}
private:
	void workOutDefaultScale() noexcept override;
	bool workOutDefaultSizeAndOffset() override;
};

#endif // !RAYLIB_SPINE_PLAYER_H_

