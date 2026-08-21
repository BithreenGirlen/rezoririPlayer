
/* To calculate bounding box */
#include <float.h>

#include "raylib_spine_drawable.h"

#include <rlgl.h>

#if defined(SPINE_42)
static_assert(static_cast<spine::Physics>(CRaylibSpineDrawable::Physics::None) == spine::Physics::Physics_None, "Physics_None");
static_assert(static_cast<spine::Physics>(CRaylibSpineDrawable::Physics::Reset) == spine::Physics::Physics_Reset, "Physics_Reset");
static_assert(static_cast<spine::Physics>(CRaylibSpineDrawable::Physics::Update) == spine::Physics::Physics_Update, "Physics_Update");
static_assert(static_cast<spine::Physics>(CRaylibSpineDrawable::Physics::Pose) == spine::Physics::Physics_Pose, "Physics_Pose");
#endif

CRaylibSpineDrawable::CRaylibSpineDrawable(spine::SkeletonData* pSkeletonData)
{
	spine::Bone::setYDown(true);

	m_skeleton = new spine::Skeleton(pSkeletonData);
	spine::AnimationStateData* pAnimationStateData = new spine::AnimationStateData(pSkeletonData);
	m_animationState = new spine::AnimationState(pAnimationStateData);

	m_quadIndices.add(0);
	m_quadIndices.add(1);
	m_quadIndices.add(2);
	m_quadIndices.add(2);
	m_quadIndices.add(3);
	m_quadIndices.add(0);
}

CRaylibSpineDrawable::~CRaylibSpineDrawable()
{
	if (m_animationState != nullptr)
	{
		spine::AnimationStateData* pAnimationStateData = m_animationState->getData();
		delete pAnimationStateData;

		delete m_animationState;
	}
	if (m_skeleton != nullptr)
	{
		delete m_skeleton;
	}
}

spine::Skeleton* CRaylibSpineDrawable::skeleton() const noexcept
{
	return m_skeleton;
}

spine::AnimationState* CRaylibSpineDrawable::animationState() const noexcept
{
	return m_animationState;
}

void CRaylibSpineDrawable::premultiplyAlpha(bool premultiplied) noexcept
{
	m_isAlphaPremultiplied = premultiplied;
}

bool CRaylibSpineDrawable::isAlphaPremultiplied() const noexcept
{
	return m_isAlphaPremultiplied;
}

void CRaylibSpineDrawable::forceBlendModeNormal(bool toForce) noexcept
{
	m_isToForceBlendModeNormal = toForce;
}

bool CRaylibSpineDrawable::isBlendModeNormalForced() const noexcept
{
	return m_isToForceBlendModeNormal;
}

void CRaylibSpineDrawable::setPause(bool paused) noexcept
{
	m_isPaused = paused;
}

bool CRaylibSpineDrawable::isPaused() const noexcept
{
	return m_isPaused;
}

void CRaylibSpineDrawable::setVisibility(bool visible) noexcept
{
	m_isVisible = visible;
}

bool CRaylibSpineDrawable::isVisible() const noexcept
{
	return m_isVisible;
}

void CRaylibSpineDrawable::setPhysics(Physics physics)
{
	m_physics = physics;
}

CRaylibSpineDrawable::Physics CRaylibSpineDrawable::getPhysics() const noexcept
{
	return m_physics;
}

void CRaylibSpineDrawable::update(float fDelta)
{
	if (m_skeleton != nullptr && m_animationState != nullptr)
	{
		if (!m_isPaused)m_animationState->update(fDelta);
		m_animationState->apply(*m_skeleton);

		/* Spine 4.1 does not have Skeleton::update() */
#if !defined(SPINE_41)
		if (!m_isPaused)m_skeleton->update(fDelta);
#endif

#if defined(SPINE_42)
		m_skeleton->updateWorldTransform(static_cast<spine::Physics>(m_physics));
#else
		m_skeleton->updateWorldTransform();
#endif
	}
}

