#ifndef RAYLIB_SPINE_PLAYER_H_
#define RAYLIB_SPINE_PLAYER_H_

#include "spine_player.h"

/*
* The base class CSpinePlayer handles manipulation of Spine.
* This derived class is to retrive platform/library dependent information like monitor resolution,
* and also explicitly accesses library-dependent structs/classes like vector, rectangle, or render-target.
* 
* How to play Spine:
* (1) clearDrawables() to clear already loaded Spines.
* (2) Iterate loadSpineFromFile() or loadSpineFromMemory()
*     until all the Spines which should be synchronically drawn be loaded.
* (3.a) If the size should be fit to the whole bounding-box, call resetBaseSize().
* (3.b) If it should have specific size, call setOffset() and setBaseSize().
* (4.a) If the first animation should be played, call restartAnimation().
*       - this is not necessary if resetBaseSize() had been called.
* (4.b) If an animation other than the first should be played, call setAnimationByName().
*
* Then call update() and redraw() every frame.
*/
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
