#ifndef RAYLIB_RENDER_TARGET_SCOPE_H_
#define RAYLIB_RENDER_TARGET_SCOPE_H_

#include <raylib.h>

class CRaylibRenderTargetScope
{
public:
	CRaylibRenderTargetScope(const RenderTexture2D& renderTexture, bool toClear = true, Color clearColor = BLANK);
	~CRaylibRenderTargetScope();

private:
};

#endif // !RAYLIB_RENDER_TARGET_SCOPE_H_
