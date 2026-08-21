

#include "raylib_render_target_scope.h"

CRaylibRenderTargetScope::CRaylibRenderTargetScope(const RenderTexture2D& renderTexture, bool toClear, Color clearColor)
{
	::BeginTextureMode(renderTexture);
	if (toClear)
	{
		::ClearBackground(clearColor);
	}
}

CRaylibRenderTargetScope::~CRaylibRenderTargetScope()
{
	::EndTextureMode();
}