void CRaylibSpineDrawable::draw(float fScale, float fOffsetX, float fOffsetY)
{
	if (m_skeleton == nullptr || m_animationState == nullptr)return;

	if (m_skeleton->getColor().a == 0) return;

	for (size_t i = 0; i < m_skeleton->getSlots().size(); ++i)
	{
		spine::Slot& slot = *m_skeleton->getDrawOrder()[i];
		spine::Attachment* pAttachment = slot.getAttachment();

		if (pAttachment == nullptr || slot.getColor().a == 0 || !slot.getBone().isActive())
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		if (isSlotToBeExcluded(slot.getData().getName()))
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		spine::Vector<float>* pVertices = &m_worldVertices;
		spine::Vector<float>* pAttachmentUvs = nullptr;
		spine::Vector<unsigned short>* pIndices = nullptr;

		spine::Color* pAttachmentColor = nullptr;

		Texture2D* pRaylibTexture = nullptr;

		if (pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			spine::RegionAttachment* pRegionAttachment = static_cast<spine::RegionAttachment*>(pAttachment);
			pAttachmentColor = &pRegionAttachment->getColor();

			if (pAttachmentColor->a == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}

			m_worldVertices.setSize(8, 0);
#if defined (SPINE_41) || defined (SPINE_42)
			pRegionAttachment->computeWorldVertices(slot, m_worldVertices, 0, 2);
#else
			pRegionAttachment->computeWorldVertices(slot.getBone(), m_worldVertices, 0, 2);
#endif
			pAttachmentUvs = &pRegionAttachment->getUVs();
			pIndices = &m_quadIndices;
#if defined (SPINE_41) || defined (SPINE_42)
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pRegionAttachment->getRegion());

			m_isAlphaPremultiplied = pAtlasRegion->page->pma;
			pRaylibTexture = reinterpret_cast<Texture2D*>(static_cast<spine::AtlasRegion*>(pRegionAttachment->getRegion())->rendererObject);
#else
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pRegionAttachment->getRendererObject());
#ifdef SPINE_40
			m_isAlphaPremultiplied = pAtlasRegion->page->pma;
#endif
			pRaylibTexture = reinterpret_cast<Texture2D*>(static_cast<spine::AtlasRegion*>(pRegionAttachment->getRendererObject())->page->getRendererObject());
#endif
		}
		else if (pAttachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			spine::MeshAttachment* pMeshAttachment = static_cast<spine::MeshAttachment*>(pAttachment);
			pAttachmentColor = &pMeshAttachment->getColor();

			if (pAttachmentColor->a == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}
			m_worldVertices.setSize(pMeshAttachment->getWorldVerticesLength(), 0);
			pMeshAttachment->computeWorldVertices(slot, 0, pMeshAttachment->getWorldVerticesLength(), m_worldVertices, 0, 2);
			pAttachmentUvs = &pMeshAttachment->getUVs();
			pIndices = &pMeshAttachment->getTriangles();

#if defined (SPINE_41) || defined (SPINE_42)
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pMeshAttachment->getRegion());

			m_isAlphaPremultiplied = pAtlasRegion->page->pma;
			pRaylibTexture = reinterpret_cast<Texture2D*>(static_cast<spine::AtlasRegion*>(pMeshAttachment->getRegion())->rendererObject);
#else
			spine::AtlasRegion* pAtlasRegion = static_cast<spine::AtlasRegion*>(pMeshAttachment->getRendererObject());
#ifdef SPINE_40
			m_isAlphaPremultiplied = pAtlasRegion->page->pma;
#endif
			pRaylibTexture = reinterpret_cast<Texture2D*>(static_cast<spine::AtlasRegion*>(pMeshAttachment->getRendererObject())->page->getRendererObject());
