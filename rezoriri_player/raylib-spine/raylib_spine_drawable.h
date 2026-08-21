#ifndef RAYLIB_SPINE_DRAWABLE_H_
#define RAYLIB_SPINE_DRAWABLE_H_

/* Avoid conflict between <MathUtils.h> and <Windows.h> */
#if defined(min)
	#undef min
#endif
#if defined(max)
	#undef max
#endif
#include <spine/spine.h>

#include <raylib.h>

class CRaylibSpineDrawable
{
public:
	CRaylibSpineDrawable(spine::SkeletonData* pSkeletonData);
	~CRaylibSpineDrawable();

	spine::Skeleton* skeleton() const noexcept;
	spine::AnimationState* animationState() const noexcept;

	/// @brief 乗算済みアルファ適用有無。Spine 3.8より古い場合のみ有効で、4.0からはAtlasPageの同値を参照するため手動変更不可
	void premultiplyAlpha(bool premultiplied) noexcept;
	bool isAlphaPremultiplied() const noexcept;

	/// @brief スロット指定の混色法を無視して通常混色法を適用するか否か
	void forceBlendModeNormal(bool toForce) noexcept;
	bool isBlendModeNormalForced() const noexcept;

	/// @brief 時間更新を行うか。
	void setPause(bool paused) noexcept;
	bool isPaused() const noexcept;

	/// @brief 描画を行うか否か
	void setVisibility(bool visible) noexcept;
	bool isVisible() const noexcept;

	/// @brief 物理演算法
	enum class Physics : unsigned char
	{
		None = 0, /* 物理演算を行わない */
		Reset, /* 1フレーム前までの影響をリセットして新たに物理演算を開始する */
		Update, /* 物理演算を行い、通算の影響を反映させる */
		Pose /* 1フレーム前の状態で静止させる */
	};

	/// @brief 物理演算方法指定。Spine4.2以降でのみ有効
	void setPhysics(Physics physics);
	Physics getPhysics() const noexcept;

	/// @brief 時間の加算並びワールド座標の更新
	/// @param fDelta 加算すべき時間(秒単位)
	/// @remark 停止中や0.0fの時間加算であっても座標更新は行う
	void update(float fDelta);
	void draw(float fScale = 1.f, float fOffsetX = 0.f, float fOffsetY = 0.f);

	/// @brief 描画対象から除外するスロットを設定
	void setSlotsToExclude(spine::Vector<spine::String>& slotNames);
	void setSlotExcludeCallback(bool (*pFunc)(const char*, size_t)) { m_pSlotExcludeCallback = pFunc; }

	/// @brief 全体の境界矩形を算出
	Rectangle getBoundingBox() const;
	/// @brief スロットの境界矩形を算出
	Rectangle getBoundingBoxOfSlot(const char* slotName, size_t nameLength, bool* found = nullptr) const;
private:
	struct RayLibVertex2D
	{
		Vector2 pos;
		Color color;
		Vector2 tex;
	};

	bool m_isAlphaPremultiplied = true;
	bool m_isToForceBlendModeNormal = false;
	bool m_isVisible = true;
	bool m_isPaused = false;
	Physics m_physics = Physics::Update;

	spine::Skeleton* m_skeleton = nullptr;
	spine::AnimationState* m_animationState = nullptr;

	spine::Vector<float> m_worldVertices;
	spine::Vector<unsigned short> m_quadIndices;
	spine::Vector<RayLibVertex2D> m_raylibVertices;

	spine::SkeletonClipping m_skeletonClipping;

	spine::Vector<spine::String> m_slotsToExclude;

	bool isSlotToBeExcluded(const spine::String& slotName) noexcept;
	bool (*m_pSlotExcludeCallback)(const char*, size_t) = nullptr;
};

#endif // RAYLIB_SPINE_DRAWABLE_H_
