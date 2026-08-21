

#include "raylib_text_drawer.h"
#include "raylib_render_target_scope.h"

void CRaylibTextDrawer::setFont(Font& font, float fontSize, int thickness, float spacing) noexcept
{
	m_storedFont = &font;
	m_fontSize = fontSize;
	m_thickness = thickness;
	m_spacing = spacing;

	static constexpr const char s_fragmentShaderCode[] =
		R"(
			#version 330

			in vec2 fragTexCoord;
			in vec4 fragColor;
			
			uniform sampler2D texture0;
			uniform vec4 colDiffuse;

			uniform float outlineSize;
			uniform vec4 outlineColor;
			uniform vec2 textureSize;

			out vec4 finalColor;

			void main()
			{
			    vec4 texel = texture(texture0, fragTexCoord);
			    vec2 texelSize = 1.0 / textureSize;

			    float maxAlpha = 0.0;
			    float r = outlineSize;

			    for (float x = -r; x <= r; x += 1.0)
			    {
			        for (float y = -r; y <= r; y += 1.0)
			        {
			            if (x*x + y*y <= r*r)
			            {
			                vec2 offset = vec2(x, y) * texelSize;
			                maxAlpha = max(maxAlpha, texture(texture0, fragTexCoord + offset).a);
			            }
			        }
			    }
				vec4 textElement = texel * colDiffuse;
			    vec4 blendedColor = mix(outlineColor, textElement, texel.a);
			    blendedColor.a = maxAlpha;

			    finalColor = blendedColor;
			}
		)";

	if (!m_outlineShader.isHandleValid())
	{
		m_outlineShader.reset(::LoadShaderFromMemory(nullptr, s_fragmentShaderCode));
	}
}

void CRaylibTextDrawer::recreateTextRenderTexture(const char* text) noexcept
{
	if (m_storedFont == nullptr)return;

	Vector2 textSize = ::MeasureTextEx(*m_storedFont, text, m_fontSize, m_spacing);
	m_textRenderTexture.reset(
		::LoadRenderTexture(
			static_cast<int>(textSize.x + m_thickness * 2),
			static_cast<int>(textSize.y + m_thickness * 2)
		));

	const float thickness = static_cast<float>(m_thickness);
	const Vector2 textPos = { thickness, thickness };
	const Color fillColour = m_isTextColourReversed ? WHITE : BLACK;

	CRaylibRenderTargetScope renderTargetScope(m_textRenderTexture.get());

	::BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
	::DrawTextEx(*m_storedFont, text, textPos, m_fontSize, m_spacing, fillColour);
	::EndBlendMode();
}

void CRaylibTextDrawer::renderTextRenderTexture(float posX, float posY) const noexcept
{
	if (!m_isTextVisible)return;

	const Texture& texture = m_textRenderTexture.get().texture;
	const Rectangle srcRec =
	{
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(texture.width),
		.height = -static_cast<float>(texture.height)
	};
	const Rectangle destRec =
	{
		.x = posX,
		.y = posY,
		.width = static_cast<float>(texture.width),
		.height = static_cast<float>(texture.height)
	};

	const int outlineSizeLocation = ::GetShaderLocation(m_outlineShader.get(), "outlineSize");
	const int outlineColorLocation = ::GetShaderLocation(m_outlineShader.get(), "outlineColor");
	const int textureSizeLocation = ::GetShaderLocation(m_outlineShader.get(), "textureSize");

	const float thickness = static_cast<float>(m_thickness);
	const Color outlineColour = m_isTextColourReversed ? BLACK : WHITE;
	const Color fillColour = m_isTextColourReversed ? WHITE : BLACK;
	const float colours[4] =
	{
		outlineColour.r / 255.f,
		outlineColour.g / 255.f,
		outlineColour.b / 255.f,
		outlineColour.a / 255.f
	};
	const Vector2 textutreSize =
	{
		static_cast<float>(m_textRenderTexture.get().texture.width),
		static_cast<float>(m_textRenderTexture.get().texture.height)
	};

	::SetShaderValue(m_outlineShader.get(), outlineSizeLocation, &thickness, SHADER_UNIFORM_FLOAT);
	::SetShaderValue(m_outlineShader.get(), outlineColorLocation, colours, SHADER_UNIFORM_VEC4);
	::SetShaderValue(m_outlineShader.get(), textureSizeLocation, &textutreSize, SHADER_UNIFORM_VEC2);

	::BeginShaderMode(m_outlineShader.get());
	::DrawTexturePro(texture, srcRec, destRec, {}, 0.f, WHITE);
	::EndShaderMode();
}

void CRaylibTextDrawer::toggleTextColour() noexcept
{
	m_isTextColourReversed ^= true;
}

void CRaylibTextDrawer::setTextVisibility(bool visible) noexcept
{
	m_isTextVisible = visible;
}

bool CRaylibTextDrawer::isTextVisible() const noexcept
{
	return m_isTextVisible;
}

void CRaylibTextDrawer::getTextTextureSize(int* width, int* height) const noexcept
{
	const Texture& texture = m_textRenderTexture.get().texture;

	if (width != nullptr)*width = texture.width;
	if (height != nullptr)*height = texture.height;
}

float CRaylibTextDrawer::getFontSize() const noexcept
{
	return m_fontSize;
}

int CRaylibTextDrawer::getFontThickness() const noexcept
{
	return m_thickness;
}

float CRaylibTextDrawer::getFontSpacing() const noexcept
{
	return m_spacing;
}