#endif
		}
		else if (pAttachment->getRTTI().isExactly(spine::ClippingAttachment::rtti))
		{
			spine::ClippingAttachment* pClippingAttachment = static_cast<spine::ClippingAttachment*>(slot.getAttachment());
			m_skeletonClipping.clipStart(slot, pClippingAttachment);
			continue;
		}
		else
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		if (pRaylibTexture == nullptr)
		{
			m_skeletonClipping.clipEnd(slot);
			continue;
		}

		if (m_skeletonClipping.isClipping())
		{
			m_skeletonClipping.clipTriangles(m_worldVertices, *pIndices, *pAttachmentUvs, 2);
			if (m_skeletonClipping.getClippedTriangles().size() == 0)
			{
				m_skeletonClipping.clipEnd(slot);
				continue;
			}
			pVertices = &m_skeletonClipping.getClippedVertices();
			pAttachmentUvs = &m_skeletonClipping.getClippedUVs();
			pIndices = &m_skeletonClipping.getClippedTriangles();
		}

		const spine::Color tint
		{
			m_skeleton->getColor().r * slot.getColor().r * pAttachmentColor->r,
			m_skeleton->getColor().g * slot.getColor().g * pAttachmentColor->g,
			m_skeleton->getColor().b * slot.getColor().b * pAttachmentColor->b,
			m_skeleton->getColor().a * slot.getColor().a * pAttachmentColor->a,
		};

		m_raylibVertices.setSize(pIndices->size(), {});
		for (int ii = 0; ii < pIndices->size(); ++ii)
		{
			auto& raylibVertex2d = m_raylibVertices[ii];

			raylibVertex2d.pos.x = (*pVertices)[(*pIndices)[ii] * 2LL] * fScale + fOffsetX;
			raylibVertex2d.pos.y = (*pVertices)[(*pIndices)[ii] * 2LL + 1] * fScale + fOffsetY;

			raylibVertex2d.color.r = static_cast<unsigned char>(tint.r * 255.f * (m_isAlphaPremultiplied ? tint.a : 1.f));
			raylibVertex2d.color.g = static_cast<unsigned char>(tint.g * 255.f * (m_isAlphaPremultiplied ? tint.a : 1.f));
			raylibVertex2d.color.b = static_cast<unsigned char>(tint.b * 255.f * (m_isAlphaPremultiplied ? tint.a : 1.f));
			raylibVertex2d.color.a = static_cast<unsigned char>(tint.a * 255.f);

			raylibVertex2d.tex.x = (*pAttachmentUvs)[(*pIndices)[ii] * 2LL];
			raylibVertex2d.tex.y = (*pAttachmentUvs)[(*pIndices)[ii] * 2LL + 1];
		}

		BlendMode raylibBlendMode;
		spine::BlendMode spineBlendMode = m_isToForceBlendModeNormal ? spine::BlendMode::BlendMode_Normal : slot.getData().getBlendMode();
		switch (spineBlendMode)
		{
		case spine::BlendMode::BlendMode_Additive:
			if (m_isAlphaPremultiplied)
			{
				::rlSetBlendFactors(RL_ONE, RL_ONE, RL_FUNC_ADD);
				raylibBlendMode = BlendMode::BLEND_CUSTOM;
			}
			else
			{
				raylibBlendMode = BlendMode::BLEND_ADDITIVE;
			}
			break;
		case spine::BlendMode::BlendMode_Multiply:
			raylibBlendMode = BlendMode::BLEND_MULTIPLIED;
			break;
		case spine::BlendMode::BlendMode_Screen:
			::rlSetBlendFactorsSeparate(RL_ONE, RL_ONE_MINUS_SRC_COLOR, RL_FUNC_ADD, RL_ONE, RL_ONE_MINUS_SRC_ALPHA, RL_FUNC_ADD);
			raylibBlendMode = BlendMode::BLEND_CUSTOM_SEPARATE;
			break;
		default: /* spine::BlendMode::BlendMode_Normal */
			raylibBlendMode = m_isAlphaPremultiplied ? BlendMode::BLEND_ALPHA_PREMULTIPLY : BlendMode::BLEND_ALPHA;
			break;
		}

		::rlSetBlendMode(raylibBlendMode);
		::rlSetTexture(pRaylibTexture->id);
		/* rlTexCoord2f is limited to RL_QUADS */
		::rlBegin(RL_QUADS);

		/* Triangles to Quads. */
		for (int ii = 0; ii < m_raylibVertices.size(); ii += 3)
		{
			for (int k = 2; k >= 0; --k)
			{
				auto& raylibVertex2d = m_raylibVertices[static_cast<size_t>(ii + k)];

				::rlColor4ub(raylibVertex2d.color.r, raylibVertex2d.color.g, raylibVertex2d.color.b, raylibVertex2d.color.a);
				::rlTexCoord2f(raylibVertex2d.tex.x, raylibVertex2d.tex.y);
				::rlVertex2f(raylibVertex2d.pos.x, raylibVertex2d.pos.y);
			}

			{
				auto& raylibVertex2d = m_raylibVertices[ii];
				::rlColor4ub(raylibVertex2d.color.r, raylibVertex2d.color.g, raylibVertex2d.color.b, raylibVertex2d.color.a);
				::rlTexCoord2f(raylibVertex2d.tex.x, raylibVertex2d.tex.y);
				::rlVertex2f(raylibVertex2d.pos.x, raylibVertex2d.pos.y);
			}
		}

		::rlEnd();
		::rlSetTexture(0);

		m_skeletonClipping.clipEnd(slot);
	}

	::rlSetBlendMode(BlendMode::BLEND_ALPHA);

	m_skeletonClipping.clipEnd();
}

void CRaylibSpineDrawable::setSlotsToExclude(spine::Vector<spine::String>& slotNames)
{
	m_slotsToExclude = std::move(slotNames);
}

Rectangle CRaylibSpineDrawable::getBoundingBox() const
{
	Rectangle boundingBox{};

	if (m_skeleton != nullptr)
	{
		spine::Vector<float> tempVertices;
		m_skeleton->getBounds(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, tempVertices);
	}

	return boundingBox;
}

Rectangle CRaylibSpineDrawable::getBoundingBoxOfSlot(const char* slotName, size_t nameLength, bool* found) const
{
	float fMinX = FLT_MAX;
	float fMinY = FLT_MAX;
	float fMaxX = -FLT_MAX;
	float fMaxY = -FLT_MAX;

	if (m_skeleton != nullptr)
	{
		for (size_t i = 0; i < m_skeleton->getSlots().size(); ++i)
		{
			spine::Slot& slot = *m_skeleton->getDrawOrder()[i];
			const spine::String& slotDataName = slot.getData().getName();
			if (nameLength != slotDataName.length())continue;

			if (::memcmp(slotDataName.buffer(), slotName, slotDataName.length()) == 0)
			{
				spine::Attachment* pAttachment = slot.getAttachment();
				if (pAttachment != nullptr)
				{
					spine::Vector<float> tempVertices;
					if (pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
					{
						spine::RegionAttachment* pRegionAttachment = static_cast<spine::RegionAttachment*>(pAttachment);

						tempVertices.setSize(8, 0);
#if defined (SPINE_41) || defined (SPINE_42)
						pRegionAttachment->computeWorldVertices(slot, tempVertices, 0, 2);
#else
						pRegionAttachment->computeWorldVertices(slot.getBone(), tempVertices, 0, 2);
#endif
					}
					else if (pAttachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
					{
						spine::MeshAttachment* pMeshAttachment = static_cast<spine::MeshAttachment*>(pAttachment);
						tempVertices.setSize(pMeshAttachment->getWorldVerticesLength(), 0);
						pMeshAttachment->computeWorldVertices(slot, 0, pMeshAttachment->getWorldVerticesLength(), tempVertices, 0, 2);
					}
					else
					{
						continue;
					}

					for (size_t ii = 0; ii < tempVertices.size(); ii += 2)
					{
						float fX = tempVertices[ii];
						float fY = tempVertices[ii + 1LL];

						fMinX = fMinX < fX ? fMinX : fX;
						fMinY = fMinY < fY ? fMinY : fY;
						fMaxX = fMaxX > fX ? fMaxX : fX;
						fMaxY = fMaxY > fY ? fMaxY : fY;
					}

					if (found != nullptr)*found = true;
					break;
				}
			}
		}
	}

	return Rectangle{ fMinX, fMinY, fMaxX - fMinX, fMaxY - fMinY };
}

bool CRaylibSpineDrawable::isSlotToBeExcluded(const spine::String& slotName) noexcept
{
	if (m_pSlotExcludeCallback != nullptr)
	{
		return m_pSlotExcludeCallback(slotName.buffer(), slotName.length());
	}
	else
	{
		return m_slotsToExclude.contains(slotName);
	}

	return false;
}
